# Safety-Critical-RTOS

## Metrics and baseline reporting
This project includes an isolated metrics module with workload knobs for
baseline measurements. Controls live in `metrics/metrics_config.h`.

Collected metrics:
- Release jitter (actual release vs expected)
- Release-to-start latency
- Execution time (min/max/avg)
- Response time (release to finish)
- CPU utilization (active vs idle ticks)
- Deadline misses and WCET violations

Reporting:
- Metrics are gathered after `METRICS_WARMUP_TICKS`
- A report is emitted every `METRICS_REPORT_PERIOD_TICKS` ticks
- Output is over UART when `METRICS_OUTPUT_UART` is enabled

## Workload simulation
Workload tasks are defined by the config in `metrics/metrics_config.h` and
used by `kernel/task_table.c`. Select a profile with `WORKLOAD_PROFILE`:
- `WORKLOAD_PROFILE_STATIC`: fixed iterations per task
- `WORKLOAD_PROFILE_STEP`: alternate low/high load
- `WORKLOAD_PROFILE_RAMP`: ramp load over time
- `WORKLOAD_PROFILE_SPIKE`: periodic spikes

Adjust the per-task periods, deadlines, WCET thresholds, and iteration counts
to tune utilization and observe deadline or WCET behavior.
