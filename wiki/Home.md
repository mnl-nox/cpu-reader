# CPU Reader — Wiki

Bem-vindo ao manual do CPU Reader, uma biblioteca C99 e ferramenta de monitoramento para Linux.

## Navegação

- [Visão geral e conceitos](Overview.md)
- [Instalação](Installation.md)
- [Tutorial de uso](Tutorial.md)
- [API C](API.md)
- [Arquitetura](Architecture.md)
- [Métricas e limitações](Metrics.md)
- [Solução de problemas](Troubleshooting.md)
- [Testes e validação](Testing.md)
- [Contribuição](Contributing.md)
- [FAQ](FAQ.md)

## Em um minuto

~~~sh
sudo apt update
sudo apt install -y git build-essential pkg-config libgtk-4-dev dpkg-dev
git clone https://github.com/mnl-nox/cpu-reader.git
cd cpu-reader
make package-deb
sudo apt install ./build/cpu-reader_*.deb
cpu-reader
~~~

Para instalar somente a biblioteca para desenvolvimento, execute <code>make core</code> e <code>make test</code>. A interface gráfica requer GTK4; o monitor de terminal requer ncurses.

## Estado e precisão

O projeto está em Beta. Uso agregado, modelo e topologia são lidos de interfaces Linux. Temperatura e frequência dependem de sensores e drivers expostos pelo sistema. Quando uma métrica não está disponível, ela deve ser tratada como desconhecida, não como zero.

O diretório <code>wiki/</code> contém páginas Markdown versionadas no repositório. Elas também podem ser copiadas para uma GitHub Wiki se esse recurso for habilitado no projeto.
\n\n## Diagnóstico e auditoria\n\n- [API de diagnóstico por métrica](Diagnostic-API.md)\n- [Confiança e limites da telemetria](../doc/diagnostico-hardware.md)\n