# Arquitetura do CPU Reader

## Visão geral

O projeto é uma biblioteca C pequena, compilada como `libcpu.a`, e uma aplicação de exemplo separada. A biblioteca depende apenas da libc e do pseudo-sistema de arquivos `/proc` do Linux. O monitor depende adicionalmente de ncurses.

## Componentes

### API pública: `include/cpu.h`

Declara `cpu_info_t`, `cpu_usage_context_t`, `cpu_error_t` e as funções públicas da biblioteca:

- `cpu_init()` e `cpu_cleanup()` reinicializam o contexto padrão usado pelo cálculo de uso.
- `cpu_get_info()` retorna informações lidas de `/proc/cpuinfo`.
- `cpu_get_usage()` calcula o uso agregado a partir de `/proc/stat`.
- `cpu_usage_context_init()`, `cpu_usage_context_cleanup()` e `cpu_get_usage_context()` permitem estado independente por instância.
- `cpu_get_temperature()` lê a temperatura por `sysfs`, quando há sensor compatível, e retorna `-1.0f` quando ela não está disponível.
- `cpu_get_clock_speed()` consulta o clock atual por `sysfs`, com fallback para `/proc/cpuinfo`.
- `cpu_get_active_processes()` lê o número de processos em execução em `/proc/loadavg`.
- `cpu_free_info()` libera a estrutura retornada por `cpu_get_info()`.
- `cpu_get_last_error_code()` e `cpu_get_last_error()` expõem o último relatório de falha.

### Implementação do núcleo

`src/cpu.c` concentra o ciclo de vida, o contexto padrão e o relatório global de falhas.

`src/cpu_info.c` abre `/proc/cpuinfo`, ou o caminho definido em `CPU_READER_CPUINFO_PATH`, aloca a estrutura e processa as chaves `processor`, `model name`, `cpu MHz` e `flags`. O mesmo módulo lê temperatura, clock e processos ativos. Os caminhos podem ser substituídos, respectivamente, por `CPU_READER_CPU_TEMP_PATH`, `CPU_READER_CPU_FREQ_PATH` e `CPU_READER_LOADAVG_PATH`.

`src/cpu_usage.c` lê `/proc/stat`, ou o caminho definido em `CPU_READER_PROC_STAT_PATH`, e calcula o uso agregado por contexto. A soma dos oito contadores usa assembly inline em `x86_64` com GCC ou Clang; nas demais combinações, usa o fallback C equivalente.

`cpu_get_info()` retorna `NULL` quando não consegue abrir o arquivo, alocar memória ou obter contagens válidas. O campo `cores` conta entradas `processor`; na prática, representa processadores lógicos. `threads` vem de `_SC_NPROCESSORS_ONLN`. Quando disponíveis, `active_processes` e `temperature_c` também são preenchidos.

`cpu_get_usage_context()` lê os oito primeiros contadores da linha `cpu` em `/proc/stat`. A diferença entre a leitura atual e a anterior produz a porcentagem de tempo não ocioso. A primeira chamada estabelece a referência e retorna `0.0f`. `cpu_get_usage()` usa um contexto padrão por compatibilidade.

### Aplicação: `examples/monitor.c`

Inicializa ncurses, atualiza as informações a cada segundo e encerra quando recebe `q`. A aplicação libera cada `cpu_info_t` depois de exibi-la.

## Estado e limitações

O contexto padrão usado por `cpu_get_usage()` e o relatório retornado por `cpu_get_last_error()` são globais e não devem ser compartilhados por chamadas concorrentes. Para obter isolamento no cálculo, cada consumidor deve usar seu próprio `cpu_usage_context_t`; o relatório de falhas ainda não tem alternativa por contexto. A biblioteca não implementa sincronização automática, cache de informações, logging ou métricas por núcleo.

As variáveis de ambiente de override existem para testes e depuração. Em uso normal, a biblioteca continua dependente das interfaces Linux em `/proc`.

## Fluxo de dados

```text
Aplicação
    |
    v
include/cpu.h -> src/cpu.c
                    |-- /proc/cpuinfo -> cpu_info_t
                    |-- /proc/stat    -> uso agregado (%)
                    |-- /proc/loadavg -> processos ativos
                    `-- sysfs          -> temperatura e clock
```
