# Prioridades de Desenvolvimento SDLC - CPU Reader

## 1. Overview do SDLC

O projeto segue um modelo de desenvolvimento híbrido entre **Waterfall** (para planejamento) e **Agile** (para iterações). O ciclo de vida é dividido em **3 fases principais** com releases incrementais.

```
┌──────────────────────────────────────────────────────────┐
│           SDLC - CPU Reader Timeline                     │
├──────────────────────────────────────────────────────────┤
│ Fase 1: MVP (v0.1)  │ Fase 2: Release (v1.0) │ Fase 3:  │
│ 2026-09-01 a        │ 2026-10-01 a            │ Maint.   │
│ 2026-09-30 (4 sems) │ 2026-12-01 (13 sems)    │ & Ext.   │
└──────────────────────────────────────────────────────────┘
```

---

## 2. Fases de Desenvolvimento

### FASE 1: MVP - Proof of Concept (v0.1)

**Período**: 2026-09-01 até 2026-09-30 (4 semanas)  
**Objetivo**: Demonstrar funcionalidade básica

#### Sprint 1.1: Setup e Leitura Básica (Semana 1-2)

**Prioridade Geral**: CRÍTICA

| #     | Task                                 | Prioridade | Estimativa | Responsável |
| ----- | ------------------------------------ | ---------- | ---------- | ----------- |
| 1.1.1 | Configurar projeto (Makefile, Git)   | Crítica    | 2h         | Tech Lead   |
| 1.1.2 | Criar estrutura de arquivos          | Crítica    | 1h         | Tech Lead   |
| 1.1.3 | Implementar leitura de /proc/cpuinfo | Crítica    | 4h         | Dev 1       |
| 1.1.4 | Parser de cpuinfo (cores, threads)   | Crítica    | 3h         | Dev 1       |
| 1.1.5 | Estrutura cpu_info_t                 | Crítica    | 1h         | Dev 1       |
| 1.1.6 | Função cpu_get_info()                | Crítica    | 2h         | Dev 1       |
| 1.1.7 | Tratamento básico de erros           | Alta       | 2h         | Dev 2       |

**Deliverables**:

- ✓ Projeto compila sem erros
- ✓ cpu_get_info() funciona
- ✓ Cores e threads retornados corretamente

**DoD (Definition of Done)**:

- [ ] Código compila sem warnings
- [ ] Testes unitários passam
- [ ] Documentação atualizada
- [ ] Code review aprovado

---

#### Sprint 1.2: CPU Usage e Validação (Semana 3-4)

**Prioridade Geral**: CRÍTICA

| #     | Task                              | Prioridade | Estimativa | Responsável |
| ----- | --------------------------------- | ---------- | ---------- | ----------- |
| 1.2.1 | Implementar leitura de /proc/stat | Crítica    | 3h         | Dev 1       |
| 1.2.2 | Algoritmo de cálculo de CPU usage | Crítica    | 4h         | Dev 1       |
| 1.2.3 | Função cpu_get_usage()            | Crítica    | 2h         | Dev 1       |
| 1.2.4 | Validação de dados                | Alta       | 2h         | Dev 2       |
| 1.2.5 | Testes unitários                  | Alta       | 3h         | QA          |
| 1.2.6 | Documentação API                  | Alta       | 2h         | Tech Writer |
| 1.2.7 | Exemplo básico de uso             | Média      | 1h         | Dev 2       |

**Deliverables**:

- ✓ cpu_get_usage() funciona
- ✓ Validação de dados implementada
- ✓ Testes com 80%+ cobertura
- ✓ Documentação básica do README

**Métricas v0.1**:

- Linhas de código: ~500
- Funções públicas: 4 (get_info, get_usage, free_info, init)
- Cobertura de testes: 80%+
- Bugs knowns: 0 críticos

---

### FASE 2: Release Estável (v1.0)

**Período**: 2026-10-01 até 2026-12-01 (13 semanas)  
**Objetivo**: Produto production-ready

