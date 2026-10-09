# Requerimentos do CPU Reader

Este documento descreve o escopo implementado atualmente. Recursos futuros estão marcados como planejados e não devem ser tratados como disponíveis.

## Requerimentos funcionais implementados

### RF-001: Informações básicas da CPU

`cpu_get_info()` deve ler `/proc/cpuinfo` e preencher:

- quantidade de entradas `processor` em `logical_processors`;
- núcleos físicos distintos em `physical_cores`, contados por pares únicos
  `physical id`/`core id` (quando ausentes ou incompletos, o valor deve ficar
  em `0`, significando topologia física desconhecida);
- primeiro `model name` (ou campos equivalentes da arquitetura) em `model`;
- primeiro `cpu MHz` em `current_frequency_mhz`;
- primeiro `flags` em `flags`.

Os campos `cores`, `threads` e `frequency_mhz` permanecem por compatibilidade
pré-1.0, mas não devem ser usados como nomenclatura semântica principal.

Também deve tentar complementar a estrutura com:

- processos ativos em `active_processes`, lidos de `/proc/loadavg`;
- temperatura atual em `temperature_c`, lida de `sysfs` quando houver sensor compatível.

Se o arquivo não puder ser aberto ou a memória não puder ser alocada, retorna `NULL`.

### RF-002: Uso agregado da CPU

`cpu_get_usage()` deve ler a linha `cpu` de `/proc/stat` e retornar uma porcentagem entre `0` e `100` usando o delta entre chamadas. Retorna `-1.0f` quando a leitura ou o parsing falha.

### RF-003: Gerenciamento de memória

Cada estrutura retornada por `cpu_get_info()` deve ser liberada com `cpu_free_info()`. A implementação fecha os arquivos de `/proc` em todas as saídas dessa leitura.

### RF-004: Inicialização e limpeza

`cpu_init()` e `cpu_cleanup()` devem zerar os contadores internos do contexto padrão de uso. `cpu_usage_context_t` permite manter contadores independentes por instância; a primeira leitura de cada contexto retorna `0.0f` e estabelece a referência.

### RF-005: Fontes de dados injetáveis para teste

Quando `CPU_READER_CPUINFO_PATH` estiver definida, `cpu_get_info()` deve ler o arquivo apontado por ela no lugar de `/proc/cpuinfo`.

Quando `CPU_READER_PROC_STAT_PATH` estiver definida, `cpu_get_usage()` e `cpu_get_usage_context()` devem ler o arquivo apontado por ela no lugar de `/proc/stat`.

### RF-006: Relatório de falhas

Após uma operação que falhar, a aplicação deve poder consultar `cpu_get_last_error_code()` e `cpu_get_last_error()` para obter, respectivamente, a categoria e uma mensagem descritiva. Uma chamada bem-sucedida limpa o relatório. O relatório é global e não possui garantia de segurança para múltiplas threads.

### RF-007: Temperatura da CPU

`cpu_get_temperature()` deve tentar ler a temperatura atual em `sysfs`. Quando não houver sensor compatível ou quando a leitura falhar, retorna `-1.0f`.

### RF-008: Velocidade de clock

`cpu_get_clock_speed()` deve tentar ler o clock atual por `sysfs`. Quando esse caminho não estiver disponível, deve usar como fallback o primeiro `cpu MHz` de `/proc/cpuinfo`. Em falha, retorna `-1.0f`.

### RF-009: Processos ativos

`cpu_get_active_processes()` deve retornar a contagem de processos em execução a partir de `/proc/loadavg`. Em falha, retorna `-1`.

### RF-010: Soma otimizada dos contadores de uso

Em `x86_64` compilado com GCC ou Clang, a soma dos oito contadores da linha `cpu` de `/proc/stat` deve usar assembly inline. Em outras arquiteturas ou compiladores, o mesmo cálculo deve usar a implementação C equivalente.

## Recursos planejados

- métricas individuais por núcleo;
- sincronização automática para chamadas concorrentes no mesmo contexto;
- instalação da biblioteca e documentação de API mais extensa.

## Requerimentos não funcionais

- Código compatível com C99.
- Execução em Linux com `/proc/cpuinfo` e `/proc/stat`.
- Núcleo sem dependência de ncurses.
- Compilação com `make` sem warnings com as flags padrão do projeto: `-std=c99 -Wall -Wextra -Wpedantic`.
