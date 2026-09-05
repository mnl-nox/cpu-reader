# Roadmap de Evolução - CPU Reader

**Período:** Semana de 2026-09-04 a 2026-09-11  
**Versão do Produto:** v0.2.0 (planejada)  
**Status:** EM PLANEJAMENTO

---

## 1. Visão Geral

Esta semana focará em **consolidação de robustez** e preparação para **integração contínua (CI/CD)**. Todos os requisitos funcionais (RF-001 a RF-012) estão implementados; as próximas ações preparam para production e melhoram a qualidade.

### Objetivos Principais

1. ✅ Documentação consolidada (CONCLUÍDO)
2. 🔄 Validação formal de requisitos
3. 🔄 Preparação de CI/CD
4. 🔄 Testes com sanitizers
5. 🔄 Suporte inicial a múltiplas plataformas

---

## 2. Plano Detalhado por Dia

### Dia 1 (Terça, 2026-09-04) - Documentação ✅ CONCLUÍDO

**Objetivo:** Consolidar toda a documentação do projeto.

**Tarefas:**

- [x] Criar `doc/requisitos-funcionais.md` (RF-001 a RF-012)
- [x] Criar `doc/requisitos-nao-funcionais.md` (RNF-001 a RNF-012)
- [x] Criar `doc/architecture-decision-records.md` (ADR-0001 a ADR-0013)
- [x] Criar `doc/design.md` (Design Document v2.0)
- [x] Criar `doc/software-requirements-specification.md` (SRS)
- [x] Criar `doc/uml-diagrams.md` (14 diagramas Mermaid)
- [x] Atualizar `doc/prioridades-sdlc.md` com roadmap P1-P4

**Entregáveis:**

- 7 novos documentos de arquitetura/requisitos
- 14 diagramas UML em Mermaid
- Roadmap de 8 semanas

**Status:** ✅ CONCLUÍDO

---

### Dia 2 (Quarta, 2026-09-05) - Validação e Testes

**Objetivo:** Validar implementação contra requisitos documentados.

**Tarefas:**

1. **Validação de RF (Requisitos Funcionais)**
   - [ ] Executar `make test` e validar 12 RF
   - [ ] Verificar critérios de aceitação de RF-001 a RF-012
   - [ ] Documentar resultado em `VALIDATION.md`
   - Tempo estimado: 2h

2. **Validação de RNF (Requisitos Não-Funcionais)**
   - [ ] Compilar com `gcc -std=c99 -Wall -Wextra -Wpedantic` (RNF-001)
   - [ ] Compilar com `clang -std=c99 -Wall -Wextra -Wpedantic` (RNF-001)
   - [ ] Testar em pelo menos 2 distribuições Linux (RNF-002)
   - [ ] Validar sem ncurses no núcleo (RNF-003)
   - Tempo estimado: 2h

3. **Executar Testes com Sanitizers**
   - [ ] Compilar com `-fsanitize=address -fsanitize=undefined`
   - [ ] Executar `./build/test_cpu`
   - [ ] Documento de sanitizer report
   - Tempo estimado: 1h

**Entregáveis:**

- `doc/VALIDATION.md` (relatório de validação)
- Relatório de sanitizers
- Checklist de RNF validados

---

### Dia 3 (Quinta, 2026-09-06) - CI/CD Setup Inicial

**Objetivo:** Preparar automação básica de testes.

**Tarefas:**

1. **GitHub Actions Workflow (`.github/workflows/`)**
   - [ ] Criar `test.yml` que executa:
     - Compilação com `make`
     - Testes com `make test`
     - Build com diferentes flags (`-fsanitize=*`)
   - [ ] Rodar em `ubuntu-latest` (x86_64)
   - Tempo estimado: 2h

2. **Script de Build Reprodutível**
   - [ ] Validar `Makefile` em múltiplas shells (bash, sh)
   - [ ] Documentar pré-requisitos em `BUILD.md`
   - [ ] Criar script `scripts/build.sh` para reprodução
   - Tempo estimado: 1h