#### Sprint 2.1: Temperatura e Robustez (Semana 1-2)

**Prioridade Geral**: ALTA

| #     | Task                                | Prioridade | Estimativa | Responsável |
| ----- | ----------------------------------- | ---------- | ---------- | ----------- |
| 2.1.1 | Implementar leitura de temperature  | Alta       | 3h         | Dev 1       |
| 2.1.2 | Suporte a múltiplos sensores        | Alta       | 2h         | Dev 1       |
| 2.1.3 | cpu_get_temperature()               | Alta       | 2h         | Dev 1       |
| 2.1.4 | Tratamento de sensors indisponíveis | Alta       | 2h         | Dev 2       |
| 2.1.5 | Stress tests                        | Alta       | 3h         | QA          |
| 2.1.6 | Performance benchmarks              | Média      | 2h         | QA          |

**Deliverables**:

- ✓ Temperatura funciona quando disponível
- ✓ Graceful degradation se sensor falta
- ✓ Stress tests passam

---

#### Sprint 2.2: Arquitetura e Refactoring (Semana 3-4)

**Prioridade Geral**: ALTA

| #     | Task                      | Prioridade | Estimativa | Responsável |
| ----- | ------------------------- | ---------- | ---------- | ----------- |
| 2.2.1 | Code refactoring          | Alta       | 4h         | Dev 1       |
| 2.2.2 | Separação de concerns     | Alta       | 3h         | Dev 1       |
| 2.2.3 | Melhorar headers públicos | Alta       | 2h         | Dev 2       |
| 2.2.4 | Static analysis           | Alta       | 2h         | QA          |
| 2.2.5 | Memory leak detection     | Alta       | 2h         | QA          |

**Deliverables**:

- ✓ Código com cobertura 85%+
- ✓ Zero memory leaks (valgrind clean)
- ✓ Zero static analysis warnings

---

#### Sprint 2.3: Portabilidade e CI/CD (Semana 5-6)

**Prioridade Geral**: ALTA

| #     | Task                             | Prioridade | Estimativa | Responsável |
| ----- | -------------------------------- | ---------- | ---------- | ----------- |
| 2.3.1 | Testes em múltiplas arquiteturas | Alta       | 3h         | Infra       |
| 2.3.2 | Suporte ARM/ARM64                | Alta       | 3h         | Dev 1       |
| 2.3.3 | Setup CI/CD (GitHub Actions)     | Alta       | 4h         | Infra       |
| 2.3.4 | Testes automáticos em CI         | Alta       | 3h         | Infra       |
| 2.3.5 | Build em múltiplos compiladores  | Alta       | 2h         | Infra       |

**Deliverables**:

- ✓ CI/CD pipeline operacional
- ✓ Testes passam em x86, ARM
- ✓ Suporte GCC 5.0+, Clang 3.5+

---

#### Sprint 2.4: Documentação Completa (Semana 7-8)

**Prioridade Geral**: ALTA

| #     | Task                 | Prioridade | Estimativa | Responsável |
| ----- | -------------------- | ---------- | ---------- | ----------- |
| 2.4.1 | README completo      | Alta       | 2h         | Tech Writer |
| 2.4.2 | API documentation    | Alta       | 3h         | Tech Writer |
| 2.4.3 | Exemplos de uso      | Alta       | 2h         | Dev 2       |
| 2.4.4 | Guia de contribuição | Média      | 1h         | Tech Lead   |
| 2.4.5 | CHANGELOG            | Alta       | 1h         | Tech Lead   |

**Deliverables**:

- ✓ README.md completo
- ✓ Documentação API com exemplos
- ✓ CHANGELOG v1.0

---

#### Sprint 2.5: Security e Performance (Semana 9-10)

**Prioridade Geral**: ALTA

| #     | Task                     | Prioridade | Estimativa | Responsável |
| ----- | ------------------------ | ---------- | ---------- | ----------- |
| 2.5.1 | Security audit           | Alta       | 3h         | Security    |
| 2.5.2 | Fuzzing tests            | Alta       | 3h         | QA          |
| 2.5.3 | Performance optimization | Alta       | 4h         | Dev 1       |
| 2.5.4 | Benchmark suite          | Média      | 2h         | QA          |

