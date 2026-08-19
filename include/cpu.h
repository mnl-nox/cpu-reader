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

int cpu_init(void);
void cpu_cleanup(void);
cpu_info_t *cpu_get_info(void);
float cpu_get_usage(void);
float cpu_get_temperature(void);
void cpu_free_info(cpu_info_t *info);

#endif
