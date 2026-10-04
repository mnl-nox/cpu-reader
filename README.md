# CPU Reader

Biblioteca C99 para consultar informações da CPU em Linux, incluindo uso agregado,
processos ativos, temperatura e velocidade de clock. O repositório também inclui
um monitor de terminal baseado em ncurses como exemplo de consumo da API.

## Visão geral

- `cpu_get_info()` lê dados estáticos e complementares da CPU.
- `cpu_get_usage()` calcula uso agregado com base em duas leituras de `/proc/stat`.
- `cpu_get_usage_context()` permite múltiplos contextos independentes.
- `cpu_get_temperature()` e `cpu_get_clock_speed()` consultam `sysfs`, com fallback
  para arquivos do Linux quando aplicável.
- `cpu_get_active_processes()` lê a quantidade de processos em execução.
- `cpu_get_last_error_code()` e `cpu_get_last_error()` expõem a última falha.

## Estrutura do repositório

```text
include/cpu.h                  API pública
src/cpu.c                      fachada da API e estado global
src/cpu_info.c                 leitura de /proc/cpuinfo e dados auxiliares
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

A versão Beta suporta Linux em `x86_64`, `aarch64/arm64`, `armv7` e outras
arquiteturas que forneçam as interfaces `/proc` e `sysfs`. O caminho otimizado
com assembly é usado apenas em `x86_64` com GCC/Clang; todas as demais
plataformas usam o fallback C portátil.

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
```

### Comportamento principal

- `cpu_get_info()` retorna `NULL` em falha e deve ser liberada com `cpu_free_info()`.
- `cpu_get_usage()` retorna um valor entre `0` e `100`; na primeira leitura, o
  contexto padrão retorna `0.0f`.
- `cpu_get_temperature()` e `cpu_get_clock_speed()` retornam `-1.0f` quando a
  leitura não está disponível.
- `cpu_get_active_processes()` retorna `-1` em falha.
- O relatório global de erro é limpo por chamadas bem-sucedidas.

## Overrides para testes e depuração

As variáveis abaixo permitem redirecionar leituras sem recompilar:

- `CPU_READER_CPUINFO_PATH`
- `CPU_READER_PROC_STAT_PATH`
- `CPU_READER_CPU_TEMP_PATH`
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

## Status

`v1.0.0-beta.1`: biblioteca Beta pronta para uso em Linux, com suporte ao
fallback C portátil e testes automatizados da API pública.

## Licença

MIT. Consulte [LICENSE](LICENSE).
