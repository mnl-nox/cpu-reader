#ifndef CPU_READER_CPU_INTERNAL_H
#define CPU_READER_CPU_INTERNAL_H

#include "cpu.h"

#include <stddef.h>

cpu_info_t *cpu_read_info(void);
void cpu_clear_last_error(void);
void cpu_set_last_error(cpu_error_t code, const char *format, ...);
/* Strict conversions shared by every /proc and sysfs reader. */
int cpu_parse_unsigned_long_long(const char *text,
                                 unsigned long long *value);
int cpu_parse_long(const char *text, long *value);
int cpu_parse_float(const char *text, float *value);
float cpu_read_temperature(int report_errors);
float cpu_read_clock_speed(int report_errors);
int cpu_read_active_processes(int report_errors);

#endif
