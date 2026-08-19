# Requerimentos do CPU Reader

Este documento descreve o escopo implementado atualmente. Recursos futuros estão marcados como planejados e não devem ser tratados como disponíveis.

## Requerimentos funcionais implementados

### RF-001: Informações básicas da CPU

`cpu_get_info()` deve ler `/proc/cpuinfo` e preencher:

- quantidade de entradas `processor` em `cores`;
- processadores lógicos online em `threads`;
- primeiro `model name` em `model`;
- primeiro `cpu MHz` em `frequency_mhz`;
- primeiro `flags` em `flags`.

Se o arquivo não puder ser aberto ou a memória não puder ser alocada, retorna `NULL`.

### RF-002: Uso agregado da CPU

`cpu_get_usage()` deve ler a linha `cpu` de `/proc/stat` e retornar uma porcentagem entre `0` e `100` usando o delta entre chamadas. Retorna `-1.0f` quando a leitura ou o parsing falha.

### RF-003: Gerenciamento de memória

Cada estrutura retornada por `cpu_get_info()` deve ser liberada com `cpu_free_info()`. A implementação fecha os arquivos de `/proc` em todas as saídas dessa leitura.

### RF-004: Inicialização e limpeza

`cpu_init()` e `cpu_cleanup()` devem zerar os contadores internos do cálculo de uso. Não há outros recursos persistentes para inicializar ou liberar.

## Recursos planejados

- leitura de temperatura por `sysfs` ou sensores compatíveis;
- métricas individuais por núcleo;
- contexto de estado por instância e thread-safety;
- mensagens de erro detalhadas;
- instalação da biblioteca e documentação de API mais extensa.

## Requerimentos não funcionais

- Código compatível com C99.
- Execução em Linux com `/proc/cpuinfo` e `/proc/stat`.
- Núcleo sem dependência de ncurses.
- Compilação com `make` sem warnings com as flags padrão do projeto: `-std=c99 -Wall -Wextra -Wpedantic`.
