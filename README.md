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
make          # biblioteca e monitor
make test     # compila e executa os testes
make clean    # remove build/ e libcpu.a
```

O monitor é executado com:

```bash
./build/cpu-monitor
```

Pressione `q` para sair. A biblioteca é gerada como `libcpu.a` e o monitor como `build/cpu-monitor`.

## Estrutura

```text
include/cpu.h       API pública
src/cpu.c           ciclo de vida e fachada da API
src/cpu_info.c      leitura e parsing de /proc/cpuinfo
src/cpu_usage.c     leitura de /proc/stat e cálculo de uso
examples/monitor.c  aplicação ncurses
tests/test_cpu.c    teste básico da API
Makefile            build da biblioteca, monitor e testes
build/              objetos e executáveis gerados
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

- [Arquitetura](doc/arquitetura.md)
- [Requerimentos](doc/requerimentos.md)
- [Casos de uso](doc/casosdeuso.md)
- [Critérios de aceitação](doc/criterios.md)
- [Decisões de design](doc/decisao-design.md)
- [Prioridades](doc/prioridades-sdlc.md)
- [Diagramas](doc/diagramas.md)

## Licença

MIT. Consulte [LICENSE](LICENSE).
