#include "cpu.h"

#include <ncurses.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
        if (cpu_init() != 0)
        {
                return 1;
        }

        if (initscr() == NULL)
        {
                cpu_cleanup();
                return 1;
        }
        cbreak();
        noecho();
        curs_set(0);
        timeout(1000);

        while (getch() != 'q')
        {
                cpu_info_t *info = cpu_get_info();
                erase();

                if (info == NULL)
                {
                        mvprintw(0, 0, "Falha: %s", cpu_get_last_error());
                }
                else
                {
                        float usage = cpu_get_usage();
                        char usage_error[256] = "";
                        float clock_speed = cpu_get_clock_speed();
                        float temperature = cpu_get_temperature();

                        /*
                         * Each metric updates the shared error slot. Copy the
                         * usage error before collecting the other metrics.
                         */
                        if (usage < 0.0f)
                        {
                                snprintf(usage_error, sizeof(usage_error),
                                         "%s", cpu_get_last_error());
                        }

                        mvprintw(0, 0, "CPU Reader MVP");
                        mvprintw(2, 0, "Modelo:    %s", info->model[0] ? info->model : "desconhecido");
                        if (info->physical_cores > 0)
                        {
                                mvprintw(3, 0, "Nucleos fisicos: %d", info->physical_cores);
                        }
                        else
                        {
                                mvprintw(3, 0, "Nucleos fisicos: indisponivel");
                        }
                        mvprintw(4, 0, "Processadores logicos: %d", info->logical_processors);
                        if (info->active_processes >= 0)
                        {
                                mvprintw(5, 0, "Processos: %d",
                                         info->active_processes);
                        }
                        else
                        {
                                mvprintw(5, 0, "Processos: indisponivel");
                        }
                        if (info->current_frequency_mhz > 0.0f)
                        {
                                mvprintw(6, 0, "Frequencia atual: %.2f MHz",
                                         info->current_frequency_mhz);
                        }
                        else
                        {
                                mvprintw(6, 0, "Frequencia atual: indisponivel");
                        }
                        if (clock_speed < 0.0f)
                        {
                                mvprintw(7, 0, "Clock atual: indisponivel");
                        }
                        else
                        {
                                mvprintw(7, 0, "Clock atual: %.2f MHz", clock_speed);
                        }
                        if (temperature < 0.0f)
                        {
                                mvprintw(8, 0, "Temperatura: indisponivel");
                        }
                        else
                        {
                                mvprintw(8, 0, "Temperatura: %.2f C", temperature);
                        }
                        if (usage < 0.0f)
                        {
                                mvprintw(9, 0, "Uso:       indisponivel");
                                mvprintw(10, 0, "Falha: %s", usage_error);
                        }
                        else
                        {
                                mvprintw(9, 0, "Uso:       %.2f%%", usage);
                        }
                        mvprintw(12, 0, "Pressione q para sair");
                        cpu_free_info(info);
                }

                refresh();
        }

        endwin();
        cpu_cleanup();
        return 0;
}
