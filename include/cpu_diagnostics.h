#ifndef CPU_READER_DIAGNOSTICS_H
#define CPU_READER_DIAGNOSTICS_H
#include "cpu.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
  CPU_DIAG_OK = 0,
  CPU_DIAG_BASELINE,
  CPU_DIAG_UNAVAILABLE,
  CPU_DIAG_ERROR
} cpu_diagnostic_status_t;
typedef struct {
  cpu_diagnostic_status_t status;
  double value;
  cpu_error_t error_code;
  /* Selected procfs/sysfs path, or source-family fallback if none was selected. */
  char source[4096];
  /* Thermal-zone type or explicit override; empty for non-thermal metrics. */
  char sensor_type[128];
  char message[256];
} cpu_diagnostic_metric_t;
typedef struct {
  int info_available;
  char model[256];
  int logical_processors;
  int physical_cores;
  char flags[512];
  cpu_error_info_t info_error;
  cpu_diagnostic_metric_t usage_percent;
  cpu_diagnostic_metric_t temperature_c;
  cpu_diagnostic_metric_t frequency_mhz;
  cpu_diagnostic_metric_t runnable_processes;
} cpu_diagnostic_report_t;
/* Returns 0 if assembled, even with unavailable metrics; -1 for invalid args. */
int cpu_get_diagnostic_report(cpu_diagnostic_report_t *report,
                              cpu_error_info_t *error);
#ifdef __cplusplus
}
#endif
#endif
