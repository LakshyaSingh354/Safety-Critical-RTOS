#include "metrics.h"
#include "metrics_config.h"
#include "../kernel/kernel.h"

extern task_t tasks[];
extern const uint32_t task_count;

typedef struct {
    uint32_t release_count;
    uint32_t start_count;
    uint32_t exec_count;
    uint32_t release_jitter_min;
    uint32_t release_jitter_max;
    uint32_t release_jitter_sum;
    uint32_t start_latency_min;
    uint32_t start_latency_max;
    uint32_t start_latency_sum;
    uint32_t exec_time_min;
    uint32_t exec_time_max;
    uint32_t exec_time_sum;
    uint32_t response_time_min;
    uint32_t response_time_max;
    uint32_t response_time_sum;
    uint32_t deadline_misses;
    uint32_t wcet_violations;
    uint32_t last_release_time;
    uint32_t last_start_time;
    uint32_t last_finish_time;
    uint8_t has_release;
} metrics_task_stats_t;

typedef struct {
    uint32_t window_start;
    uint32_t window_end;
    uint32_t idle_ticks;
    uint32_t active_ticks;
    uint32_t release_events;
    uint32_t total_execs;
    uint32_t deadline_misses;
    uint32_t wcet_violations;
} metrics_system_stats_t;

static metrics_task_stats_t task_stats[METRICS_MAX_TASKS];
static metrics_system_stats_t system_stats;
static uint8_t metrics_active;
static uint8_t metrics_reported_once;

static uint32_t metrics_tracked_task_count(void) {
    return (task_count < METRICS_MAX_TASKS) ? task_count : METRICS_MAX_TASKS;
}

static void metrics_reset_task_stats(metrics_task_stats_t *s) {
    s->release_count = 0;
    s->start_count = 0;
    s->exec_count = 0;
    s->release_jitter_min = UINT32_MAX;
    s->release_jitter_max = 0;
    s->release_jitter_sum = 0;
    s->start_latency_min = UINT32_MAX;
    s->start_latency_max = 0;
    s->start_latency_sum = 0;
    s->exec_time_min = UINT32_MAX;
    s->exec_time_max = 0;
    s->exec_time_sum = 0;
    s->response_time_min = UINT32_MAX;
    s->response_time_max = 0;
    s->response_time_sum = 0;
    s->deadline_misses = 0;
    s->wcet_violations = 0;
    s->last_release_time = 0;
    s->last_start_time = 0;
    s->last_finish_time = 0;
    s->has_release = 0;
}

static void metrics_reset_window(uint32_t now) {
    system_stats.window_start = now;
    system_stats.window_end = now;
    system_stats.idle_ticks = 0;
    system_stats.active_ticks = 0;
    system_stats.release_events = 0;
    system_stats.total_execs = 0;
    system_stats.deadline_misses = 0;
    system_stats.wcet_violations = 0;

    uint32_t count = metrics_tracked_task_count();
    for (uint32_t i = 0; i < count; i++) {
        metrics_reset_task_stats(&task_stats[i]);
    }
}

static void metrics_update_min_max_sum(uint32_t value,
                                       uint32_t *min,
                                       uint32_t *max,
                                       uint32_t *sum) {
    if (*min == UINT32_MAX || value < *min) {
        *min = value;
    }
    if (value > *max) {
        *max = value;
    }
    *sum += value;
}

static uint32_t metrics_avg_u32(uint32_t sum, uint32_t count) {
    if (count == 0) {
        return 0;
    }
    return (uint32_t)(sum / count);
}

static int32_t metrics_task_index(task_t *t) {
    return (int32_t)(t - tasks);
}

#if METRICS_OUTPUT_UART
#include "../drivers/uart/uart.h"
#include "../drivers/uart/uart_rx_buf.h"
#include "../drivers/uart/uart_tx_buf.h"

static void metrics_uart_putc(char c) {
    uart_send_byte((uint8_t)c);
}

static void metrics_uart_puts(const char *s) {
    while (*s != '\0') {
        metrics_uart_putc(*s);
        s++;
    }
}

static void metrics_uart_put_u32(uint32_t value) {
    char buf[11];
    uint32_t i = 0;

    if (value == 0) {
        metrics_uart_putc('0');
        return;
    }

    while (value > 0 && i < sizeof(buf)) {
        buf[i++] = (char)('0' + (value % 10u));
        value /= 10u;
    }

    while (i > 0) {
        metrics_uart_putc(buf[--i]);
    }
}

