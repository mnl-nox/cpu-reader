# CPU Reader

![CPU Reader](https://img.shields.io/badge/CPU%20Reader-Linux%20Monitor-2563eb?style=for-the-badge&logo=linux&logoColor=white)
![Beta](https://img.shields.io/badge/status-BETA-f59e0b?style=for-the-badge&logo=opensourceinitiative&logoColor=white)
![Linux](https://img.shields.io/badge/platform-Linux-FCC624?style=flat-square&logo=linux&logoColor=black)
![C99](https://img.shields.io/badge/language-C99-A8B9CC?style=flat-square&logo=c&logoColor=white)
![MIT License](https://img.shields.io/badge/license-MIT-22c55e?style=flat-square)

![Build](https://img.shields.io/github/actions/workflow/status/mnl-nox/cpu-reader/ci.yml?branch=main&label=build&logo=githubactions&logoColor=white)
![Docker](https://img.shields.io/badge/tests-Docker%20Compose-2496ED?style=flat-square&logo=docker&logoColor=white)
![Make](https://img.shields.io/badge/build-GNU%20Make-427819?style=flat-square&logo=gnu&logoColor=white)
![Sanitizers](https://img.shields.io/badge/quality-ASan%20%2B%20UBSan-8b5cf6?style=flat-square)
![Cppcheck](https://img.shields.io/badge/static%20analysis-Cppcheck-0f766e?style=flat-square)
![Architecture](https://img.shields.io/badge/architectures-x86__64%20%7C%20arm64-64748b?style=flat-square)

Biblioteca C99 para consultar informações da CPU em Linux, incluindo uso agregado,
processos ativos, temperatura e velocidade de clock. O repositório também inclui
um monitor de terminal baseado em ncurses como exemplo de consumo da API.

> Biblioteca Beta pronta para uso em Linux, com testes funcionais, validação
> portátil, sanitizers, hardening, análise estática e execução reproduzível com
> Docker Compose.

## Visão geral

- `cpu_get_info()` lê dados estáticos e complementares da CPU.
- `cpu_get_usage()` calcula uso agregado com base em duas leituras de `/proc/stat`.
- `cpu_get_usage_context()` permite múltiplos contextos independentes.
- `cpu_get_temperature()` e `cpu_get_clock_speed()` consultam `sysfs`, com fallback
  para arquivos do Linux quando aplicável.
- `cpu_get_active_processes()` lê a quantidade de processos em execução.
- `cpu_get_last_error_code()` e `cpu_get_last_error()` expõem a última falha da thread chamadora (compatibilidade legada). Para capturar um diagnóstico estável por operação, use as variantes `*_ex(..., cpu_error_info_t *)`; o snapshot pertence ao chamador e não muda em chamadas posteriores.

## Estrutura do repositório

```text
include/cpu.h                  API pública
src/cpu.c                      fachada da API e estado por thread
src/cpu_info.c                 parsing de /proc/cpuinfo e topologia
src/cpu_telemetry.c             temperatura, clock e loadavg via proc/sysfs
src/cpu_usage.c                cálculo de uso agregado via /proc/stat
src/cpu_internal.h             contratos internos da implementação
examples/monitor.c             exemplo de monitor em ncurses
tests/test_cpu.c               teste integrado da API pública
doc/                           documentação técnica consolidada
BUILD.md                       instruções de compilação
VALIDATION.md                  guia de validação e testes
Makefile                       tarefas de build, teste e limpeza
build/                         artefatos gerados
```

## Requisitos

- Linux com `/proc/cpuinfo`, `/proc/stat` e `/proc/loadavg`
- GCC ou Clang com suporte a C99
- GNU Make
- Desenvolvimento ncurses para compilar o monitor (opcional para a biblioteca)

## Compatibilidade Beta

A validação automatizada executa em Linux `x86_64` e `aarch64/arm64`. A implementação atual usa C em todas as arquiteturas; não há caminho de assembly ativo. O alvo `test-portable` mantém compatibilidade com scripts existentes e valida a compilação C. `armv7` de 32 bits também executa a suíte sob emulação QEMU na CI; isso não equivale a validar hardware ARMv7 nativo.

O núcleo (`libcpu.a`) não depende de ncurses. O monitor é um exemplo opcional.
Para validar apenas o núcleo:

```bash
make core
make test
make test-portable
```

## Compilação e execução

```bash
make       # biblioteca e monitor
make test  # compila e executa os testes
make test-sanitize # AddressSanitizer e UndefinedBehaviorSanitizer
make test-security # flags de hardening do compilador e linker
make benchmark # microbenchmark local, sem limiar de aprovação
make telemetry # gera telemetria local em build/telemetry.json
make clean # remove build/ e libcpu.a
```

Executar o monitor:

```bash
./build/cpu-monitor
```

Pressione `q` para sair.

## API pública

```c
int cpu_init(void);
void cpu_cleanup(void);
cpu_info_t *cpu_get_info(void);
float cpu_get_usage(void);
int cpu_usage_context_init(cpu_usage_context_t *context);
void cpu_usage_context_cleanup(cpu_usage_context_t *context);
float cpu_get_usage_context(cpu_usage_context_t *context);
float cpu_get_temperature(void);
float cpu_get_clock_speed(void);
int cpu_get_active_processes(void);
void cpu_free_info(cpu_info_t *info);
cpu_error_t cpu_get_last_error_code(void);
const char *cpu_get_last_error(void);
void cpu_error_info_clear(cpu_error_info_t *error);
cpu_info_t *cpu_get_info_ex(cpu_error_info_t *error);
float cpu_get_usage_ex(cpu_error_info_t *error);
float cpu_get_usage_context_ex(cpu_usage_context_t *context, cpu_error_info_t *error);
```

### Comportamento principal

- `cpu_get_info()` retorna `NULL` em falha e deve ser liberada com `cpu_free_info()`.
- `cpu_get_usage()` retorna um valor entre `0` e `100`; na primeira leitura, o
  contexto padrão retorna `0.0f`.
- `cpu_get_temperature()` e `cpu_get_clock_speed()` retornam `-1.0f` quando a
  leitura não está disponível.
- `cpu_get_active_processes()` retorna `-1` em falha.
- O contexto usado por `cpu_get_usage()` e o relatório de erro legado são locais à thread em GCC/Clang; threads distintas não compartilham esse estado. Contextos explícitos não devem ser acessados simultaneamente sem sincronização externa.
- A API de erro legado é transitória; veja ADR-0008. A família `*_ex` já permite snapshots de erro pertencentes ao chamador; novos endpoints fallíveis devem oferecer variante explícita.

## Overrides para testes e depuração

As variáveis abaixo permitem redirecionar leituras sem recompilar:

- `CPU_READER_CPUINFO_PATH`
- `CPU_READER_PROC_STAT_PATH`
- `CPU_READER_CPU_TEMP_PATH`
- `CPU_READER_THERMAL_PATH` (raiz alternativa para `thermal_zone*/type` e `temp`)
- `CPU_READER_CPU_FREQ_PATH`
- `CPU_READER_LOADAVG_PATH`

## Documentação

- [BUILD.md](BUILD.md) - instruções de compilação
- [VALIDATION.md](VALIDATION.md) - validação e testes
- [doc/README.md](doc/README.md) - índice da documentação técnica
- [doc/arquitetura.md](doc/arquitetura.md) - visão arquitetural
- [doc/requerimentos.md](doc/requerimentos.md) - requisitos funcionais
- [doc/requisitos-nao-funcionais.md](doc/requisitos-nao-funcionais.md) - requisitos não funcionais
- [doc/software-requirements-specification.md](doc/software-requirements-specification.md) - SRS
- [doc/architecture-decision-records.md](doc/architecture-decision-records.md) - ADRs
- [doc/uml-diagrams.md](doc/uml-diagrams.md) - diagramas UML
- [SECURITY.md](SECURITY.md) - controles de segurança e política de relato

## Testes com Docker

Com Docker e Docker Compose instalados, execute a matriz completa:

```bash
docker compose build
docker compose run --rm gcc
docker compose run --rm monitor
docker compose run --rm portable
docker compose run --rm sanitizers
docker compose run --rm security
docker compose run --rm static-analysis
docker compose run --rm telemetry
```

Os serviços não acessam rede durante a execução dos testes. O serviço
`telemetry` gera apenas um resumo local em JSON; nenhuma métrica é enviada
para fora do ambiente.

## Status

`v1.0.0-beta.1`: biblioteca Beta pronta para uso em Linux, com suporte ao
fallback C portátil e testes automatizados da API pública.

## Semântica das métricas

- `logical_processors` conta entradas `processor` em `/proc/cpuinfo`; não é uma
  contagem de núcleos físicos.
- `physical_cores` é calculado a partir dos pares `physical id`/`core id` de
  `/proc/cpuinfo`. O valor zero indica que a topologia física não foi
  disponibilizada pelo sistema.
- `current_frequency_mhz` representa o valor atual reportado por `cpu MHz` ou
  pelo sysfs. Ele não é uma frequência base garantida; `-1.0f` indica que a
  plataforma não disponibilizou uma leitura válida.
- `model` usa os campos disponíveis na arquitetura (`model name`, `Processor`
  ou `Hardware`); se nenhum existir, retorna `Unknown CPU`. `flags` pode ficar
  vazio quando o kernel não expõe `flags` ou `Features`.
- A contagem de núcleos físicos pode ser zero quando `physical id`/`core id`
  não são fornecidos. Isso indica dado desconhecido, não uma CPU sem núcleos.
- Os campos legados `cores`, `threads` e `frequency_mhz` permanecem na
  estrutura por compatibilidade, mas novos consumidores devem usar os campos
  semânticos acima. Os parsers rejeitam valores numéricos incompletos, fora de
  faixa ou com caracteres residuais; campos adicionais numéricos válidos de
  `/proc/stat` são aceitos para compatibilidade entre versões do kernel.

## Benchmark

Execute `make benchmark` para medir o custo local de `cpu_get_info()` e `cpu_get_usage_context()`. O resultado é informativo, depende do host e não representa uma garantia de desempenho; não há limiar de CI.

## Licença

MIT. Consulte [LICENSE](LICENSE).
