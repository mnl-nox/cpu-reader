# Validation Guide

Este documento define um roteiro reproduzível para testar a biblioteca antes de integrá-la ou publicá-la.

## Requisitos

- Linux
- GCC ou Clang com suporte a C99
- GNU Make
- Cppcheck para análise estática
- ncurses de desenvolvimento apenas para compilar o monitor

No Debian/Ubuntu:

```sh
sudo apt update
sudo apt install -y git build-essential gcc clang make cppcheck libncurses-dev pkg-config libgtk-4-dev dpkg-dev desktop-file-utils xvfb
```

## Primeiro teste (clone limpo)

```sh
git clone https://github.com/mnl-nox/cpu-reader.git
cd cpu-reader
make clean
make test
make
./build/cpu-monitor
```

O monitor requer um terminal interativo; pressione `q` para sair. Para validar somente a biblioteca, use `make core` e `make test`.

## Matriz de validação local

Execute em etapas para identificar precisamente a origem de uma falha:

```sh
make clean
make test
make test-sanitize
make test-security
cppcheck --enable=warning,style,performance,portability --error-exitcode=1 --inline-suppr --std=c99 -Iinclude src tests examples bench
make benchmark
make telemetry
```

- `make test`: testes funcionais da API e testes de plataforma.
- `make test-sanitize`: AddressSanitizer e UndefinedBehaviorSanitizer.
- `make test-security`: flags de hardening no compilador e linker.
- `cppcheck`: análise estática; qualquer diagnóstico configurado como erro faz o comando falhar.
- `make benchmark`: resultado informativo, dependente do host, sem limiar de aprovação.
- `make telemetry`: grava somente um resumo local em `build/telemetry.json`.

Alguns alvos chamam `make clean`; não os execute em paralelo no mesmo diretório de trabalho.

## Testes sem hardware específico

Use os overrides documentados no README para direcionar o leitor a fixtures em arquivos temporários. Configure variáveis de ambiente antes de iniciar threads: `setenv()`/`unsetenv()` concorrentes com chamadas à biblioteca não são seguros.

## Semântica e critérios de aceitação

- `cpu_get_info()`: retorna snapshot ou `NULL`; liberar snapshots com `cpu_free_info()`.
- `cpu_get_usage*()`: retorna percentual de 0 a 100, ou `-1.0f` em falha. A primeira amostra de um contexto estabelece baseline e retorna zero.
- `physical_cores == 0`: topologia física desconhecida/indisponível; não significa que a CPU não possui núcleos.
- Temperatura e frequência são métricas opcionais. `-1.0f` indica que não existe leitura válida disponível.
- Contextos explícitos não são thread-safe quando compartilhados sem sincronização externa.
- Falha em qualquer teste funcional, sanitizer, hardening ou análise estática bloqueia a promoção de versão.

## CI e release

Pull requests executam a validação do projeto. Para o fluxo de tags e criação de releases, consulte [RELEASING.md](RELEASING.md). A CI testa x86_64 e aarch64 nativamente, e ARMv7 sob QEMU; emulação não equivale a teste em hardware ARMv7 nativo.

## Frontend GTK4 e pacote .deb

~~~sh
make clean
make test
make test-gtk
make test-gtk-ui
make package-deb
desktop-file-validate packaging/debian/cpu-reader.desktop
dpkg-deb --info build/cpu-reader_*.deb
dpkg-deb --contents build/cpu-reader_*.deb
sudo apt install ./build/cpu-reader_*.deb
cpu-reader --version
cpu-reader --help
sh tests/test_gtk_ui.sh cpu-reader
~~~

O smoke test não exige servidor gráfico. A validação visual da janela GTK4 deve
ser realizada em uma sessão desktop Linux; em máquinas headless, execute os
testes de compilação e do pacote sem iniciar a janela. O workflow de CI gera um
artefato temporário .deb, sem publicar tags ou GitHub Releases.
