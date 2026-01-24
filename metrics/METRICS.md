# Metrics Module

This module provides isolated measurement and reporting for the RTOS
kernel, without modifying scheduling policy. It collects task-level and
system-level metrics, simulates controlled workloads, and emits a baseline
report over UART.

The module is controlled entirely through compile-time knobs in
`metrics/metrics_config.h`.

## Goals
- Measure latency, jitter, utilization, deadline misses, and WCET violations
- Provide an isolated instrumentation layer with minimal kernel changes
- Allow repeatable baseline measurement windows
- Support synthetic workload profiles to vary load

## Timing source
All timing is derived from `kernel_time_now()` which returns `kernel_ticks`
incremented by the timer ISR. Metrics are expressed in ticks. The current
configuration assumes a 0.5 ms tick and provides `US_TO_TICKS()` and
`MS_TO_TICKS()` helpers in `metrics/metrics_config.h`.

## Collected metrics

### Per-task metrics
- Release jitter: `actual_release - expected_release`
- Release-to-start latency: `start_time - actual_release`
- Execution time: `end_time - start_time`
- Response time: `end_time - actual_release`
- Deadline misses (count)
- WCET violations (count)
- CPU share (execution ticks / window ticks)

### System metrics
- CPU utilization: active vs idle ticks per window
- Total releases and executions
- Total deadline misses and WCET violations

## Integration points
The kernel is instrumented with non-invasive hooks:
- `kernel_release_tasks()` calls `metrics_on_release()` before updating state
- `kernel_execute_task()` calls `metrics_on_task_start()` and `metrics_on_task_end()`
- `kernel_idle()` calls `metrics_on_idle()` around WFI sleep
- `kernel_run()` calls `metrics_poll()` once per loop

## Reporting window
Metrics are gathered in a fixed window:
- Warmup period: `METRICS_WARMUP_TICKS`
- Window length: `METRICS_REPORT_PERIOD_TICKS`
- Report once or periodic: `METRICS_REPORT_ONCE`
- Reset on report: `METRICS_RESET_ON_REPORT`

If `METRICS_REPORT_ONCE` is set, the module emits one report and then stops
collecting to preserve the baseline.

## UART output
The report is emitted over UART using the existing driver. UART init is done
inside `metrics_init()` when `METRICS_OUTPUT_UART` is enabled.

Example report fields:
- Window start and end ticks
- CPU utilization, active/idle ticks
- Per-task jitter, latency, exec, response min/max/avg
- Per-task deadline misses and WCET violations

## Configuration knobs
All knobs live in `metrics/metrics_config.h`.

### Metrics toggles
- `METRICS_ENABLED`: enable/disable instrumentation
- `METRICS_MAX_TASKS`: maximum number of tracked tasks
- `METRICS_WARMUP_TICKS`: warmup before collection
- `METRICS_REPORT_PERIOD_TICKS`: measurement window size
- `METRICS_REPORT_ONCE`: emit a single report
- `METRICS_RESET_ON_REPORT`: reset window counters on report
- `METRICS_OUTPUT_UART`: enable UART output
 - `TICK_US`, `US_TO_TICKS()`, `MS_TO_TICKS()`: tick conversions

### Workload configuration
The baseline workload is described in the same config file:
- `WORKLOAD_TASK_COUNT`: number of workload tasks (1-3)
- `WORKLOAD_TASK{N}_*`: name, period, deadline, WCET, start offset, base iterations

## Workload profiles
Synthetic workload profiles vary load without changing task schedules.

Set `WORKLOAD_PROFILE` to one of:
- `WORKLOAD_PROFILE_STATIC`: fixed iterations per task
- `WORKLOAD_PROFILE_STEP`: alternate low/high load
- `WORKLOAD_PROFILE_RAMP`: ramp load over time
- `WORKLOAD_PROFILE_SPIKE`: periodic spikes

Profile parameters are also in `metrics/metrics_config.h`:
- Step: `WORKLOAD_STEP_PERIOD_TICKS`, `WORKLOAD_STEP_MULTIPLIER`
- Ramp: `WORKLOAD_RAMP_STEP_TICKS`, `WORKLOAD_RAMP_STEPS`
- Spike: `WORKLOAD_SPIKE_PERIOD_TICKS`, `WORKLOAD_SPIKE_MULTIPLIER`

## Usage
1. Configure the metrics knobs in `metrics/metrics_config.h`
2. Configure workload tasks in the same file
3. Build and run the firmware
4. Capture UART output to log the baseline report

To produce periodic reports instead of a one-shot baseline:
- Set `METRICS_REPORT_ONCE` to `0`

## Implementation details
- Per-task statistics are stored in fixed arrays indexed by task table order
- `METRICS_MAX_TASKS` limits the number of tracked tasks without heap use
- `metrics_on_release()` caches the release timestamp for latency/response
- `metrics_on_task_end()` updates exec/response statistics and violations
- `metrics_on_idle()` accumulates idle ticks for utilization

## Extending
If you need new metrics:
- Add fields to the per-task or system stats in `metrics.c`
- Update the hook in `execute.c`, `release.c`, or `idle.c`
- Extend `metrics_emit_report()` to output the new fields

## Notes and limitations
- All measurements are in ticks; ensure timer tick frequency is known
- Metrics are not stored persistently across resets
- UART output can affect timing if you enable frequent reporting