static void metrics_uart_newline(void) {
    metrics_uart_putc('\r');
    metrics_uart_putc('\n');
}

static void metrics_emit_report(uint32_t now) {
    system_stats.window_end = now;
    uint32_t total_ticks = system_stats.window_end - system_stats.window_start;
    if (total_ticks == 0) {
        total_ticks = 1;
    }

    uint32_t utilization = (system_stats.active_ticks * 100u) / total_ticks;

    metrics_uart_newline();
    metrics_uart_puts("METRICS REPORT");
    metrics_uart_newline();
    metrics_uart_puts("Window ticks: ");
    metrics_uart_put_u32(system_stats.window_start);
    metrics_uart_puts(" -> ");
    metrics_uart_put_u32(system_stats.window_end);
    metrics_uart_newline();
    metrics_uart_puts("CPU util: ");
    metrics_uart_put_u32(utilization);
    metrics_uart_puts("% (active ");
    metrics_uart_put_u32(system_stats.active_ticks);
    metrics_uart_puts(", idle ");
    metrics_uart_put_u32(system_stats.idle_ticks);
    metrics_uart_puts(")");
    metrics_uart_newline();
    metrics_uart_puts("Releases: ");
    metrics_uart_put_u32(system_stats.release_events);
    metrics_uart_puts(", Executions: ");
    metrics_uart_put_u32(system_stats.total_execs);
    metrics_uart_puts(", Deadline misses: ");
    metrics_uart_put_u32(system_stats.deadline_misses);
    metrics_uart_puts(", WCET violations: ");
    metrics_uart_put_u32(system_stats.wcet_violations);
    metrics_uart_newline();

    uint32_t count = metrics_tracked_task_count();
    for (uint32_t i = 0; i < count; i++) {
        metrics_task_stats_t *s = &task_stats[i];
        task_t *t = &tasks[i];

        uint32_t rel_avg = metrics_avg_u32(s->release_jitter_sum, s->release_count);
        uint32_t start_avg = metrics_avg_u32(s->start_latency_sum, s->start_count);
        uint32_t exec_avg = metrics_avg_u32(s->exec_time_sum, s->exec_count);
        uint32_t resp_avg = metrics_avg_u32(s->response_time_sum, s->exec_count);

        uint32_t rel_min = (s->release_count == 0) ? 0 : s->release_jitter_min;
        uint32_t start_min = (s->start_count == 0) ? 0 : s->start_latency_min;
        uint32_t exec_min = (s->exec_count == 0) ? 0 : s->exec_time_min;
        uint32_t resp_min = (s->exec_count == 0) ? 0 : s->response_time_min;

        metrics_uart_newline();
        metrics_uart_puts("Task ");
        metrics_uart_puts(t->name);
        metrics_uart_puts(": releases ");
        metrics_uart_put_u32(s->release_count);
        metrics_uart_puts(", execs ");
        metrics_uart_put_u32(s->exec_count);
        metrics_uart_puts(", deadline misses ");
        metrics_uart_put_u32(s->deadline_misses);
        metrics_uart_puts(", wcet violations ");
        metrics_uart_put_u32(s->wcet_violations);
        metrics_uart_newline();

        metrics_uart_puts("  release jitter min/max/avg: ");
        metrics_uart_put_u32(rel_min);
        metrics_uart_puts("/");
        metrics_uart_put_u32(s->release_jitter_max);
        metrics_uart_puts("/");
        metrics_uart_put_u32(rel_avg);
        metrics_uart_newline();

        metrics_uart_puts("  start latency min/max/avg: ");
        metrics_uart_put_u32(start_min);
        metrics_uart_puts("/");
        metrics_uart_put_u32(s->start_latency_max);
        metrics_uart_puts("/");
        metrics_uart_put_u32(start_avg);
        metrics_uart_newline();

        metrics_uart_puts("  exec time min/max/avg: ");
        metrics_uart_put_u32(exec_min);
        metrics_uart_puts("/");
        metrics_uart_put_u32(s->exec_time_max);
        metrics_uart_puts("/");
        metrics_uart_put_u32(exec_avg);
        metrics_uart_newline();

        metrics_uart_puts("  response time min/max/avg: ");
        metrics_uart_put_u32(resp_min);
        metrics_uart_puts("/");
        metrics_uart_put_u32(s->response_time_max);
        metrics_uart_puts("/");
        metrics_uart_put_u32(resp_avg);
        metrics_uart_newline();

        uint32_t share = (total_ticks == 0) ? 0 : (uint32_t)((s->exec_time_sum * 100u) / total_ticks);
        metrics_uart_puts("  cpu share: ");
        metrics_uart_put_u32(share);
        metrics_uart_puts("%");
        metrics_uart_newline();
    }
}
#else
static void metrics_emit_report(uint32_t now) {
    (void)now;
}
#endif