**Deliverables**:

- ✓ Zero security vulnerabilities
- ✓ Performance targets atingidos
- ✓ Benchmark suite estabelecido

---

#### Sprint 2.6: QA e Release Prep (Semana 11-13)

**Prioridade Geral**: CRÍTICA

| #     | Task               | Prioridade | Estimativa | Responsável |
| ----- | ------------------ | ---------- | ---------- | ----------- |
| 2.6.1 | Regression testing | Crítica    | 3h         | QA          |
| 2.6.2 | UAT com usuários   | Alta       | 4h         | PM          |
| 2.6.3 | Bug fixes          | Alta       | 4h         | Dev Team    |
| 2.6.4 | Release notes      | Alta       | 1h         | Tech Writer |
| 2.6.5 | Tag v1.0 release   | Crítica    | 0.5h       | Tech Lead   |
| 2.6.6 | Publicar artifacts | Crítica    | 0.5h       | Infra       |

**Deliverables**:

- ✓ v1.0 released e publicado
- ✓ Release notes publicadas
- ✓ Artifacts disponíveis para download

---

### FASE 3: Manutenção e Extensão (v1.1+)

**Período**: 2026-12-02 em diante  
**Objetivo**: Melhorias e correções contínuas

#### Sprint 3.1: Feedback e Bug Fixes (Ongoing)

**Prioridade Geral**: MÉDIA

| #     | Task              | Prioridade | Estimativa | Responsável |
| ----- | ----------------- | ---------- | ---------- | ----------- |
| 3.1.1 | Monitorar issues  | Média      | 2h/sem     | Support     |
| 3.1.2 | Priorizar bugs    | Média      | 1h/sem     | Tech Lead   |
| 3.1.3 | Hotfixes críticas | Crítica    | Var.       | Dev Team    |
| 3.1.4 | Feature requests  | Baixa      | 1h/sem     | PM          |

---

#### Sprint 3.2: Recursos Futuros (Planejado)

**Prioridade Geral**: BAIXA

| #     | Recurso                      | Versão | Prioridade |
| ----- | ---------------------------- | ------ | ---------- |
| 3.2.1 | Thread-safety com mutex      | v1.1   | Média      |
| 3.2.2 | Suporte a histórico de dados | v1.1   | Média      |
| 3.2.3 | Callback de eventos          | v1.2   | Baixa      |
| 3.2.4 | Suporte Windows              | v2.0   | Baixa      |
| 3.2.5 | Interface Rust               | v2.0   | Baixa      |

---

## 3. Matriz de Priorização (MoSCoW)

### Must Have (Crítico - v1.0)

- [ ] Leitura de informações básicas (cores, threads, modelo)
- [ ] Cálculo de CPU usage
- [ ] Tratamento robusto de erros
- [ ] Suporte para x86 e ARM
- [ ] Documentação completa
- [ ] 80%+ cobertura de testes
- [ ] Zero memory leaks
- [ ] CI/CD pipeline

### Should Have (Alta - v1.0-v1.1)

- [ ] Leitura de temperatura
- [ ] Performance benchmarks
- [ ] Static analysis limpo
- [ ] Suporte a múltiplos sensores
- [ ] Exemplos de uso
- [ ] Logging para debug

### Could Have (Média - v1.1+)

- [ ] Thread-safety
- [ ] Histórico de dados
- [ ] Cache avançado
- [ ] Callbacks de eventos
- [ ] Python bindings

### Won't Have (Baixa - Future)

- [ ] Suporte Windows
- [ ] Interface gráfica
- [ ] Dashboard web integrado
- [ ] Machine learning de predição

---

## 4. Roadmap Visual

