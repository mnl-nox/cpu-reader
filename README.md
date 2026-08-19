# CPU Reader

Biblioteca C99 para consultar informações básicas do processador e calcular o uso agregado da CPU em sistemas Linux. O projeto também fornece um monitor de terminal baseado em ncurses.

## Estado atual

- `cpu_get_info()` lê modelo, frequência, flags, processadores lógicos e threads online.
- `cpu_get_usage()` calcula o uso agregado a partir de duas leituras de `/proc/stat`.
- `cpu_get_temperature()` ainda retorna `-1.0f` e não lê sensores.
- A biblioteca não mantém arquivos abertos; os dados são lidos a cada chamada.
- O estado usado pelo cálculo de uso é global e não é thread-safe.

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
<<<<<<< Updated upstream
make install PREFIX=/usr/local
---
```

```
cpu-reader/
├── Makefile
├── README.md
├── LICENSE
├── doc/
│   ├── requerimentos.md
│   ├── decisao-design.md
│   ├── prioridades-sdlc.md
│   ├── arquitetura.md
│   ├── casosdeuso.md
│   ├── criterios.md
│   └── diagramas.md
├── src/
│   ├── cpu.c
│   └── cpu.h
├── examples/
│   └── main.c
└── include/
    └── cpu.h
||||||| Stash base
make install PREFIX=/usr/local


```
cpu-reader/
├── Makefile
├── README.md
├── LICENSE
├── doc/
│   ├── requerimentos.md
│   ├── decisao-design.md
│   ├── prioridades-sdlc.md
│   ├── arquitetura.md
│   ├── casosdeuso.md
│   ├── criterios.md
│   └── diagramas.md
├── src/
│   ├── cpu.c
│   └── cpu.h
├── examples/
│   └── main.c
└── include/
    └── cpu.h
=======
./build/cpu-monitor
>>>>>>> Stashed changes
```
---

Pressione `q` para sair. A biblioteca é gerada como `libcpu.a` e o monitor como `build/cpu-monitor`.

## Estrutura

```text
include/cpu.h       API pública
src/cpu.c           implementação da biblioteca
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
float cpu_get_temperature(void);
void cpu_free_info(cpu_info_t *info);
```

`cpu_get_info()` retorna uma estrutura alocada dinamicamente. A aplicação deve liberar o resultado com `cpu_free_info()`. Em caso de falha, a função retorna `NULL`. `cpu_get_usage()` retorna um valor entre `0` e `100` em condições normais e `-1.0f` se não conseguir ler `/proc/stat`.

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
