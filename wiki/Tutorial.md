# Tutorial de uso

Este tutorial mostra como instalar, abrir a interface, interpretar as métricas e executar os testes.

## 1. Instalar

~~~sh
sudo apt update
sudo apt install -y git build-essential pkg-config libgtk-4-dev dpkg-dev
git clone https://github.com/mnl-nox/cpu-reader.git
cd cpu-reader
make package-deb
sudo apt install ./build/cpu-reader_*.deb
~~~

## 2. Abrir o monitor

No menu de aplicativos, procure CPU Reader. Também é possível iniciar pelo terminal:

~~~sh
cpu-reader
~~~

Para conferir opções de linha de comando:

~~~sh
cpu-reader --help
cpu-reader --version
~~~

## 3. Ler o painel

- **Uso da CPU:** percentual agregado calculado comparando leituras de <code>/proc/stat</code>. Na primeira amostra é estabelecido o baseline.
- **Modelo:** nome informado pelo kernel.
- **Núcleos físicos:** estimativa baseada na topologia que o sistema expõe. Zero pode significar que o sistema não forneceu essa informação.
- **Processadores lógicos:** processadores lógicos visíveis para o sistema.
- **Processos em execução:** valor lido de <code>/proc/loadavg</code>.
- **Temperatura:** depende de um sensor compatível e de sua exposição pelo kernel.
- **Frequência:** frequência atual quando o driver disponibiliza a leitura.

## 4. Prévia ilustrativa

A imagem [dashboard-preview.svg](../site/assets/dashboard-preview.svg) é uma prévia esquemática da interface com valores fictícios. Ela documenta a organização visual, não é uma captura real de um computador e não deve ser usada para inferir leituras do seu hardware.

Para produzir uma captura real no seu sistema, abra o aplicativo em uma sessão gráfica, deixe as métricas atualizarem e faça uma captura de tela usando a ferramenta do ambiente desktop. Antes de publicar, confira se a imagem não revela informações pessoais ou dados do equipamento que você não queira compartilhar.

## 5. Validar a instalação

~~~sh
cpu-reader --version
cpu-reader --help
make test
make test-gtk
make test-gtk-ui
~~~

Os comandos Make devem ser executados dentro do diretório do repositório. Os dois últimos testes exigem que a aplicação GTK4 tenha sido compilada; o teste de janela pode usar Xvfb.

## 6. Encerrar e remover

Feche a janela normalmente. Para remover o pacote:

~~~sh
sudo apt remove cpu-reader
~~~

Para reconstruir do zero no clone:

~~~sh
make clean
make package-deb
~~~
