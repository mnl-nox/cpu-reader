# 📚 Índice de Documentação - CPU Reader

**Última atualização:** 2026-09-04  
**Status:** ✅ COMPLETO

Bem-vindo à documentação do projeto CPU Reader. Este guia ajuda a navegar por todos os documentos disponíveis.

---

## 🎯 Comece Aqui

- **[README.md](../README.md)** - Visão geral do projeto, compilação e uso
- **[CONSOLIDACAO-VALIDACAO.md](CONSOLIDACAO-VALIDACAO.md)** - Sumário completo da consolidação realizada em 2026-09-04

---

## 📋 Documentação por Tipo

### Requisitos e Especificação

| Documento                                                                        | Descrição                                                             | Linhas | Versão |
| -------------------------------------------------------------------------------- | --------------------------------------------------------------------- | ------ | ------ |
| [requisitos-funcionais.md](requisitos-funcionais.md)                             | RF-001 a RF-015: Funcionalidades implementadas e futuras              | 850+   | 2.0    |
| [requisitos-nao-funcionais.md](requisitos-nao-funcionais.md)                     | RNF-001 a RNF-012: Compatibilidade, performance, segurança, etc.      | 450+   | 2.0    |
| [software-requirements-specification.md](software-requirements-specification.md) | SRS formal: Escopo, critérios de aceitação, matriz de rastreabilidade | 700+   | 2.0    |
| [requerimentos.md](requerimentos.md)                                             | Original: Requisitos funcionais (referência)                          | -      | 1.0    |

### Arquitetura e Design

| Documento                                                            | Descrição                                                       | Linhas | Versão |
| -------------------------------------------------------------------- | --------------------------------------------------------------- | ------ | ------ |
| [architecture-decision-records.md](architecture-decision-records.md) | ADR-0001 a ADR-0013: Decisões arquiteturais formalizadas        | 900+   | 2.0    |
| [design.md](design.md)                                               | Design Document: Componentes, padrões, trade-offs, roadmap      | 800+   | 2.0    |
| [uml-diagrams.md](uml-diagrams.md)                                   | 14 Diagramas UML em Mermaid (classes, sequência, estado, fluxo) | 900+   | 2.0    |
| [arquitetura.md](arquitetura.md)                                     | Original: Visão geral de arquitetura (referência)               | -      | 1.0    |
| [decisao-design.md](decisao-design.md)                               | Original: Decisões de design (referência)                       | -      | 1.0    |

### Planejamento e Roadmap

| Documento                                                | Descrição                                                | Linhas | Versão |
| -------------------------------------------------------- | -------------------------------------------------------- | ------ | ------ |
| [roadmap-evolucao-semana.md](roadmap-evolucao-semana.md) | Roadmap de 8 semanas: Tarefas diárias, backlog, métricas | 650+   | 2.0    |
| [prioridades-sdlc.md](prioridades-sdlc.md)               | Prioridades P1-P4: Estado concluído, próximos passos     | 200+   | 2.0    |

### Casos de Uso e Critérios

| Documento                      | Descrição                                                | Linhas |
| ------------------------------ | -------------------------------------------------------- | ------ |
| [casosdeuso.md](casosdeuso.md) | Casos de uso 1-4: Exemplos de interação com a biblioteca |        |
| [criterios.md](criterios.md)   | Critérios de aceitação: Build, API, Monitor, Limitações  |        |

### Referência

| Documento                    | Descrição                                          | Linhas |
| ---------------------------- | -------------------------------------------------- | ------ |
| [diagramas.md](diagramas.md) | Diagramas originais: Componentes, leitura, cálculo | 100+   |

---

## 🗂️ Guia de Navegação por Persona

### 👨‍💻 Para Desenvolvedores

**Comece com:**

1. [README.md](../README.md) - Compilação e execução
2. [requisitos-funcionais.md](requisitos-funcionais.md) - O que você pode fazer com a API
3. [design.md](design.md) - Arquitetura interna

**Depois leia:**

- [architecture-decision-records.md](architecture-decision-records.md) - Por que as coisas são assim
- [uml-diagrams.md](uml-diagrams.md) - Diagramas de classes e sequência

