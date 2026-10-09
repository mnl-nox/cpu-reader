#define _POSIX_C_SOURCE 200809L

#include "cpu_internal.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *proc_stat_path(void) {
  const char *path = getenv("CPU_READER_PROC_STAT_PATH");

  if (path == NULL || path[0] == '\0') {
    return "/proc/stat";
  }

  return path;
}

static int sum_cpu_counters(const unsigned long long counters[8],
                            unsigned long long *total) {
  unsigned long long accumulated = 0;
  size_t index;

  if (total == NULL) {
    return -1;
  }
  for (index = 0; index < 8; index++) {
    if (accumulated > ULLONG_MAX - counters[index]) {
      return -1;
    }
    accumulated += counters[index];
  }
  *total = accumulated;
  return 0;
}

/*
 * /proc/stat starts with at least eight counters. Newer Linux kernels append
 * guest and guest_nice (and may add more fields in the future). We use the
 * first eight counters for totals, but validate every additional token so
 * malformed trailing data cannot be accepted as a valid sample.
 */
static int parse_cpu_stat_line(char *line,
                               unsigned long long counters[8]) {
  const char *token;
  char *save = NULL;
  int index;

  token = strtok_r(line, " \t\r\n", &save);
  if (token == NULL || strcmp(token, "cpu") != 0) {
    return -1;
  }
  for (index = 0; index < 8; index++) {
    token = strtok_r(NULL, " \t\r\n", &save);
    if (token == NULL ||
        cpu_parse_unsigned_long_long(token, &counters[index]) != 0) {
      return -1;
    }
  }
  while ((token = strtok_r(NULL, " \t\r\n", &save)) != NULL) {
    unsigned long long ignored_counter;
    if (cpu_parse_unsigned_long_long(token, &ignored_counter) != 0) {
      return -1;
    }
  }
  return 0;
}

int cpu_usage_context_init(cpu_usage_context_t *context) {
  if (context == NULL) {
    cpu_set_last_error(CPU_ERROR_INVALID_ARGUMENT,
                       "Contexto de uso da CPU nao pode ser nulo");
    return -1;
  }

  context->previous_total = 0;
  context->previous_idle = 0;
  context->has_previous = 0;
  cpu_clear_last_error();
  return 0;
}

void cpu_usage_context_cleanup(cpu_usage_context_t *context) {
  if (context != NULL) {
    context->previous_total = 0;
    context->previous_idle = 0;
    context->has_previous = 0;
  }
}

float cpu_get_usage_context(cpu_usage_context_t *context) {
  FILE *file;
  unsigned long long user;
  unsigned long long nice;
  unsigned long long system;
  unsigned long long idle;
  unsigned long long iowait;
  unsigned long long irq;
  unsigned long long softirq;
  unsigned long long steal;
  unsigned long long total;
  unsigned long long idle_total;
  unsigned long long total_delta;
  unsigned long long idle_delta;
  unsigned long long counters[8];
  float usage;
  const char *path = proc_stat_path();

  if (context == NULL) {
    cpu_set_last_error(CPU_ERROR_INVALID_ARGUMENT,
                       "Contexto de uso da CPU nao pode ser nulo");
    return -1.0f;
  }

  file = fopen(path, "r");
  if (file == NULL) {
    cpu_set_last_error(CPU_ERROR_FILE_OPEN, "Nao foi possivel abrir %s: %s",
                       path, strerror(errno));
    return -1.0f;
  }

  {
    char line[1024];
    unsigned long long values[8];
    if (fgets(line, sizeof(line), file) == NULL ||
        strchr(line, '\n') == NULL) {
      fclose(file);
      cpu_set_last_error(CPU_ERROR_PARSE,
                         "Nao foi possivel interpretar a linha cpu em %s", path);
      return -1.0f;
    }
    if (parse_cpu_stat_line(line, values) != 0) {
      fclose(file);
      cpu_set_last_error(CPU_ERROR_PARSE,
                         "Nao foi possivel interpretar a linha cpu em %s", path);
      return -1.0f;
    }
    user = values[0];
    nice = values[1];
    system = values[2];
    idle = values[3];
    iowait = values[4];
    irq = values[5];
    softirq = values[6];
    steal = values[7];
  }

  fclose(file);

  counters[0] = user;
  counters[1] = nice;
  counters[2] = system;
  counters[3] = idle;
  counters[4] = iowait;
  counters[5] = irq;
  counters[6] = softirq;
  counters[7] = steal;
  if (sum_cpu_counters(counters, &total) != 0) {
    cpu_set_last_error(CPU_ERROR_PARSE,
                       "Overflow nos contadores de CPU em %s", path);
    return -1.0f;
  }
  if (idle > ULLONG_MAX - iowait) {
    cpu_set_last_error(CPU_ERROR_PARSE,
                       "Overflow nos contadores de ociosidade em %s", path);
    return -1.0f;
  }
  idle_total = idle + iowait;
  if (!context->has_previous) {
    context->previous_total = total;
    context->previous_idle = idle_total;
    context->has_previous = 1;
    cpu_clear_last_error();
    return 0.0f;
  }

  if (total < context->previous_total || idle_total < context->previous_idle) {
    context->previous_total = total;
    context->previous_idle = idle_total;
    cpu_clear_last_error();
    return 0.0f;
  }

  total_delta = total - context->previous_total;
  idle_delta = idle_total - context->previous_idle;
  context->previous_total = total;
  context->previous_idle = idle_total;

  if (total_delta == 0 || idle_delta > total_delta) {
    cpu_clear_last_error();
    return 0.0f;
  }

  usage = 100.0f * (float)(total_delta - idle_delta) / (float)total_delta;
  cpu_clear_last_error();
  return usage > 100.0f ? 100.0f : usage;
}