3. **Lint e Verificação Estática**
   - [ ] Adicionar `clang-tidy` ao workflow (opcional, não-blocking)
   - [ ] Adicionar `cppcheck` (opcional, não-blocking)
   - [ ] Documento de como rodar localmente
   - Tempo estimado: 1h

**Entregáveis:**

- `.github/workflows/test.yml`
- `BUILD.md` (instruções de build)
- `scripts/build.sh`

---

### Dia 4 (Sexta, 2026-09-07) - Suporte Multi-Plataforma

**Objetivo:** Validar funcionamento em múltiplas arquiteturas.

**Tarefas:**

1. **Testes em x86_64**
   - [x] Já funcionando (máquina padrão)
   - [ ] Executar `make test` com assembly inline (RNF-005)
   - [ ] Validar performance (< 5ms `cpu_get_usage()`)
   - Tempo estimado: 0.5h

2. **Testes em ARM64 (se disponível)**
   - [ ] Usar QEMU ou máquina ARM
   - [ ] Compilar com `arm64-linux-gnu-gcc` (cross-compile)
   - [ ] Executar testes
   - [ ] Validar fallback C para soma de contadores (RNF-004)
   - Tempo estimado: 2h (ou skip se não disponível)

3. **Testes em distribuições diferentes**
   - [ ] Alpine Linux (musl libc)
   - [ ] Debian/Ubuntu (glibc)
   - [ ] RHEL/CentOS (glibc)
   - Usar docker se necessário
   - Tempo estimado: 2h

**Entregáveis:**

- Relatório de compatibilidade multi-plataforma
- Dockerfile para testes (opcional)
- `doc/COMPATIBILITY.md`

---

### Semana (Segunda, 2026-09-09) - Benchmarks e Documentação Final

**Objetivo:** Medir performance e preparar release v0.2.0.

**Tarefas:**

1. **Benchmarks de Performance (RNF-005)**
   - [ ] Medir `cpu_get_info()` com `time` / `perf`
   - [ ] Medir `cpu_get_usage()` (1ª e 2ª chamada)
   - [ ] Medir consumo de memória com `valgrind --tool=massif`
   - [ ] Documentar em `PERFORMANCE.md`
   - Tempo estimado: 1.5h

2. **Cobertura de Testes (RNF-010)**
   - [ ] Compilar com `--coverage` / gcov
   - [ ] Executar `make test`
   - [ ] Gerar relatório: `gcov src/*.c`
   - [ ] Aspiração: > 80% cobertura
   - Tempo estimado: 1.5h

3. **Preparação de Release**
   - [ ] Criar `CHANGELOG.md` (resumo de v0.2.0)
   - [ ] Atualizar `README.md` com novas seções (Documentação)
   - [ ] Tag `v0.2.0` (ou deixar automation GitHub Actions)
   - [ ] Commit final: `docs: consolidate documentation`
   - Tempo estimado: 1h

4. **Documentação de Contribuição**
   - [ ] Criar `CONTRIBUTING.md` (guia para contribuidores)
   - [ ] Documentar workflow de ADR
   - [ ] Guia de teste local
   - Tempo estimado: 1h

**Entregáveis:**

- `doc/PERFORMANCE.md` (benchmarks)
- `doc/COVERAGE.md` (cobertura de testes)
- `CHANGELOG.md`
- `CONTRIBUTING.md`
- Tag `v0.2.0`

---

## 3. Matriz de Requisitos e Ações

| Requisito               | Ação                        | Dia | Prioridade | Status  |
| ----------------------- | --------------------------- | --- | ---------- | ------- |
| RNF-001 (C99)           | Compilar com flags -std=c99 | 2   | ALTA       | Qua     |
| RNF-002 (Linux)         | Testar em 3+ distribuições  | 4-5 | ALTA       | Sex-Seg |
| RNF-003 (Sem deps)      | Validar sem ncurses         | 2   | ALTA       | Qua     |
| RNF-005 (Performance)   | Benchmarks formais          | 5   | MÉDIA      | Seg     |
| RNF-006 (Segurança mem) | AddressSanitizer + Valgrind | 2-3 | ALTA       | Qua-Qui |
| RNF-010 (Testabilidade) | Cobertura com gcov          | 5   | MÉDIA      | Seg     |
| CI/CD                   | GitHub Actions workflow     | 3   | ALTA       | Qui     |

