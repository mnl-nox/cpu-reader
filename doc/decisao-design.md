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

## D-007: Assembly limitado ao trecho aritmético

O acesso a `/proc` e o parsing permanecem em C, pois são a maior parte do trabalho e dependem de E/S. A soma fixa dos oito contadores de uso usa assembly inline somente em `x86_64` com GCC ou Clang. O código preserva um fallback C idêntico para manter portabilidade e facilitar validação.

## D-008: Relatório global de falhas

A API mantém os valores de retorno existentes para compatibilidade e acrescenta `cpu_get_last_error_code()` e `cpu_get_last_error()` para diagnóstico. O estado é global e sobrescrito por cada chamada, portanto a aplicação deve consultá-lo imediatamente e não usá-lo de forma concorrente.

## D-009: Métricas dependentes do Linux

Temperatura, clock atual e processos ativos são obtidos nas interfaces Linux `sysfs`, `/proc/cpuinfo` e `/proc/loadavg`. A temperatura pode não estar disponível quando a máquina não expõe um sensor compatível; a API representa esse caso com `-1.0f` e `CPU_ERROR_UNSUPPORTED`.

Uso por núcleo, logging, instalação, cache de dados e thread-safety continuam fora da implementação atual.

## D-010: Versionamento por commits convencionais

O workflow de release interpreta os prefixos dos commits enviados para `main`. Commits `feat` criam uma versão minor, enquanto `refactor`, `fix` e `bugfix` criam uma versão patch. Assim, a automação gera tags previsíveis sem exigir edição manual de arquivos de versão.
