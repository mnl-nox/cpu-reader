# Guia de compilação e instalação

Este guia descreve a compilação local, os testes, o frontend GTK4 e o pacote Debian. Para uma introdução guiada, consulte o [tutorial](wiki/Tutorial.md).

## Dependências

Base para biblioteca e testes:

~~~sh
sudo apt update
sudo apt install -y build-essential gcc clang make pkg-config cppcheck
~~~

Dependências opcionais por componente:

~~~sh
sudo apt install -y libncurses-dev       # monitor de terminal
sudo apt install -y libgtk-4-dev         # interface desktop GTK4
sudo apt install -y dpkg-dev desktop-file-utils xvfb  # pacote e smoke tests
~~~

## Alvos Make

| Comando | Finalidade |
|---|---|
| <code>make</code> | Compila a biblioteca e o monitor de terminal |
| <code>make core</code> | Compila somente a biblioteca estática |
| <code>make monitor</code> | Compila o monitor ncurses |
| <code>make gtk-monitor</code> | Compila a aplicação GTK4 |
| <code>make test</code> | Executa os testes funcionais, de plataforma, diagnóstico e concorrência |\n| <code>make test-diagnostics</code> | Testa status e valores do relatório de diagnóstico |
| <code>make test-gtk</code> | Verifica <code>--version</code> e <code>--help</code> |
| <code>make test-gtk-ui</code> | Verifica a inicialização da janela via Xvfb |
| <code>make test-sanitize</code> | Executa os testes com ASan e UBSan |
| <code>make test-security</code> | Executa os testes com flags de hardening |
| <code>make test-portable</code> | Compila e testa o caminho C portátil |
| <code>make benchmark</code> | Executa um microbenchmark informativo |
| <code>make telemetry</code> | Cria um resumo local em <code>build/telemetry.json</code> |
| <code>make package-deb</code> | Gera o pacote Debian da arquitetura local |
| <code>make clean</code> | Remove artefatos gerados e a biblioteca estática |

## Compilar o núcleo

~~~sh
make clean
make core
make test
~~~

A saída da biblioteca é <code>libcpu.a</code>. Para compilar uma aplicação que use a API pública, inclua <code>include/cpu.h</code> e vincule a biblioteca estática. Consulte [wiki/API.md](wiki/API.md) e [wiki/Diagnostic-API.md](wiki/Diagnostic-API.md).

## Interface de terminal

Instale <code>libncurses-dev</code> e execute:

~~~sh
make monitor
./build/cpu-monitor
~~~

O monitor é um exemplo separado da biblioteca. Use a tecla <strong>q</strong> para sair.

## Interface desktop GTK4

Instale <code>libgtk-4-dev</code> e <code>pkg-config</code>:

~~~sh
make gtk-monitor
./build/cpu-reader-gtk
~~~

A janela atualiza as métricas aproximadamente a cada segundo. Se o sistema não expuser um sensor de temperatura ou a frequência atual, o painel deve mostrar o dado como indisponível.

## Gerar e instalar o .deb

~~~sh
sudo apt install -y libgtk-4-dev dpkg-dev pkg-config
make package-deb
ls -lh build/cpu-reader_*.deb
sudo apt install ./build/cpu-reader_*.deb
cpu-reader --version
cpu-reader --help
~~~

O pacote instala o executável em <code>/usr/bin/cpu-reader</code>, a biblioteca estática em <code>/usr/lib/$(dpkg-architecture -qDEB_HOST_MULTIARCH)/libcpu.a</code>, os cabeçalhos em <code>/usr/include/cpu-reader/</code>, o arquivo <code>pkg-config</code>, um launcher desktop, o ícone e os metadados AppStream. Consumidores podem usar <code>pkg-config --cflags --libs cpu-reader</code>. Para remover:

~~~sh
sudo apt remove cpu-reader
~~~

A versão gerada pelo Makefile inclui um identificador derivado do commit. Isso é um pacote de build; não representa uma versão estável publicada.

## Validação local da interface e do pacote

~~~sh
make test-gtk
make test-gtk-ui
desktop-file-validate packaging/debian/cpu-reader.desktop
dpkg-deb --info build/cpu-reader_*.deb
dpkg-deb --contents build/cpu-reader_*.deb
~~~

O teste de interface usa Xvfb e não substitui uma inspeção visual em uma sessão gráfica real.

## Limpeza e reprodutibilidade

Execute <code>make clean</code> antes de comparar builds. Os diretórios <code>build/</code> e os binários produzidos são descartáveis e não devem ser versionados. Não execute simultaneamente alvos que limpem e recompilam no mesmo diretório.