---

## 4. Backlog de Curto Prazo (Próximas 2 Semanas)

### Semana 2 (2026-09-11 a 2026-09-18) - P1: Robustez

**Objetivo:** Consolidar qualidade e automação.

**Tarefas:**

1. [ ] Integrar CI/CD em `main` (GitHub Actions)
2. [ ] Alcançar > 80% cobertura de testes
3. [ ] Passar AddressSanitizer / Valgrind sem erros
4. [ ] Documentar e testar em 3+ plataformas
5. [ ] Release v0.2.0 com tags automáticas
6. [ ] Publicar no GitHub Releases

**Requisitos Relacionados:** RNF-005, RNF-006, RNF-010

---

## 5. Backlog Médio Prazo (Próximas 4 Semanas)

### Semana 3-4 (2026-09-18 a 2026-10-02) - P2: Recursos de CPU

**Objetivo:** Adicionar métricas por núcleo.

**Tarefas:**

1. [ ] RF-013: Implementar `cpu_get_core_info()` e `cpu_get_core_usage()`
2. [ ] Ler `/proc/stat` por núcleo (linhas "cpu0", "cpu1", etc.)
3. [ ] Estrutura `cpu_core_stats_t` com informações por núcleo
4. [ ] Testes com fixtures
5. [ ] Atualizar ADRs e RFs
6. [ ] Release v0.3.0

**Requisitos Relacionados:** RF-013, RNF-004

---

## 6. Backlog Longo Prazo (Próximas 8 Semanas)

### Semana 5-6 (2026-10-02 a 2026-10-16) - P3: Performance

**Objetivo:** Otimizar e benchmarquear.

**Tarefas:**

1. [ ] Implementar cache configurável (RF-014)
2. [ ] Adicionar intrinsics SIMD (AVX2) para parsing
3. [ ] Benchmark detalhado (perfil de CPU)
4. [ ] Otimização de latência (< 2ms `cpu_get_usage()`)

### Semana 7-8 (2026-10-16 a 2026-10-30) - P4: Integração Contínua

**Objetivo:** Automação completa de release.

**Tarefas:**

1. [ ] GitHub Actions multi-plataforma (x86_64, ARM64, ARM32)
2. [ ] Binários pre-compilados em Releases
3. [ ] Publicar em package managers (apt, brew, conan, vcpkg)
4. [ ] Documentação de API (Doxygen)
5. [ ] Website de documentação

---

## 7. Métricas de Sucesso

### Semana 1 (Atual: 2026-09-04)

- [x] 100% da documentação consolidada
- [x] 7 novos documentos criados
- [x] 14 diagramas UML desenhados
- [x] Roadmap de 8 semanas definido

### Semana 2 (2026-09-11)

- [ ] 100% dos testes passando
- [ ] 0 erros em sanitizers
- [ ] CI/CD pipeline funcionando
- [ ] Multi-plataforma testada
- [ ] v0.2.0 releasado

### Semana 3+ (futuro)

- [ ] RF-013 implementada
- [ ] Cobertura > 80%
- [ ] Performance < 2ms (RF-005)
- [ ] GitHub Releases com binários
- [ ] Documentação de API completa

---

## 8. Recursos Necessários

### Ferramentas

- **Compiladores:** gcc, clang
- **Sanitizers:** AddressSanitizer, UBSan, Valgrind
- **Coverage:** gcov
- **Linters:** clang-tidy, cppcheck
- **CI:** GitHub Actions (gratuito)
- **Containerização:** Docker (opcional, para testes multi-plataforma)

### Acesso

- [ ] Repositório GitHub (já disponível)
- [ ] Permissões de push para `main` (para tags)
- [ ] Acesso a máquinas para testes (x86_64, arm64, armv7)

### Estimativa de Esforço

