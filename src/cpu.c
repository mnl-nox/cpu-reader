#include "cpu_internal.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

/* GCC and Clang provide thread-local storage as an extension in C99 mode. */
#if defined(__GNUC__) || defined(__clang__)
#define CPU_THREAD_LOCAL __thread
#else
#error "cpu-reader requires GCC or Clang thread-local storage support"
#endif
static CPU_THREAD_LOCAL cpu_usage_context_t default_usage_context;
static CPU_THREAD_LOCAL cpu_error_t last_error_code;
static CPU_THREAD_LOCAL char last_error_message[256];
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

cpu_info_t *cpu_get_info(void) {
  cpu_info_t *info = cpu_read_info();

  if (info == NULL) {
    return NULL;
  }

  info->active_processes = cpu_read_active_processes(0);
  info->temperature_c = cpu_read_temperature(0);
  if (info->current_frequency_mhz <= 0.0f) {
    info->current_frequency_mhz = cpu_read_clock_speed(0);
  }
  if (info->current_frequency_mhz <= 0.0f) {
    /* Frequency telemetry is optional on some architectures and virtualized
     * systems. Keep the snapshot useful when no clock source is available. */
    info->current_frequency_mhz = -1.0f;
  }
  info->frequency_mhz = info->current_frequency_mhz;
  cpu_clear_last_error();
  return info;
}

void cpu_free_info(cpu_info_t *info) { free(info); }

cpu_error_t cpu_get_last_error_code(void) { return last_error_code; }

const char *cpu_get_last_error(void) { return last_error_message; }

/* Copy the legacy thread-local diagnostic into caller-owned storage. This
 * snapshot has a stable lifetime independent of subsequent API calls. */
void cpu_error_info_clear(cpu_error_info_t *error) {
  if (error != NULL) {
    error->code = CPU_ERROR_NONE;
    error->message[0] = '\0';
  }
}

static void cpu_capture_error(cpu_error_info_t *error) {
  if (error != NULL) {
    error->code = last_error_code;
    snprintf(error->message, sizeof(error->message), "%s", last_error_message);
  }
}

int cpu_init_ex(cpu_error_info_t *error) {
  int result = cpu_init();
  cpu_capture_error(error);
  return result;
}

cpu_info_t *cpu_get_info_ex(cpu_error_info_t *error) {
  cpu_info_t *result = cpu_get_info();
  cpu_capture_error(error);
  return result;
}

float cpu_get_usage_ex(cpu_error_info_t *error) {
  float result = cpu_get_usage();
  cpu_capture_error(error);
  return result;
}

int cpu_usage_context_init_ex(cpu_usage_context_t *context,
                              cpu_error_info_t *error) {
  int result = cpu_usage_context_init(context);
  cpu_capture_error(error);
  return result;
}

float cpu_get_usage_context_ex(cpu_usage_context_t *context,
                               cpu_error_info_t *error) {
  float result = cpu_get_usage_context(context);
  cpu_capture_error(error);
  return result;
}

float cpu_get_temperature_ex(cpu_error_info_t *error) {
  float result = cpu_get_temperature();
  cpu_capture_error(error);
  return result;
}

float cpu_get_clock_speed_ex(cpu_error_info_t *error) {
  float result = cpu_get_clock_speed();
  cpu_capture_error(error);
  return result;
}

int cpu_get_active_processes_ex(cpu_error_info_t *error) {
  int result = cpu_get_active_processes();
  cpu_capture_error(error);
  return result;
}
