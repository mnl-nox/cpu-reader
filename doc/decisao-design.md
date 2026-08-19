# Decisões de design do CPU Reader

## D-001: C99

O projeto usa C99 para manter uma API pequena, compilação simples e dependência mínima. O `Makefile` aplica `-std=c99 -Wall -Wextra -Wpedantic` por padrão.

## D-002: API baseada em estrutura

Informações estáticas são retornadas em `cpu_info_t`, declarado em `include/cpu.h`. A estrutura usa buffers de tamanho fixo para `model` e `flags`, evitando que o chamador precise gerenciar essas strings separadamente.

## D-003: Interface `/proc`

A implementação usa `/proc/cpuinfo` para informações descritivas e `/proc/stat` para contadores de CPU. Isso mantém o núcleo sem dependências externas e é adequado ao escopo Linux atual.

## D-004: Alocação explícita

`cpu_get_info()` aloca com `calloc()` e o chamador libera com `cpu_free_info()`. Em caso de falha de alocação ou abertura do arquivo, a função retorna `NULL`.

## D-005: Uso por deltas

`cpu_get_usage()` compara os contadores atuais com a leitura anterior. O estado é mantido em duas variáveis estáticas no módulo. Essa escolha simplifica a API, mas torna o cálculo compartilhado e não thread-safe.

## D-006: Monitor separado

O núcleo não depende de ncurses. `examples/monitor.c` é um consumidor da API e é vinculado com `-lncurses` pelo alvo `monitor` do `Makefile`.

## D-007: Recursos ainda não implementados

Temperatura, uso por núcleo, logging, instalação, cache de dados, thread-safety e códigos de erro detalhados foram mantidos fora da implementação atual. Esses itens devem ser tratados como evolução futura, não como comportamento garantido pela API.