| Fase                 | Semanas       | Pessoa-horas  | Prioridade |
| -------------------- | ------------- | ------------- | ---------- |
| Documentação (Dia 1) | 1             | 8             | ALTA       |
| Validação + Testes   | 1             | 6             | ALTA       |
| CI/CD Setup          | 1-2           | 6             | ALTA       |
| Multi-plataforma     | 1-2           | 6             | ALTA       |
| Benchmarks           | 1             | 4             | MÉDIA      |
| RF-013 (cores)       | 2             | 12            | MÉDIA      |
| Release + Package    | 2             | 8             | MÉDIA      |
| **Total**            | **8 semanas** | **~50 horas** |            |

---

## 9. Riscos e Mitigações

| Risco                          | Probabilidade | Impacto | Mitigação                        |
| ------------------------------ | ------------- | ------- | -------------------------------- |
| CI/CD demora mais que esperado | MÉDIA         | MÉDIO   | Começar cedo, usar templates     |
| Incompatibilidade ARM64        | BAIXA         | MÉDIO   | Testar com QEMU, cross-compile   |
| Performance não atinge meta    | BAIXA         | MÉDIO   | Profiling antecipado com `perf`  |
| Cobertura abaixo de 80%        | MÉDIA         | MÉDIO   | Adicionar testes para edge cases |
| Release automation falha       | BAIXA         | ALTO    | Teste dry-run de release         |

---

## 10. Definições de Pronto (Definition of Done)

### Por Tarefa

- [ ] Código compilado sem warnings
- [ ] Testes passando (unit + integration)
- [ ] Documentação atualizada
- [ ] ADRs criadas/atualizadas se há mudanças arquiteturais
- [ ] Revisão de código realizada
- [ ] Merge em `main`

### Por Release

- [ ] Todos os RF funcionando
- [ ] Todos os RNF validados
- [ ] Testes > 80% cobertura
- [ ] Documentação completa
- [ ] Tag semântica criada (`vX.Y.Z`)
- [ ] CHANGELOG atualizado
- [ ] Release notes publicadas

---

## 11. Comunicação e Acompanhamento

### Daily Standup

- **Frequência:** Diária (on-demand)
- **Duração:** 15 min
- **Pontos:** O que foi feito, o que será feito, bloqueadores

### Revisão de Sprint

- **Data:** Sexta, 2026-09-06 (final da semana 1)
- **Agenda:** Status de documentação e plano para semana 2

### Retrospectiva

- **Data:** Sexta, 2026-09-13
- **Agenda:** O que funcionou, o que melhorar, ações

---

## 12. Plano de Contingência

### Se documentação tomar mais tempo

- Priorizar RNF, RF, ADR (core)
- Adiar UML/Design para semana 2

### Se CI/CD bloqueador

- Continuar com testes locais
- Setup CI/CD paralelo com alguém else

### Se testes falham em novo platform

- Criar issue de compatibilidade
- Adiar suporte até semana 3
- Documentar workaround

---

## 13. Próximos Passos Imediatos (Hoje, 2026-09-04)

1. ✅ Documentação completada
2. 🔄 **PRÓXIMO:** Revisar este documento com a equipe
3. 🔄 **PRÓXIMO:** Executar validação de requisitos (Dia 2)
4. 🔄 **PRÓXIMO:** Começar setup CI/CD (Dia 3)

---

## 14. Referências

- [doc/requisitos-funcionais.md](requisitos-funcionais.md)
- [doc/requisitos-nao-funcionais.md](requisitos-nao-funcionais.md)
- [doc/architecture-decision-records.md](architecture-decision-records.md)
- [doc/design.md](design.md)
- [doc/software-requirements-specification.md](software-requirements-specification.md)
- [README.md](../README.md)
- [doc/prioridades-sdlc.md](prioridades-sdlc.md)

---

**Data de Criação:** 2026-09-04  
**Última Atualização:** 2026-09-04  
**Status:** APROVADO PARA EXECUÇÃO  
**Próxima Revisão:** 2026-09-06 (Final da Semana 1)
