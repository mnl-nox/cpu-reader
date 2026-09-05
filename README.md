# CPU Reader

Biblioteca C99 para consultar informações da CPU em sistemas Linux, incluindo uso agregado, processos ativos, temperatura e velocidade de clock. O projeto também fornece um monitor de terminal baseado em ncurses.

## Estado atual

- `cpu_get_info()` lê modelo, frequência, flags, processadores lógicos, threads online e processos ativos.
- `cpu_get_usage()` calcula o uso agregado a partir de duas leituras de `/proc/stat`.
- `cpu_get_active_processes()` lê a quantidade de processos em execução a partir de `/proc/loadavg`.
- `cpu_get_clock_speed()` consulta o clock atual por `sysfs`, com fallback para `/proc/cpuinfo`.
- `cpu_get_temperature()` lê a temperatura por `sysfs` quando houver sensor compatível.
- Falhas expõem um código e uma mensagem por `cpu_get_last_error_code()` e `cpu_get_last_error()`.
- Em `x86_64` com GCC ou Clang, a soma dos contadores de uso usa assembly inline com fallback C nas demais plataformas.
- A biblioteca não mantém arquivos abertos; os dados são lidos a cada chamada.
- `cpu_usage_context_t` permite manter estado independente para o cálculo de uso.

## Requisitos

- Linux com `/proc/cpuinfo` e `/proc/stat` disponíveis
- GCC ou Clang com suporte a C99
- GNU Make
- Desenvolvimento ncurses para compilar o monitor (`libncurses-dev` em Debian/Ubuntu)

## Compilação e execução

```bash
make # biblioteca e monitor
make test # compila e executa os testes
make clean # remove build/ e libcpu.a
```

O monitor é executado com:

```bash
./build/cpu-monitor
```

Pressione `q` para sair. A biblioteca é gerada como `libcpu.a` e o monitor como `build/cpu-monitor`.

## Estrutura

```text
include/cpu.h API pública
src/cpu.c ciclo de vida e fachada da API
src/cpu_info.c leitura e parsing de /proc/cpuinfo
src/cpu_usage.c leitura de /proc/stat e cálculo de uso
examples/monitor.c aplicação ncurses
tests/test_cpu.c teste básico da API
Makefile build da biblioteca, monitor e testes
build/ objetos e executáveis gerados
```

## API

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

`cpu_get_info()` retorna uma estrutura alocada dinamicamente. A aplicação deve liberar o resultado com `cpu_free_info()`. Em caso de falha, a função retorna `NULL`. `cpu_get_usage()` retorna um valor entre `0` e `100` em condições normais e `-1.0f` se não conseguir ler `/proc/stat`. `cpu_usage_context_t` permite leituras de uso independentes.

Quando disponíveis, `cpu_get_info()` também preenche `active_processes` e `temperature_c`. A leitura de clock atual via `cpu_get_clock_speed()` tenta primeiro `/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq` e cai para o primeiro `cpu MHz` encontrado em `/proc/cpuinfo`.

Após uma falha, consulte `cpu_get_last_error_code()` e `cpu_get_last_error()` antes de executar outra chamada da biblioteca. O relatório é global, é limpo por chamadas bem-sucedidas e não é thread-safe. Os códigos possíveis são `CPU_ERROR_INVALID_ARGUMENT`, `CPU_ERROR_FILE_OPEN`, `CPU_ERROR_PARSE`, `CPU_ERROR_MEMORY` e `CPU_ERROR_UNSUPPORTED`.

Para testes e depuração, as leituras podem ser redirecionadas via `CPU_READER_CPUINFO_PATH`, `CPU_READER_PROC_STAT_PATH`, `CPU_READER_CPU_TEMP_PATH`, `CPU_READER_CPU_FREQ_PATH` e `CPU_READER_LOADAVG_PATH`. Quando essas variáveis não estão definidas, a biblioteca usa os caminhos padrão do Linux em `/proc` e `sysfs`.

## Versionamento

O projeto usa tags no formato `vMAJOR.MINOR.PATCH` e as cria automaticamente quando um commit é enviado para a branch `main`.

