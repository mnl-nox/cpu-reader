# Instalação

## Opção A — compilar e instalar o pacote Debian

Compatível com Debian e Ubuntu quando os pacotes GTK4 estão disponíveis para a distribuição.

~~~sh
sudo apt update
sudo apt install -y git build-essential pkg-config libgtk-4-dev dpkg-dev
git clone https://github.com/mnl-nox/cpu-reader.git
cd cpu-reader
make package-deb
sudo apt install ./build/cpu-reader_*.deb
cpu-reader
~~~

O pacote é gerado para a arquitetura do sistema atual. Ele instala o executável <code>/usr/bin/cpu-reader</code> e os recursos para integração com o desktop.

## Opção B — executar sem instalar

~~~sh
make gtk-monitor
./build/cpu-reader-gtk
~~~

É necessário instalar <code>libgtk-4-dev</code> e <code>pkg-config</code> antes de compilar.

## Opção C — somente a biblioteca C

~~~sh
sudo apt install -y build-essential make
make core
make test
~~~

A biblioteca estática resultante é <code>libcpu.a</code>. Inclua <code>include/cpu.h</code> no seu projeto e vincule a biblioteca.

## Remoção

~~~sh
sudo apt remove cpu-reader
~~~

A remoção do pacote não apaga o clone do código-fonte nem os artefatos locais em <code>build/</code>.

## Pacote temporário de CI

Quando uma execução de GitHub Actions publica um artefato, abra a execução de workflow, encontre **Artifacts** e baixe <code>cpu-reader-deb</code>. Extraia o ZIP para obter o <code>.deb</code>. Esses artefatos expiram e não equivalem a uma release permanente.
