#define _POSIX_C_SOURCE 200809L

#include "cpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
        if (info->cores <= 0 || info->threads <= 0)
        {
                free(info);
                return NULL;
        }

        return info;
}
