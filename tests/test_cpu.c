#include "cpu.h"

#include <stdio.h>

int main(void)
{
        cpu_info_t *info = cpu_get_info();
        cpu_usage_context_t first_context;
        cpu_usage_context_t second_context;
        float usage;

        if (info == NULL || info->cores <= 0 || info->threads <= 0 ||
            info->model[0] == '\0' || info->frequency_mhz <= 0.0f ||
            info->flags[0] == '\0')
        {
                fprintf(stderr, "cpu_get_info returned incomplete data\n");
                return 1;
        }

        if (cpu_usage_context_init(&first_context) != 0 ||
            cpu_usage_context_init(&second_context) != 0 ||
            cpu_usage_context_init(NULL) != -1 ||
            cpu_get_usage_context(NULL) >= 0.0f ||
            cpu_get_usage_context(&first_context) != 0.0f ||
            cpu_get_usage_context(&second_context) != 0.0f ||
            cpu_init() != 0 || cpu_get_usage() != 0.0f)
        {
                fprintf(stderr, "cpu usage contexts failed\n");
                cpu_free_info(info);
                return 1;
        }

        usage = cpu_get_usage();
        if (usage < 0.0f || usage > 100.0f)
        {
                fprintf(stderr, "cpu_get_usage returned an invalid value\n");
                cpu_free_info(info);
                return 1;
        }

        cpu_free_info(info);
        return 0;
}
