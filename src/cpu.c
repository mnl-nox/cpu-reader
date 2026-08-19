#define _POSIX_C_SOURCE 200809L

#include "cpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static unsigned long long previous_total;
static unsigned long long previous_idle;

static void copy_value(char *destination, size_t size, const char *value)
{
        value += strspn(value, " \t");
        snprintf(destination, size, "%s", value);
        destination[strcspn(destination, "\n")] = '\0';
}

static void trim_key(char *key)
{
        size_t length = strlen(key);

        while (length > 0 && (key[length - 1] == ' ' || key[length - 1] == '\t'))
        {
                key[--length] = '\0';
        }
}

int cpu_init(void)
{
        previous_total = 0;
        previous_idle = 0;
        return 0;
}

void cpu_cleanup(void)
{
        previous_total = 0;
        previous_idle = 0;
}

cpu_info_t *cpu_get_info(void)
{
        FILE *file;
        cpu_info_t *info;
        char line[1024];

        file = fopen("/proc/cpuinfo", "r");
        if (file == NULL)
        {
                return NULL;
        }

        info = calloc(1, sizeof(*info));
        if (info == NULL)
        {
                fclose(file);
                return NULL;
        }

        info->threads = (int)sysconf(_SC_NPROCESSORS_ONLN);
        while (fgets(line, sizeof(line), file) != NULL)
        {
                char *separator = strchr(line, ':');

                if (separator == NULL)
                {
                        continue;
                }
                *separator = '\0';
                separator++;
                trim_key(line);

                if (strcmp(line, "processor") == 0)
                {
                        info->cores++;
                }
                else if (strcmp(line, "model name") == 0 && info->model[0] == '\0')
                {
                        copy_value(info->model, sizeof(info->model), separator);
                }
                else if (strcmp(line, "cpu MHz") == 0 && info->frequency_mhz == 0.0f)
                {
                        info->frequency_mhz = strtof(separator, NULL);
                }
                else if (strcmp(line, "flags") == 0 && info->flags[0] == '\0')
                {
                        copy_value(info->flags, sizeof(info->flags), separator);
                }
        }

        fclose(file);
        return info;
}

float cpu_get_usage(void)
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
        total_delta = total - previous_total;
        idle_delta = idle_total - previous_idle;
        previous_total = total;
        previous_idle = idle_total;

        if (total_delta == 0)
        {
                return 0.0f;
        }

        usage = 100.0f * (float)(total_delta - idle_delta) / (float)total_delta;
        return usage < 0.0f ? 0.0f : usage;
}

float cpu_get_temperature(void)
{
        return -1.0f;
}

void cpu_free_info(cpu_info_t *info)
{
        free(info);
}