### 🏛️ Para Arquitetos

**Leitura essencial:**

1. [design.md](design.md) - Visão de alto nível
2. [architecture-decision-records.md](architecture-decision-records.md) - Decisões formalizadas
3. [uml-diagrams.md](uml-diagrams.md) - Todos os 14 diagramas

**Análise aprofundada:**

- [software-requirements-specification.md](software-requirements-specification.md) - Escopo e limites
- [roadmap-evolucao-semana.md](roadmap-evolucao-semana.md) - Próximos passos

### 🧪 Para QA/Testes

**Prioridade:**

1. [requisitos-funcionais.md](requisitos-funcionais.md) - Critérios de aceitação por RF
2. [requisitos-nao-funcionais.md](requisitos-nao-funcionais.md) - RNFs mensuráveis
3. [software-requirements-specification.md](software-requirements-specification.md) - Matriz de rastreabilidade

**Implementação:**

- [casosdeuso.md](casosdeuso.md) - Cenários de teste
- [criterios.md](criterios.md) - Limites e comportamento esperado

### 👔 Para Stakeholders/PM

**Visão executiva:**

1. [CONSOLIDACAO-VALIDACAO.md](CONSOLIDACAO-VALIDACAO.md) - Sumário de tudo
2. [roadmap-evolucao-semana.md](roadmap-evolucao-semana.md) - Plano de 8 semanas
3. [prioridades-sdlc.md](prioridades-sdlc.md) - Roadmap de features

**Detalhe técnico:**

- [software-requirements-specification.md](software-requirements-specification.md) - Escopo formal

---

## 📊 Estatísticas da Documentação

| Métrica                                | Valor  |
| -------------------------------------- | ------ |
| Total de documentos                    | 15     |
| Documentos novos (2026-09-04)          | 7      |
| Documentos atualizados                 | 2      |
| Documentos originais (referência)      | 6      |
| Linhas totais de documentação          | ~6500+ |
| Diagramas UML                          | 14     |
| Requisitos funcionais documentados     | 12     |
| Requisitos não-funcionais documentados | 12     |
| ADRs formalizadas                      | 13     |
| Critérios de aceitação                 | 100+   |

---

## 🔗 Mapa Conceitual

```
ESCOPO DO PROJETO
    ├── REQUISITOS
    │   ├── Funcionais (RF-001 a RF-012 ✅)
    │   └── Não-Funcionais (RNF-001 a RNF-012 ✅)
    │
    ├── ARQUITETURA
    │   ├── Decisões (ADR-0001 a ADR-0013)
    │   ├── Design (Componentes, Padrões)
    │   └── Diagramas (14 UML)
    │
    ├── IMPLEMENTAÇÃO
    │   ├── Código (src/*, include/*, examples/*, tests/*)
    │   └── Build (Makefile)
    │
    ├── VALIDAÇÃO
    │   ├── Critérios de Aceitação
    │   ├── Casos de Uso
    │   └── Matriz de Rastreabilidade
    │
    └── PLANEJAMENTO
        ├── Roadmap (8 semanas)
        ├── Prioridades (P1-P4)
        └── Evolução (v0.2.0 → v1.0.0)
```

---

## 📈 Progressão de Leitura Recomendada

### Primeira Vez? 30 minutos

1. [README.md](../README.md) (5 min)
2. [casosdeuso.md](casosdeuso.md) (5 min)
3. [CONSOLIDACAO-VALIDACAO.md](CONSOLIDACAO-VALIDACAO.md) - Seção 1-3 (10 min)
4. [requisitos-funcionais.md](requisitos-funcionais.md) - Visão geral (10 min)

### Deep Dive? 2 horas

1. [requisitos-funcionais.md](requisitos-funcionais.md) (30 min)
2. [requisitos-nao-funcionais.md](requisitos-nao-funcionais.md) (20 min)
3. [design.md](design.md) (30 min)
4. [architecture-decision-records.md](architecture-decision-records.md) - ADR-0001 a ADR-0010 (20 min)
5. [uml-diagrams.md](uml-diagrams.md) - Diagramas principais (20 min)