```
2026
├── Q3 (Set-Ago)
│   ├── v0.1 MVP (30 ago)
│   │   ├── Leitura básica ✓
│   │   └── CPU usage ✓
│   └── Planning v1.0
│
├── Q4 (Out-Dez)
│   ├── v1.0 Release (01 dez)
│   │   ├── Temperatura ✓
│   │   ├── Portabilidade ✓
│   │   ├── Documentação ✓
│   │   └── Production ready ✓
│   └── v1.1 Planning
│
2027
├── Q1 (Jan-Mar)
│   ├── v1.1 (mar 2027)
│   │   ├── Thread-safety
│   │   └── Melhorias
│   └── v1.2 Planning
│
└── Q2+ (Apr+)
    └── v2.0 Planning (novas plataformas)
```

---

## 5. Critérios de Sucesso por Fase

### Fase 1 (MVP)

- [x] Projeto compila
- [x] cpu_get_info() retorna dados corretos
- [x] cpu_get_usage() calcula porcentagem
- [x] Documentação básica
- [x] Cobertura >= 80%
- [x] Sem crashes

### Fase 2 (v1.0)

- [x] Todas features implementadas
- [x] Cobertura >= 85%
- [x] Zero memory leaks
- [x] Zero compilation warnings
- [x] Funciona em x86 e ARM
- [x] Documentação completa
- [x] Release notes publicadas

### Fase 3 (v1.1+)

- [x] < 5 issues abertos críticas
- [x] Tempo de resposta a issues < 1 semana
- [x] Features backlog priorizado
- [x] Compatibilidade mantida

---

## 6. Dependências Entre Tasks

```
1.1.1 (Setup)
├── 1.1.2 (Estrutura)
├── 1.1.3 (Leitura cpuinfo)
│   ├── 1.1.4 (Parser)
│   │   └── 1.1.5 (Struct)
│   │       └── 1.1.6 (Função)
│   └── 1.1.7 (Erros)
└── 1.2.1 (Leitura stat)
    ├── 1.2.2 (Cálculo)
    │   └── 1.2.3 (Função)
    ├── 1.2.4 (Validação)
    ├── 1.2.5 (Testes)
    └── 1.2.6 (Docs)
```

---

## 7. Recursos Alocados

### Equipe

- **Tech Lead**: 1 pessoa (Planning, architecture, code review)
- **Developers**: 2 pessoas (Dev 1: Core, Dev 2: Integration/Docs)
- **QA**: 1 pessoa (Testing, benchmarks)
- **DevOps/Infra**: 1 pessoa (CI/CD, releases)
- **Tech Writer**: 1 pessoa (Documentation)
- **Security**: 0.5 pessoa (Security audits)

**Total**: 6.5 FTE

### Orçamento

- Desenvolvimento: 200 horas
- QA/Testes: 60 horas
- Documentação: 40 horas
- DevOps/Infra: 30 horas
- **Total**: 330 horas (~8 semanas a 40h/sem)

---

## 8. Riscos e Mitigação

| Risco                              | Probabilidade | Impacto | Mitigação                    |
| ---------------------------------- | ------------- | ------- | ---------------------------- |
| Variações em /proc entre kernels   | Média         | Alto    | Suporte a múltiplas versões  |
| Performance em CPUs grandes (512+) | Baixa         | Médio   | Otimização e benchmarking    |
| Compatibilidade entre distros      | Baixa         | Médio   | Testes em múltiplas distros  |
| Segurança de /proc                 | Baixa         | Alto    | Security audit, validação    |
| Slip de schedule                   | Média         | Médio   | Buffer, priorização rigorosa |

---

## 9. KPIs de Progresso

| KPI                 | Target | v0.1 | v1.0 |
| ------------------- | ------ | ---- | ---- |
| Tempo em schedule   | 100%   | 95%  | 100% |
| Code coverage       | >= 80% | 82%  | 87%  |
| Bugs por 1k LOC     | <= 2   | 0.5  | 0    |
| Time to fix crítica | <= 24h | -    | 18h  |
| Issues abertos      | <= 5   | -    | 3    |