- `feat: ...` incrementa a versão minor, por exemplo, `v0.1.0` para `v0.2.0`.
- `refactor: ...`, `fix: ...` ou `bugfix: ...` incrementam a versão patch, por exemplo, `v0.2.0` para `v0.2.1`.
- Outros tipos, como `docs:`, `test:` e `chore:`, não criam tag.

Quando mais de um commit elegível chega no mesmo push, é criada uma única tag no commit mais recente. `feat` tem prioridade sobre os incrementos patch. Como o repositório ainda não possui tags, a primeira versão parte de `v0.0.0`.

## Documentação

### Iniciar aqui

- **[doc/README.md](doc/README.md)** - Índice navegável de toda a documentação
- **[BUILD.md](BUILD.md)** - Instruções completas de compilação
- **[VALIDATION.md](VALIDATION.md)** - Relatório de validação de requisitos

### Especificação

- **[doc/requisitos-funcionais.md](doc/requisitos-funcionais.md)** - RF-001 a RF-012 implementadas + RF-013 a RF-015 futuras
- **[doc/requisitos-nao-funcionais.md](doc/requisitos-nao-funcionais.md)** - RNF-001 a RNF-012 (compatibilidade, performance, segurança)
- **[doc/software-requirements-specification.md](doc/software-requirements-specification.md)** - SRS formal com matriz de rastreabilidade

### ️ Arquitetura e Design

- **[doc/design.md](doc/design.md)** - Design Document v2.0 com componentes, padrões e trade-offs
- **[doc/architecture-decision-records.md](doc/architecture-decision-records.md)** - ADR-0001 a ADR-0013 (decisões formalizadas)
- **[doc/uml-diagrams.md](doc/uml-diagrams.md)** - 14 Diagramas UML em Mermaid

### Planejamento

- **[doc/roadmap-evolucao-semana.md](doc/roadmap-evolucao-semana.md)** - Roadmap 8 semanas com tarefas diárias
- **[doc/prioridades-sdlc.md](doc/prioridades-sdlc.md)** - Prioridades P1-P4 com timeline

### Documentação Original

- [Arquitetura](doc/arquitetura.md) - Visão geral (v1.0)
- [Requerimentos](doc/requerimentos.md) - Requisitos iniciais (v1.0)
- [Casos de uso](doc/casosdeuso.md) - Cenários de uso
- [Critérios de aceitação](doc/criterios.md) - Acceptance criteria
- [Decisões de design](doc/decisao-design.md) - Design decisions (v1.0)
- [Diagramas](doc/diagramas.md) - Diagramas iniciais

### Consolidação

- **[doc/CONSOLIDACAO-VALIDACAO.md](doc/CONSOLIDACAO-VALIDACAO.md)** - Sumário completo da consolidação
- **[doc/SUMMARY.sh](doc/SUMMARY.sh)** - Script para gerar relatório visual

## Status do Projeto

### Semana 1 (2026-09-04) - Documentação Consolidada

- [x] 7 novos documentos criados
- [x] 24 requisitos documentados (12 RF + 12 RNF)
- [x] 13 ADRs formalizadas
- [x] 14 Diagramas UML
- [x] Roadmap 8 semanas definido
- [x] Documentação de build (BUILD.md)
- [x] Relatório de validação (VALIDATION.md)
- [x] Commit realizado com `docs: consolidate requirements...`

### Próximas Fases
// Semana 2 em Atraso 
**Semana 2 (2026-09-11)** - Validação e Testes
- [ ] Sanitizers (AddressSanitizer, UBSan)
- [ ] Multi-distribuição (RHEL, Alpine)
- [ ] Benchmarks básicos

**Semana 3 (2026-09-16)** - CI/CD Setup
- [ ] GitHub Actions workflow
- [ ] Lint estático (clang-tidy)
- [ ] Multi-plataforma CI

**Semana 4+ (2026-09-23)** - Features e Release
- [ ] RF-013: Métricas por núcleo
- [ ] v0.2.0 Release
- [ ] Suporte multi-plataforma

## Versão Atual

- **Versão:** v0.1.0 (tag automatizada)
- **Status:** Beta (Semana 1)
- **Requisitos:** 12/12 RF implementadas 
- **RNF Validadas:** 10/12 
- **Testes:** Todos passando 
- **Documentação:** Consolidada 

## Licença

MIT. Consulte [LICENSE](LICENSE).
