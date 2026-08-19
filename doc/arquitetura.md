# Arquitetura do CPU Reader

## Visão geral

O projeto é uma biblioteca C pequena, compilada como `libcpu.a`, e uma aplicação de exemplo separada. A biblioteca depende apenas da libc e do pseudo-sistema de arquivos `/proc` do Linux. O monitor depende adicionalmente de ncurses.

## Componentes

### API pública: `include/cpu.h`

Declara `cpu_info_t` e as seis funções públicas:

- `cpu_init()` e `cpu_cleanup()` reinicializam o estado usado pelo cálculo de uso.
- `cpu_get_info()` retorna informações lidas de `/proc/cpuinfo`.
- `cpu_get_usage()` calcula o uso agregado a partir de `/proc/stat`.
- `cpu_get_temperature()` retorna `-1.0f`; leitura de sensores ainda não foi implementada.
- `cpu_free_info()` libera a estrutura retornada por `cpu_get_info()`.

### Implementação: `src/cpu.c`

`cpu_get_info()` abre `/proc/cpuinfo`, aloca uma estrutura com `calloc()` e processa as chaves `processor`, `model name`, `cpu MHz` e `flags`. O campo `cores` conta entradas `processor`; na prática, representa processadores lógicos. `threads` vem de `_SC_NPROCESSORS_ONLN`.

`cpu_get_usage()` lê os oito primeiros contadores da linha `cpu` em `/proc/stat`. A diferença entre a leitura atual e a anterior produz a porcentagem de tempo não ocioso. A primeira chamada usa os contadores desde a inicialização do sistema como referência anterior.

### Aplicação: `examples/monitor.c`

Inicializa ncurses, atualiza as informações a cada segundo e encerra quando recebe `q`. A aplicação libera cada `cpu_info_t` depois de exibi-la.

## Estado e limitações

Os contadores anteriores de `/proc/stat` são variáveis estáticas globais. Portanto, a API de uso não oferece isolamento entre instâncias nem garantia de segurança para chamadas concorrentes. A biblioteca não implementa cache de informações, logging, leitura de temperatura ou métricas por núcleo.

## Fluxo de dados

```text
Aplicação
    |
    v
include/cpu.h -> src/cpu.c
                    |-- /proc/cpuinfo -> cpu_info_t
                    `-- /proc/stat    -> uso agregado (%)
```
