#include "cpu_internal.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

static cpu_usage_context_t default_usage_context;
static cpu_error_t last_error_code;
static char last_error_message[256];

/* Error state is process-global because the public API predates per-reader
 * objects; every failing public operation must set it before returning. */
void cpu_clear_last_error(void) {
  last_error_code = CPU_ERROR_NONE;
  last_error_message[0] = '\0';
}

void cpu_set_last_error(cpu_error_t code, const char *format, ...) {
  va_list arguments;

  last_error_code = code;
  va_start(arguments, format);
  vsnprintf(last_error_message, sizeof(last_error_message), format, arguments);
  va_end(arguments);
}

static const char *skip_space(const char *text) {
  while (*text != '\0' && isspace((unsigned char)*text)) {
    text++;
  }
  return text;
}

int cpu_parse_unsigned_long_long(const char *text,
                                 unsigned long long *value) {
  char *end;
  unsigned long long parsed;

  if (text == NULL || value == NULL) {
    return -1;
  }
  text = skip_space(text);
  if (*text == '-' || *text == '+') {
    return -1;
  }
  errno = 0;
  parsed = strtoull(text, &end, 10);
  if (errno == ERANGE || end == text || *skip_space(end) != '\0') {
    return -1;
  }
  *value = parsed;
  return 0;
}

int cpu_parse_long(const char *text, long *value) {
  char *end;
  long parsed;

  if (text == NULL || value == NULL) {
    return -1;
  }
  text = skip_space(text);
  errno = 0;
  parsed = strtol(text, &end, 10);
  if (errno == ERANGE || end == text || *skip_space(end) != '\0') {
    return -1;
  }
  *value = parsed;
  return 0;
}

int cpu_parse_float(const char *text, float *value) {
  char *end;
  float parsed;

  if (text == NULL || value == NULL) {
    return -1;
  }
  text = skip_space(text);
  errno = 0;
  parsed = strtof(text, &end);
  if (errno == ERANGE || end == text || *skip_space(end) != '\0' ||
      !isfinite(parsed)) {
    return -1;
  }
  *value = parsed;
  return 0;
}

int cpu_init(void) { return cpu_usage_context_init(&default_usage_context); }

void cpu_cleanup(void) { cpu_usage_context_cleanup(&default_usage_context); }

float cpu_get_usage(void) {
  return cpu_get_usage_context(&default_usage_context);
}

float cpu_get_temperature(void) { return cpu_read_temperature(1); }

float cpu_get_clock_speed(void) { return cpu_read_clock_speed(1); }

int cpu_get_active_processes(void) { return cpu_read_active_processes(1); }

void cpu_free_info(cpu_info_t *info) { free(info); }

cpu_error_t cpu_get_last_error_code(void) { return last_error_code; }

const char *cpu_get_last_error(void) { return last_error_message; }
