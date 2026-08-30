#ifndef CPU_READER_CPU_H
#define CPU_READER_CPU_H

typedef enum
{
        CPU_ERROR_NONE = 0,
        CPU_ERROR_INVALID_ARGUMENT,
        CPU_ERROR_FILE_OPEN,
        CPU_ERROR_PARSE,
        CPU_ERROR_MEMORY,
        CPU_ERROR_UNSUPPORTED
} cpu_error_t;

typedef struct
{
        int cores;
        int threads;
        int active_processes;
        float frequency_mhz;
        float usage_percent;
        float temperature_c;
        char model[256];
        char flags[512];
} cpu_info_t;

typedef struct
{
        unsigned long long previous_total;
        unsigned long long previous_idle;
        int has_previous;
} cpu_usage_context_t;

int cpu_init(void);
void cpu_cleanup(void);
cpu_info_t *cpu_get_info(void);
float cpu_get_usage(void);
int cpu_usage_context_init(cpu_usage_context_t *context);
void cpu_usage_context_cleanup(cpu_usage_context_t *context);
float cpu_get_usage_context(cpu_usage_context_t *context);
float cpu_get_temperature(void);
float cpu_get_clock_speed(void);
int cpu_get_active_processes(void);
void cpu_free_info(cpu_info_t *info);
cpu_error_t cpu_get_last_error_code(void);
const char *cpu_get_last_error(void);

#endif
