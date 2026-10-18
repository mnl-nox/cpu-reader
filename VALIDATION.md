# Guia de validação

A validação combina testes funcionais, portabilidade, sanitizers, análise estática e smoke tests do frontend e do pacote.

## Ambiente Debian/Ubuntu

~~~sh
sudo apt update
sudo apt install -y git build-essential gcc clang make cppcheck pkg-config libncurses-dev libgtk-4-dev dpkg-dev desktop-file-utils xvfb
~~~

## Fluxo de validação local

~~~sh
git clone https://github.com/mnl-nox/cpu-reader.git
cd cpu-reader
make clean
make test
make test-sanitize
make test-security
make test-portable
cppcheck --enable=warning,style,performance,portability --error-exitcode=1 --inline-suppr --std=c99 -Iinclude src tests examples bench
make benchmark
make telemetry
~~~

Alguns alvos executam limpeza automática; não os execute em paralelo no mesmo diretório de trabalho.

## Frontend GTK4 e pacote Debian

~~~sh
make gtk-monitor
make test-gtk
make test-gtk-ui
make package-deb
desktop-file-validate packaging/debian/cpu-reader.desktop
dpkg-deb --info build/cpu-reader_*.deb
dpkg-deb --contents build/cpu-reader_*.deb
sudo apt install ./build/cpu-reader_*.deb
cpu-reader --version
cpu-reader --help
~~~

O teste de UI deve ser executado em ambiente com servidor gráfico ou Xvfb. A inspeção manual deve confirmar que o uso agregado é atualizado, que os dados indisponíveis são indicados claramente e que a janela permanece utilizável em resoluções menores.

## Critérios de aceitação

- A API retorna dados válidos ou valores de erro documentados.
- O relatório de diagnóstico distingue valor válido, baseline, indisponibilidade e falha por métrica.
- O teste concorrente usa contextos independentes; compartilhar um contexto entre threads exige sincronização externa.
- Snapshots retornados por <code>cpu_get_info()</code> são liberados com <code>cpu_free_info()</code>.
- Uso da CPU fica entre 0 e 100; a primeira amostra estabelece baseline.
- Temperatura e frequência ausentes retornam <code>-1.0f</code>.
- <code>physical_cores == 0</code> indica topologia física desconhecida/indisponível, não ausência de núcleos.
- Falhas em testes funcionais, sanitizers, hardening ou análise estática bloqueiam uma release.
- O pacote declara as dependências necessárias, instala sem erro e executa os smoke tests CLI e GUI.

## CI e release

O workflow CI valida compilação e testes em mais de uma arquitetura, além do frontend GTK4 e do pacote quando o runner fornece as dependências necessárias. A execução sob QEMU é validação por emulação, não um teste em hardware nativo.

O artefato temporário de Actions não é uma release. Para publicar uma versão, siga [RELEASING.md](RELEASING.md) e confirme que o CI correspondente passou antes de criar uma tag.

## Matriz de suporte por distribuição

Consulte [doc/support-matrix.md](doc/support-matrix.md) para separar ambientes efetivamente executados na CI de ambientes apenas esperados. A matriz não implica suporte certificado a todo kernel, hardware ou variante de distribuição.

O workflow `.github/workflows/distro-audit.yml` executa o núcleo e os testes funcionais em Ubuntu, Fedora e Alpine/musl. Falhas de build/teste devem ser corrigidas ou explicitamente justificadas antes de declarar uma plataforma suportada. Não use apenas o sucesso da compilação como evidência de funcionamento correto.
