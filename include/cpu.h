#ifndef CPU_READER_CPU_H
#define CPU_READER_CPU_H

typedef struct
{
        int cores;
        int threads;
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
void cpu_free_info(cpu_info_t *info);

#endif