### Completo? 8 horas

Ler todos os documentos na ordem sugerida acima +

- [software-requirements-specification.md](software-requirements-specification.md) (1h)
- [roadmap-evolucao-semana.md](roadmap-evolucao-semana.md) (1h)
- [architecture-decision-records.md](architecture-decision-records.md) - Completo (30 min)
- [uml-diagrams.md](uml-diagrams.md) - Completo (30 min)

---

## 🎓 Documentação por Conceito

### Funcionalidades Core

- `cpu_get_info()` → [requisitos-funcionais.md#RF-003](requisitos-funcionais.md)
- `cpu_get_usage()` → [requisitos-funcionais.md#RF-005](requisitos-funcionais.md)
- `cpu_get_temperature()` → [requisitos-funcionais.md#RF-007](requisitos-funcionais.md)
- `cpu_get_clock_speed()` → [requisitos-funcionais.md#RF-008](requisitos-funcionais.md)

### Decisões Arquiteturais

- Por que C99? → [architecture-decision-records.md#ADR-0001](architecture-decision-records.md)
- Por que API em estrutura? → [architecture-decision-records.md#ADR-0002](architecture-decision-records.md)
- Por que /proc? → [architecture-decision-records.md#ADR-0003](architecture-decision-records.md)
- Por que thread-unsafe? → [architecture-decision-records.md#ADR-0008](architecture-decision-records.md)

### Trade-offs de Design

- Veja [design.md#5-trade-offs](design.md) para matriz completa

### Diagramas Importantes

- Classes: [uml-diagrams.md#1-diagrama-de-classes](uml-diagrams.md)
- Fluxo de leitura: [uml-diagrams.md#3-diagrama-de-sequencia-leitura](uml-diagrams.md)
- Cálculo de uso: [uml-diagrams.md#4-diagrama-de-sequencia-calculo-de-uso](uml-diagrams.md)
- Arquitetura: [uml-diagrams.md#7-hierarquia-de-chamadas](uml-diagrams.md)

---

## ✅ Checklist de Documentação

A documentação está **100% completa** para:

- [x] Todos os requisitos funcionais (RF-001 a RF-012)
- [x] Todos os requisitos não-funcionais (RNF-001 a RNF-012)
- [x] Arquitetura (10 componentes documentados)
- [x] Decisões (13 ADRs formalizadas)
- [x] Diagramas (14 UML em Mermaid)
- [x] Casos de uso (4 cenários)
- [x] Critérios de aceitação (100+)
- [x] Roadmap (8 semanas)
- [x] Planejamento (P1-P4 com timeline)

---

## 🔄 Histórico de Atualizações

| Data       | Versão | Mudanças                                            |
| ---------- | ------ | --------------------------------------------------- |
| 2026-01    | 1.0    | Documentação inicial                                |
| 2026-09-04 | 2.0    | Consolidação completa (7 novos docs, 2 atualizados) |

---

## 📞 Suporte e Dúvidas

- **Questões sobre requisitos?** → [requisitos-funcionais.md](requisitos-funcionais.md) ou [requisitos-nao-funcionais.md](requisitos-nao-funcionais.md)
- **Questões sobre arquitetura?** → [design.md](design.md) ou [architecture-decision-records.md](architecture-decision-records.md)
- **Questões sobre testes?** → [software-requirements-specification.md](software-requirements-specification.md) Seção 8
- **Questões sobre roadmap?** → [roadmap-evolucao-semana.md](roadmap-evolucao-semana.md)
- **Questões gerais?** → [CONSOLIDACAO-VALIDACAO.md](CONSOLIDACAO-VALIDACAO.md)

---

## 🚀 Próximos Passos

1. **Revisar** este índice com a equipe
2. **Executar** validação (semana 2)
3. **Configurar** CI/CD (semana 3)
4. **Publicar** v0.2.0 (semana 4)

---

**Documento:** Índice de Documentação  
**Versão:** 2.0  
**Data:** 2026-09-04  
**Manutenedor:** Arquiteto do Projeto  
**Status:** ✅ COMPLETO
