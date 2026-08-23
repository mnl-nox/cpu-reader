#include "cpu.h"

#include <stdlib.h>

static cpu_usage_context_t default_usage_context;

int cpu_init(void)
{
        return cpu_usage_context_init(&default_usage_context);
}

void cpu_cleanup(void)
{
        cpu_usage_context_cleanup(&default_usage_context);
}

float cpu_get_usage(void)
{
        return cpu_get_usage_context(&default_usage_context);
}

float cpu_get_temperature(void)
{
        return -1.0f;
}

void cpu_free_info(cpu_info_t *info)
{
        free(info);
}
