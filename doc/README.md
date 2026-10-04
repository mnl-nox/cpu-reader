# Índice de Documentação - CPU Reader

Este diretório reúne a documentação técnica consolidada do projeto.

## Comece aqui

- [../README.md](../README.md) - visão geral do projeto, build e uso
- [../BUILD.md](../BUILD.md) - instruções de compilação
- [../VALIDATION.md](../VALIDATION.md) - guia de validação

## Requisitos e especificação

- [requerimentos.md](requerimentos.md) - requisitos funcionais implementados e planejados
- [requisitos-nao-funcionais.md](requisitos-nao-funcionais.md) - requisitos não funcionais
- [software-requirements-specification.md](software-requirements-specification.md) - SRS formal

## Arquitetura e design

- [arquitetura.md](arquitetura.md) - visão arquitetural do núcleo e do monitor
- [architecture-decision-records.md](architecture-decision-records.md) - ADRs formalizadas
- [uml-diagrams.md](uml-diagrams.md) - diagramas UML em Mermaid

## Referência histórica

- [requerimentos.md](requerimentos.md) - documentação funcional consolidada
- [arquitetura.md](arquitetura.md) - documentação arquitetural consolidada

## Estrutura atual da documentação

```text
README.md
BUILD.md
VALIDATION.md
doc/
  README.md
  arquitetura.md
  architecture-decision-records.md
  requerimentos.md
  requisitos-nao-funcionais.md
  software-requirements-specification.md
  uml-diagrams.md
```

## Observação

O escopo Beta atual é Linux. A biblioteca suporta `x86_64`, `aarch64/arm64`,
`armv7` e outras arquiteturas Linux por meio do fallback C portátil.
