# Build Guide

Este repositório usa `make` para compilar a biblioteca, o monitor e os testes.

## Pré-requisitos

- Compilador C99, como `gcc` ou `clang`
- GNU Make
- ncurses de desenvolvimento para compilar `examples/monitor.c` (opcional)

## Alvos disponíveis

```bash
make       # compila libcpu.a e o monitor
make core  # compila apenas libcpu.a
make monitor
make test  # compila e executa os testes
make test-portable # testa o fallback C sem assembly x86_64
make clean # remove build/ e libcpu.a
```

## Saídas geradas

- `libcpu.a` - biblioteca estática do núcleo
- `build/cpu-monitor` - monitor ncurses de exemplo
- `build/test_cpu` - binário de testes

## Flags padrão

O `Makefile` usa, por padrão:

```bash
-std=c99 -Wall -Wextra -Wpedantic -Iinclude
```

## Observações

- O núcleo da biblioteca não depende de ncurses.
- O monitor é opcional e existe apenas como exemplo de uso da API.
- O diretório `build/` é descartável e pode ser removido com `make clean`.
- `make test-portable` simula a implementação usada por arquiteturas não x86_64.

## Plataformas suportadas

O código é Linux-only e compatível com `x86_64`, `aarch64/arm64`, `armv7` e
outras arquiteturas com `/proc` e `sysfs`. A implementação C portátil é a
referência para arquiteturas sem o caminho assembly.
