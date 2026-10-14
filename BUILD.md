# Build Guide

Este repositório usa `make` para compilar a biblioteca, o monitor e os testes.

## Pré-requisitos

- Compilador C99, como `gcc` ou `clang`
- GNU Make
- `pkg-config` e `libgtk-4-dev` para a interface GTK4
- `dpkg-deb`/`dpkg-dev` para gerar o pacote Debian
- ncurses de desenvolvimento para compilar `examples/monitor.c` (opcional)

## Alvos disponíveis

```bash
make       # compila libcpu.a e o monitor
make core  # compila apenas libcpu.a
make monitor
make gtk-monitor # compila o frontend GTK4
make test-gtk # smoke test de --version e --help
make test-gtk-ui # testa inicialização da janela via Xvfb
make package-deb # gera um .deb para a arquitetura atual
make test  # compila e executa os testes
make test-portable # testa o fallback C sem assembly x86_64
make test-sanitize # executa AddressSanitizer e UndefinedBehaviorSanitizer
make test-security # executa com flags de hardening
make telemetry # gera resumo JSON local da execução
make clean # remove build/ e libcpu.a
```

## Saídas geradas

- `libcpu.a` - biblioteca estática do núcleo
- `build/cpu-monitor` - monitor ncurses de exemplo
- `build/cpu-reader-gtk` - aplicação desktop GTK4
- `build/cpu-reader_*.deb` - pacote Debian instalável
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

## Segurança e telemetria

`make test-security` habilita proteção de stack, `_FORTIFY_SOURCE`, PIE,
relro/now e verificações de formato. `make test-sanitize` detecta acessos
inválidos e comportamento indefinido durante os testes.

`make telemetry` produz `build/telemetry.json` com compiler, plataforma e
status. Esse arquivo é local e não contém dados da máquina além do identificador
genérico da plataforma; não há envio automático de telemetria.

Consulte [SECURITY.md](SECURITY.md) para o escopo de segurança e o processo de
relato responsável.
