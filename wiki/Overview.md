# Visão geral

CPU Reader é uma biblioteca C99 para consultar métricas da CPU no Linux. Ela inclui exemplos que demonstram como integrar a API em ferramentas de terminal e em uma aplicação GTK4.

## Componentes

- **Biblioteca:** funções públicas em <code>include/cpu.h</code> e implementação em <code>src/</code>.
- **Monitor ncurses:** exemplo de terminal em <code>examples/monitor.c</code>.
- **Monitor GTK4:** dashboard desktop em <code>examples/gtk_monitor.c</code>.
- **Testes:** casos funcionais e de plataforma em <code>tests/</code>.
- **Empacotamento:** launcher, ícone e metadados em <code>packaging/</code>.

## Objetivos

- Fornecer uma API pequena e previsível em C.
- Ler métricas sem depender de ferramentas externas de monitoramento.
- Tratar sensores opcionais como opcionais.
- Permitir testes com caminhos de arquivos alternativos.
- Manter a biblioteca desacoplada dos frontends.

## Fora do escopo atual

O projeto não fornece API web, não suporta Windows/macOS e não promete métricas por núcleo. Não substitui ferramentas de diagnóstico de hardware nem garante que todo equipamento exponha temperatura ou frequência.
