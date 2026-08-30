#include "cpu.h"

#include <ncurses.h>
#include <unistd.h>

int main(void)
{
        cpu_info_t *info;
        float usage;
        float clock_speed;
        float temperature;

        if (cpu_init() != 0)
        {
                return 1;
        }

        initscr();
        cbreak();
        noecho();
        curs_set(0);
        timeout(1000);

        while (getch() != 'q')
        {
                info = cpu_get_info();
                erase();

                if (info == NULL)
                {
                        mvprintw(0, 0, "Falha: %s", cpu_get_last_error());
                }
                else
                {
                        usage = cpu_get_usage();
                        clock_speed = cpu_get_clock_speed();
                        temperature = cpu_get_temperature();
                        mvprintw(0, 0, "CPU Reader MVP");
                        mvprintw(2, 0, "Modelo:    %s", info->model[0] ? info->model : "desconhecido");
                        mvprintw(3, 0, "Nucleos:   %d", info->cores);
                        mvprintw(4, 0, "Threads:   %d", info->threads);
                        mvprintw(5, 0, "Processos: %d",
                                 info->active_processes >= 0 ? info->active_processes : 0);
                        mvprintw(6, 0, "Frequencia base: %.2f MHz", info->frequency_mhz);
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
                                mvprintw(10, 0, "Falha: %s", cpu_get_last_error());
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
