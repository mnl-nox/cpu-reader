#define _POSIX_C_SOURCE 200809L

#include "cpu_internal.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *proc_stat_path(void)
{
        const char *path = getenv("CPU_READER_PROC_STAT_PATH");

        if (path == NULL || path[0] == '\0')
        {
                return "/proc/stat";
        }

        return path;
}

static unsigned long long sum_cpu_counters(const unsigned long long counters[8])
{
#if defined(__x86_64__) && (defined(__GNUC__) || defined(__clang__))
        const unsigned long long *values = counters;
        unsigned long long total;

        __asm__ volatile(
                "xorq %[total], %[total]\n\t"
                "movq $8, %%rcx\n\t"
                "1:\n\t"
                "addq (%[values]), %[total]\n\t"
                "addq $8, %[values]\n\t"
                "decq %%rcx\n\t"
                "jnz 1b"
                : [total] "=&r"(total), [values] "+r"(values)
                :
                : "rcx", "cc", "memory");

        return total;
#else
        unsigned long long total = 0;
        size_t index;

        for (index = 0; index < 8; index++)
        {
                total += counters[index];
        }

        return total;
#endif
}

int cpu_usage_context_init(cpu_usage_context_t *context)
{
        if (context == NULL)
        {
                cpu_set_last_error(CPU_ERROR_INVALID_ARGUMENT,
                                   "Contexto de uso da CPU nao pode ser nulo");
                return -1;
        }

        context->previous_total = 0;
        context->previous_idle = 0;
        context->has_previous = 0;
        cpu_clear_last_error();
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
        unsigned long long counters[8];
        float usage;
        const char *path = proc_stat_path();

        if (context == NULL)
        {
                cpu_set_last_error(CPU_ERROR_INVALID_ARGUMENT,
                                   "Contexto de uso da CPU nao pode ser nulo");
                return -1.0f;
        }

        file = fopen(path, "r");
        if (file == NULL)
        {
                cpu_set_last_error(CPU_ERROR_FILE_OPEN,
                                   "Nao foi possivel abrir %s: %s", path,
                                   strerror(errno));
                return -1.0f;
        }

        if (fscanf(file, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
                   &user, &nice, &system, &idle, &iowait, &irq, &softirq,
                   &steal) != 8)
        {
                fclose(file);
                cpu_set_last_error(CPU_ERROR_PARSE,
                                   "Nao foi possivel interpretar a linha cpu em %s",
                                   path);
                return -1.0f;
        }

        fclose(file);

        counters[0] = user;
        counters[1] = nice;
        counters[2] = system;
        counters[3] = idle;
        counters[4] = iowait;
        counters[5] = irq;
        counters[6] = softirq;
        counters[7] = steal;
        total = sum_cpu_counters(counters);
        idle_total = idle + iowait;
        if (!context->has_previous)
        {
                context->previous_total = total;
                context->previous_idle = idle_total;
                context->has_previous = 1;
                cpu_clear_last_error();
                return 0.0f;
        }

        if (total < context->previous_total || idle_total < context->previous_idle)
        {
                context->previous_total = total;
                context->previous_idle = idle_total;
                cpu_clear_last_error();
                return 0.0f;
        }

        total_delta = total - context->previous_total;
        idle_delta = idle_total - context->previous_idle;
        context->previous_total = total;
        context->previous_idle = idle_total;

        if (total_delta == 0 || idle_delta > total_delta)
        {
                cpu_clear_last_error();
                return 0.0f;
        }

        usage = 100.0f * (float)(total_delta - idle_delta) / (float)total_delta;
        cpu_clear_last_error();
        return usage > 100.0f ? 100.0f : usage;
}
