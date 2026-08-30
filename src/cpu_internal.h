#ifndef CPU_READER_CPU_INTERNAL_H
#define CPU_READER_CPU_INTERNAL_H

#include "cpu.h"

void cpu_clear_last_error(void);
void cpu_set_last_error(cpu_error_t code, const char *format, ...);
float cpu_read_temperature(int report_errors);
float cpu_read_clock_speed(int report_errors);
int cpu_read_active_processes(int report_errors);

#endif
