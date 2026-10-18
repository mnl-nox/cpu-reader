#define _POSIX_C_SOURCE 200809L
#include "cpu_diagnostics.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(void) {
  char path[] = "/tmp/cpu-reader-diagnostics-XXXXXX";
  cpu_diagnostic_report_t report;
  cpu_error_info_t error;
  int fd = mkstemp(path);
  FILE *file;
  if (fd < 0) { perror("mkstemp"); return 1; }
  file = fdopen(fd, "w");
  if (file == NULL) { close(fd); unlink(path); perror("fdopen"); return 1; }
  if (fputs("42500\n", file) == EOF || fclose(file) != 0) {
    unlink(path); fprintf(stderr, "failed to write thermal fixture\n"); return 1;
  }
  setenv("CPU_READER_CPU_TEMP_PATH", path, 1);
  if (cpu_get_diagnostic_report(&report, &error) != 0) {
    unsetenv("CPU_READER_CPU_TEMP_PATH"); unlink(path);
    fprintf(stderr, "diagnostic report assembly failed: %s\n", error.message); return 1;
  }
  unsetenv("CPU_READER_CPU_TEMP_PATH");
  unlink(path);
  if (report.temperature_c.status != CPU_DIAG_OK ||
      report.temperature_c.value != 42.5 ||
      report.temperature_c.error_code != CPU_ERROR_NONE) {
    fprintf(stderr, "temperature diagnostic did not preserve the fixture value\n"); return 1;
  }
  if (report.usage_percent.status != CPU_DIAG_BASELINE) {
    fprintf(stderr, "single-snapshot usage must be labelled baseline, not load\n"); return 1;
  }
  if (cpu_get_diagnostic_report(NULL, &error) != -1 ||
      error.code != CPU_ERROR_INVALID_ARGUMENT) {
    fprintf(stderr, "NULL report must return an explicit argument error\n"); return 1;
  }
  puts("diagnostic report tests passed");
  return 0;
}
