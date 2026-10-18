# API de diagnóstico

Inclua cpu_diagnostics.h além de cpu.h. A API não altera o layout dos tipos públicos legados.

~~~c
#include <stdio.h>
#include "cpu_diagnostics.h"
int main(void) {
    cpu_diagnostic_report_t report;
    cpu_error_info_t error;
    if (cpu_get_diagnostic_report(&report, &error) != 0) {
        fprintf(stderr, "Falha ao montar relatório: %s\n", error.message);
        return 1;
    }
    if (report.info_available) {
        printf("CPU: %s (%d threads visíveis)\n",
               report.model, report.logical_processors);
    }
    printf("Temperatura: status=%d, %.1f C, fonte=%s\n",
           report.temperature_c.status, report.temperature_c.value,
           report.temperature_c.source);
    return 0;
}
~~~

cpu_get_diagnostic_report() devolve 0 quando consegue montar a fotografia, ainda que métricas estejam indisponíveis. Consulte cada status, error_code e message. CPU_DIAG_BASELINE em uso de CPU significa que uma única amostra não é suficiente para estimar percentual entre intervalos. Falhas de leitura/parsing, inclusive caminhos ausentes que gerem erro de I/O, permanecem em CPU_DIAG_ERROR; indisponibilidade de sensor sem erro de leitura fica em CPU_DIAG_UNAVAILABLE. O campo source contém o caminho sysfs/procfs efetivamente lido quando disponível; o campo temperature_c.sensor_type contém o tipo da zona térmica ou `explicit override`. Esses campos identificam a interface consultada, não constituem prova de origem física nem atestação.