void metrics_init(void) {
    metrics_active = 0;
    metrics_reported_once = 0;
    metrics_reset_window(0);
#if METRICS_OUTPUT_UART
    uart_tx_buffer_init();
    uart_rx_buffer_init();
    uart_init();
#endif
}

void metrics_poll(uint32_t now) {
    if (METRICS_REPORT_ONCE && metrics_reported_once) {
        return;
    }

    if (!metrics_active) {
        if (now >= METRICS_WARMUP_TICKS) {
            metrics_active = 1;
            metrics_reset_window(now);
        }
        return;
    }

    if ((now - system_stats.window_start) >= METRICS_REPORT_PERIOD_TICKS) {
        metrics_emit_report(now);
        if (METRICS_REPORT_ONCE) {
            metrics_reported_once = 1;
            metrics_active = 0;
        }
        if (METRICS_RESET_ON_REPORT) {
            metrics_reset_window(now);
        }
    }
}

void metrics_on_release(task_t *t, uint32_t now, uint32_t expected_release) {
    int32_t idx = metrics_task_index(t);
    if (idx < 0 || (uint32_t)idx >= metrics_tracked_task_count()) {
        return;
    }

    metrics_task_stats_t *s = &task_stats[idx];
    s->last_release_time = now;
    s->has_release = 1;

    if (!metrics_active) {
        return;
    }

    s->release_count++;
    system_stats.release_events++;

    uint32_t jitter = now - expected_release;
    metrics_update_min_max_sum(jitter,
                               &s->release_jitter_min,
                               &s->release_jitter_max,
                               &s->release_jitter_sum);
}

void metrics_on_task_start(task_t *t, uint32_t start) {
    int32_t idx = metrics_task_index(t);
    if (idx < 0 || (uint32_t)idx >= metrics_tracked_task_count()) {
        return;
    }

    metrics_task_stats_t *s = &task_stats[idx];
    s->last_start_time = start;

    if (!metrics_active) {
        return;
    }

    s->start_count++;
    if (s->has_release) {
        uint32_t latency = start - s->last_release_time;
        metrics_update_min_max_sum(latency,
                                   &s->start_latency_min,
                                   &s->start_latency_max,
                                   &s->start_latency_sum);
    }
}

void metrics_on_task_end(task_t *t,
                         uint32_t start,
                         uint32_t end,
                         uint8_t wcet_violation,
                         uint8_t deadline_miss) {
    int32_t idx = metrics_task_index(t);
    if (idx < 0 || (uint32_t)idx >= metrics_tracked_task_count()) {
        return;
    }

    metrics_task_stats_t *s = &task_stats[idx];
    s->last_finish_time = end;

    if (!metrics_active) {
        return;
    }

    uint32_t exec_time = end - start;
    s->exec_count++;
    system_stats.total_execs++;
    system_stats.active_ticks += exec_time;

    metrics_update_min_max_sum(exec_time,
                               &s->exec_time_min,
                               &s->exec_time_max,
                               &s->exec_time_sum);

    if (s->has_release) {
        uint32_t response = end - s->last_release_time;
        metrics_update_min_max_sum(response,
                                   &s->response_time_min,
                                   &s->response_time_max,
                                   &s->response_time_sum);
    }

    if (wcet_violation) {
        s->wcet_violations++;
        system_stats.wcet_violations++;
    }

    if (deadline_miss) {
        s->deadline_misses++;
        system_stats.deadline_misses++;
    }
}

void metrics_on_idle(uint32_t before, uint32_t after) {
    if (!metrics_active) {
        return;
    }
    system_stats.idle_ticks += (after - before);
}
