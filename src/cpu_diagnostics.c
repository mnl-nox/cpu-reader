#define _POSIX_C_SOURCE 200809L
#include "cpu_diagnostics.h"
#include "cpu_internal.h"
#include <stdio.h>
#include <string.h>

static void metric_init(cpu_diagnostic_metric_t *metric, const char *source) {
  memset(metric, 0, sizeof(*metric));
  metric->status = CPU_DIAG_UNAVAILABLE;
  metric->value = -1.0;
  metric->error_code = CPU_ERROR_UNSUPPORTED;
  snprintf(metric->source, sizeof(metric->source), "%s", source);
  snprintf(metric->message, sizeof(metric->message), "%s", "Metric not sampled yet");
}
static void metric_capture(cpu_diagnostic_metric_t *metric, double value,
                           int unavailable_sentinel,
                           const cpu_error_info_t *error) {
  metric->value = value;
  metric->error_code = error->code;
  snprintf(metric->message, sizeof(metric->message), "%s", error->message);
  if (unavailable_sentinel) {
    metric->status = error->code == CPU_ERROR_UNSUPPORTED ? CPU_DIAG_UNAVAILABLE : CPU_DIAG_ERROR;
  } else {
    metric->status = CPU_DIAG_OK;
    metric->error_code = CPU_ERROR_NONE;
    metric->message[0] = '\0';
  }
}
int cpu_get_diagnostic_report(cpu_diagnostic_report_t *report, cpu_error_info_t *error) {
  cpu_error_info_t operation_error;
  cpu_info_t *info;
  cpu_usage_context_t usage_context;
  float value;
  if (report == NULL) {
    cpu_error_info_clear(error);
    if (error != NULL) {
      error->code = CPU_ERROR_INVALID_ARGUMENT;
      snprintf(error->message, sizeof(error->message), "%s", "Diagnostic report pointer must not be NULL");
    }
    return -1;
  }
  memset(report, 0, sizeof(*report));
  cpu_error_info_clear(error);
  cpu_error_info_clear(&operation_error);
  metric_init(&report->usage_percent, "/proc/stat (aggregate counters)");
  metric_init(&report->temperature_c, "/sys/class/thermal (CPU-typed zone) or explicit override");
  metric_init(&report->frequency_mhz, "cpufreq sysfs with /proc/cpuinfo fallback");
  metric_init(&report->runnable_processes, "/proc/loadavg");
  info = cpu_get_info_ex(&operation_error);
  if (info != NULL) {
    report->info_available = 1;
    snprintf(report->model, sizeof(report->model), "%s", info->model);
    snprintf(report->flags, sizeof(report->flags), "%s", info->flags);
    report->logical_processors = info->logical_processors;
    report->physical_cores = info->physical_cores;
    cpu_free_info(info);
    cpu_error_info_clear(&report->info_error);
  } else {
    report->info_available = 0;
    report->info_error = operation_error;
  }
  cpu_error_info_clear(&operation_error);
  value = cpu_get_temperature_ex(&operation_error);
  metric_capture(&report->temperature_c, value, value < 0.0f, &operation_error);
  cpu_get_temperature_source(report->temperature_c.source, sizeof(report->temperature_c.source));
  cpu_error_info_clear(&operation_error);
  value = cpu_get_clock_speed_ex(&operation_error);
  metric_capture(&report->frequency_mhz, value, value < 0.0f, &operation_error);
  cpu_get_frequency_source(report->frequency_mhz.source, sizeof(report->frequency_mhz.source));
  cpu_error_info_clear(&operation_error);
  {
    int processes = cpu_get_active_processes_ex(&operation_error);
    metric_capture(&report->runnable_processes, (double)processes, processes < 0, &operation_error);
  }
  cpu_error_info_clear(&operation_error);
  if (cpu_usage_context_init_ex(&usage_context, &operation_error) == 0) {
    value = cpu_get_usage_context_ex(&usage_context, &operation_error);
    if (value >= 0.0f) {
      report->usage_percent.value = value;
      report->usage_percent.status = CPU_DIAG_BASELINE;
      report->usage_percent.error_code = CPU_ERROR_NONE;
      snprintf(report->usage_percent.message, sizeof(report->usage_percent.message),
               "%s", "Baseline sample only; call again after an interval");
    } else {
      metric_capture(&report->usage_percent, value, 1, &operation_error);
    }
    cpu_usage_context_cleanup(&usage_context);
  } else {
    metric_capture(&report->usage_percent, -1.0, 1, &operation_error);
  }
  cpu_error_info_clear(error);
  return 0;
}
