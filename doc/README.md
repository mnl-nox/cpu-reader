# Índice da documentação técnica

Este índice organiza a documentação por tarefa e identifica a fonte de verdade para comportamento implementado. O projeto está em beta; funcionalidades no roadmap não devem ser tratadas como disponíveis.

## Comece por aqui

- [README do projeto](../README.md) — visão geral, escopo, build e exemplo mínimo.
- [Guia de compilação e instalação](../BUILD.md) — dependências, alvos Make e pacote Debian.
- [Tutorial de uso](../wiki/Tutorial.md) — instalação e interpretação das métricas.
- [Perguntas frequentes (Q&A)](../wiki/FAQ.md) — questões comuns de uso e limitações.
- [Referência da API C](../wiki/API.md) — funções, ownership e concorrência.
- [API de diagnóstico](../wiki/Diagnostic-API.md) — estados, erros e proveniência.
- [Guia de QA, validação e relatórios](qa-and-reporting.md) — como testar e registrar evidências.

## Especificação técnica

- [Arquitetura atual](arquitetura.md)
- [Roadmap de telemetria de sistema](system-telemetry-roadmap.md)
- [Requisitos funcionais](requerimentos.md)
- [Requisitos não funcionais](requisitos-nao-funcionais.md)
- [Registros de decisões arquiteturais (ADRs)](architecture-decision-records.md)
- [Diagramas UML](uml-diagrams.md)

## Diagnóstico, plataforma e segurança

- [Diagnóstico de hardware e limites de confiança](diagnostico-hardware.md)
- [Matriz de suporte Linux](support-matrix.md)
- [Política de segurança](../SECURITY.md)
- [Guia de validação local e CI](../VALIDATION.md)

## Operação e contribuição

- [Histórico de alterações](../CHANGELOG.md)
- [Política de release](../RELEASING.md)
- [Como contribuir](../CONTRIBUTING.md)
- [Issues abertas](https://github.com/mnl-nox/cpu-reader/issues)
- [Pull requests](https://github.com/mnl-nox/cpu-reader/pulls)

## Fonte de verdade

O comportamento disponível deve ser confirmado nos cabeçalhos públicos de `include/`, na implementação em `src/`, nos testes em `tests/` e nos alvos do `Makefile`. A documentação descreve esses contratos e seus limites; em caso de divergência, abra uma issue para corrigir o código ou a documentação.

O roadmap define capacidades planejadas e critérios de aceite, não uma promessa de que essas capacidades já estejam implementadas. Relatórios de validação devem distinguir testes executados, testes não executados e limitações conhecidas.
