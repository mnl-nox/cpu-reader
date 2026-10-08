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
        cpu_error_t code;
        char message[256];
} cpu_error_info_t;

typedef struct
{
        /* Distinct topology concepts; do not use one as an alias for another. */
        int logical_processors;
        int physical_cores;
        /* "cpu MHz" and scaling_cur_freq are current-clock observations. */
        float current_frequency_mhz;
        /* Deprecated compatibility aliases. */
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

/* Clears an explicit, caller-owned error snapshot. NULL is allowed. */
void cpu_error_info_clear(cpu_error_info_t *error);
/* Explicit-error variants snapshot the failure for this operation. The caller
 * owns the error object; it remains unchanged by later library calls. */
int cpu_init_ex(cpu_error_info_t *error);
/* Initializes the default usage sample context. Returns 0 on success. */
int cpu_init(void);
/* Resets the default usage sample context. */
void cpu_cleanup(void);
/* Returns a heap-allocated snapshot; release it with cpu_free_info(). */
cpu_info_t *cpu_get_info(void);
cpu_info_t *cpu_get_info_ex(cpu_error_info_t *error);
/* Returns usage since the previous sample, or -1.0f on failure. */
float cpu_get_usage(void);
float cpu_get_usage_ex(cpu_error_info_t *error);
/* Initializes an independent usage sample context. */
int cpu_usage_context_init(cpu_usage_context_t *context);
int cpu_usage_context_init_ex(cpu_usage_context_t *context,
                              cpu_error_info_t *error);
/* Clears an independent usage sample context. */
void cpu_usage_context_cleanup(cpu_usage_context_t *context);
/* Returns usage for an independent context, or -1.0f on failure. */
float cpu_get_usage_context(cpu_usage_context_t *context);
float cpu_get_usage_context_ex(cpu_usage_context_t *context,
                               cpu_error_info_t *error);
/* Returns a temperature in Celsius, or -1.0f when unavailable/invalid. */
float cpu_get_temperature(void);
float cpu_get_temperature_ex(cpu_error_info_t *error);
/* Returns the current clock in MHz, never a guaranteed base frequency. */
float cpu_get_clock_speed(void);
float cpu_get_clock_speed_ex(cpu_error_info_t *error);
/* Returns the number of runnable processes reported by loadavg. */
int cpu_get_active_processes(void);
int cpu_get_active_processes_ex(cpu_error_info_t *error);
/* Frees a snapshot returned by cpu_get_info(). NULL is allowed. */
void cpu_free_info(cpu_info_t *info);
/* Returns the error from the most recent public operation. */
cpu_error_t cpu_get_last_error_code(void);
const char *cpu_get_last_error(void);

#endif
