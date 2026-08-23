#define _POSIX_C_SOURCE 200809L

#include "cpu.h"

#include <stdio.h>

int cpu_usage_context_init(cpu_usage_context_t *context)
{
        if (context == NULL)
        {
                return -1;
        }

        context->previous_total = 0;
        context->previous_idle = 0;
        context->has_previous = 0;
        return 0;
}

void cpu_usage_context_cleanup(cpu_usage_context_t *context)
{
        if (context != NULL)
        {
                context->previous_total = 0;
                context->previous_idle = 0;
                context->has_previous = 0;
        }
}

float cpu_get_usage_context(cpu_usage_context_t *context)
{
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
        float usage;

        if (context == NULL)
        {
                return -1.0f;
        }

        file = fopen("/proc/stat", "r");
        if (file == NULL || fscanf(file, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
                                   &user, &nice, &system, &idle, &iowait, &irq,
                                   &softirq, &steal) != 8)
        {
                if (file != NULL)
                {
                        fclose(file);
                }
                return -1.0f;
        }
        fclose(file);

        total = user + nice + system + idle + iowait + irq + softirq + steal;
        idle_total = idle + iowait;
        if (!context->has_previous)
        {
                context->previous_total = total;
                context->previous_idle = idle_total;
                context->has_previous = 1;
                return 0.0f;
        }

        if (total < context->previous_total || idle_total < context->previous_idle)
        {
                context->previous_total = total;
                context->previous_idle = idle_total;
                return 0.0f;
        }

        total_delta = total - context->previous_total;
        idle_delta = idle_total - context->previous_idle;
        context->previous_total = total;
        context->previous_idle = idle_total;

        if (total_delta == 0 || idle_delta > total_delta)
        {
                return 0.0f;
        }

        usage = 100.0f * (float)(total_delta - idle_delta) / (float)total_delta;
        return usage > 100.0f ? 100.0f : usage;
}
