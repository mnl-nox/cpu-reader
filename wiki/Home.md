# CPU Reader — Manual do projeto

CPU Reader é uma biblioteca C99 e um conjunto de ferramentas de diagnóstico de CPU para Linux. O projeto está em beta e está evoluindo, por etapas, para telemetria mais ampla do sistema.

## Navegação

- [README e início rápido](../README.md)
- [Instalação e compilação](../BUILD.md)
- [Tutorial de uso](Tutorial.md)
- [Referência da API C](API.md)
- [API de diagnóstico por métrica](Diagnostic-API.md)
- [Perguntas frequentes (Q&A)](FAQ.md)
- [Arquitetura atual](../doc/arquitetura.md)
- [Métricas e limitações](Metrics.md)
- [Solução de problemas](Troubleshooting.md)
- [Testes e validação](Testing.md)
- [Guia de QA e relatórios](../doc/qa-and-reporting.md)
- [Roadmap de telemetria](../doc/system-telemetry-roadmap.md)
- [Contribuição](Contributing.md)
- [Segurança](../SECURITY.md)

## Em um minuto

```sh
git clone https://github.com/mnl-nox/cpu-reader.git
cd cpu-reader
make core
make test
```

Para compilar a interface desktop, instale GTK4 e execute `make gtk-monitor`. Para o monitor de terminal, instale ncurses e execute `make monitor`. A criação de um pacote Debian local é documentada em [BUILD.md](../BUILD.md).

## Estado e precisão

A implementação atual cobre informações de CPU, uso agregado, frequência/temperatura opcionais e diagnóstico. **Não é ainda um monitor completo de todos os recursos do sistema.** Coleta pública por CPU lógica, memória/swap/PSI, rede e I/O de armazenamento está descrita no [roadmap](../doc/system-telemetry-roadmap.md).

A disponibilidade depende do kernel, dos drivers, da virtualização, das permissões e do ambiente. Valores ausentes ou inválidos não devem ser tratados como zero. Consulte os contratos e valores de retorno na [API](API.md).

## Diagnóstico e confiança

O relatório de diagnóstico pode ser válido mesmo quando uma métrica opcional está indisponível. Verifique status, erro, mensagem e fonte por métrica. A telemetria é uma observação do sistema operacional, não uma atestação criptográfica de hardware, firmware ou kernel.

Antes de publicar logs, remova dados pessoais e informações desnecessárias do equipamento. Para problemas comuns, abra uma [issue](https://github.com/mnl-nox/cpu-reader/issues); para vulnerabilidades, siga [SECURITY.md](../SECURITY.md).
