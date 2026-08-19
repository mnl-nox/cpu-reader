#include "cpu.h"

#include <stdio.h>

int main(void)
{
        cpu_info_t *info = cpu_get_info();
        float usage;

        if (info == NULL || info->cores <= 0 || info->threads <= 0)
        {
                fprintf(stderr, "cpu_get_info failed\n");
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
