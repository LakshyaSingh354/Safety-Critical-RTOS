#pragma once
#include <stdint.h>
#include "../kernel/task.h"
#include "metrics_config.h"

#if METRICS_ENABLED
void metrics_init(void);
void metrics_poll(uint32_t now);
void metrics_on_release(task_t *t, uint32_t now, uint32_t expected_release);
void metrics_on_task_start(task_t *t, uint32_t start);
void metrics_on_task_end(task_t *t,
                         uint32_t start,
                         uint32_t end,
                         uint8_t wcet_violation,
                         uint8_t deadline_miss);
void metrics_on_idle(uint32_t before, uint32_t after);
#else
static inline void metrics_init(void) { }
static inline void metrics_poll(uint32_t now) { (void)now; }
static inline void metrics_on_release(task_t *t, uint32_t now, uint32_t expected_release) {
    (void)t;
    (void)now;
    (void)expected_release;
}
static inline void metrics_on_task_start(task_t *t, uint32_t start) {
    (void)t;
    (void)start;
}
static inline void metrics_on_task_end(task_t *t,
                                       uint32_t start,
                                       uint32_t end,
                                       uint8_t wcet_violation,
                                       uint8_t deadline_miss) {
    (void)t;
    (void)start;
    (void)end;
    (void)wcet_violation;
    (void)deadline_miss;
}
static inline void metrics_on_idle(uint32_t before, uint32_t after) {
    (void)before;
    (void)after;
}
#endif
