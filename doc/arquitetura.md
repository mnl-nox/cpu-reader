# Arquitetura do CPU Reader

## Visão geral

O projeto é uma biblioteca C pequena, compilada como `libcpu.a`, e uma aplicação de exemplo separada. A biblioteca depende apenas da libc e do pseudo-sistema de arquivos `/proc` do Linux. O monitor depende adicionalmente de ncurses.

## Componentes

### API pública: `include/cpu.h`

Declara `cpu_info_t` e as seis funções públicas:

- `cpu_init()` e `cpu_cleanup()` reinicializam o contexto padrão usado pelo cálculo de uso.
- `cpu_get_info()` retorna informações lidas de `/proc/cpuinfo`.
- `cpu_get_usage()` calcula o uso agregado a partir de `/proc/stat`.
- `cpu_usage_context_init()`, `cpu_usage_context_cleanup()` e `cpu_get_usage_context()` permitem estado independente por instância.
- `cpu_get_temperature()` retorna `-1.0f`; leitura de sensores ainda não foi implementada.
- `cpu_free_info()` libera a estrutura retornada por `cpu_get_info()`.

### Implementação do núcleo

`src/cpu.c` concentra o ciclo de vida, o contexto padrão e funções comuns da API.

`src/cpu_info.c` abre `/proc/cpuinfo`, aloca a estrutura e processa as chaves `processor`, `model name`, `cpu MHz` e `flags`.

`src/cpu_usage.c` lê `/proc/stat` e calcula o uso agregado por contexto.

`cpu_get_info()` retorna `NULL` quando não consegue abrir o arquivo, alocar memória ou obter contagens válidas. O campo `cores` conta entradas `processor`; na prática, representa processadores lógicos. `threads` vem de `_SC_NPROCESSORS_ONLN`.

`cpu_get_usage_context()` lê os oito primeiros contadores da linha `cpu` em `/proc/stat`. A diferença entre a leitura atual e a anterior produz a porcentagem de tempo não ocioso. A primeira chamada estabelece a referência e retorna `0.0f`. `cpu_get_usage()` usa um contexto padrão por compatibilidade.

### Aplicação: `examples/monitor.c`

Inicializa ncurses, atualiza as informações a cada segundo e encerra quando recebe `q`. A aplicação libera cada `cpu_info_t` depois de exibi-la.

## Estado e limitações

O contexto padrão usado por `cpu_get_usage()` é global e não deve ser compartilhado por chamadas concorrentes. Para obter isolamento, cada consumidor deve usar seu próprio `cpu_usage_context_t`. A biblioteca não implementa sincronização automática, cache de informações, logging, leitura de temperatura ou métricas por núcleo.

## Fluxo de dados

```text
Aplicação
    |
    v
include/cpu.h -> src/cpu.c
                    |-- /proc/cpuinfo -> cpu_info_t
                    `-- /proc/stat    -> uso agregado (%)
```
