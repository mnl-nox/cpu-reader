#include "cpu.h"

#include <ncurses.h>
#include <unistd.h>

int main(void)
{
        cpu_info_t *info;
        float usage;

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
                usage = cpu_get_usage();
                erase();

                if (info == NULL)
                {
                        mvprintw(0, 0, "Nao foi possivel ler /proc/cpuinfo");
                }
                else
                {
                        mvprintw(0, 0, "CPU Reader MVP");
                        mvprintw(2, 0, "Modelo:    %s", info->model[0] ? info->model : "desconhecido");
                        mvprintw(3, 0, "Nucleos:   %d", info->cores);
                        mvprintw(4, 0, "Threads:   %d", info->threads);
                        mvprintw(5, 0, "Frequencia: %.2f MHz", info->frequency_mhz);
                        mvprintw(6, 0, "Uso:       %.2f%%", usage);
                        mvprintw(8, 0, "Pressione q para sair");
                        cpu_free_info(info);
                }

                refresh();
        }

        endwin();
        cpu_cleanup();
        return 0;
}
