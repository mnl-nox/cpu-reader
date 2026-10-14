# Solução de problemas

## Erro: GTK4 ou pkg-config ausente

Instale as dependências e tente novamente:

~~~sh
sudo apt update
sudo apt install -y libgtk-4-dev pkg-config
make gtk-monitor
~~~

## Erro ao gerar .deb

Verifique se <code>dpkg-deb</code> está instalado:

~~~sh
sudo apt install -y dpkg-dev
make package-deb
~~~

## Temperatura aparece como indisponível

Nem todo equipamento publica temperatura por uma interface compatível. Verifique os sensores disponíveis no sistema e os drivers carregados. Não interprete o valor indisponível como temperatura zero. Evite executar o aplicativo como root apenas para tentar habilitar a leitura.

## Frequência aparece como indisponível

A leitura pode depender do driver de frequência e dos arquivos expostos pelo kernel. O fallback para <code>/proc/cpuinfo</code> também pode não existir em todas as arquiteturas.

## Núcleos físicos aparece como zero

Isso significa que a topologia física não foi identificada com segurança pelos dados disponíveis. O número de processadores lógicos é uma métrica separada.

## O monitor não abre via SSH ou terminal sem desktop

A aplicação GTK4 precisa de uma sessão gráfica. Em CI/headless, use os smoke tests com Xvfb. Para observar métricas num terminal, compile o exemplo ncurses com <code>make monitor</code>.

## Os testes falham no primeiro uso de CPU

A primeira leitura de uso estabelece o baseline; é esperado que ela retorne zero. Chamadas subsequentes calculam a diferença entre amostras.

## Limpeza e reconstrução

~~~sh
make clean
make test
make package-deb
~~~

Para relatar um bug, inclua sistema operacional, arquitetura, comandos executados e mensagens de erro relevantes. Remova informações pessoais dos logs antes de compartilhar.
