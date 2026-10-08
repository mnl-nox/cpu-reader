# Arquitetura Decision Records (ADR) - CPU Reader

Este documento estende as decisões de design com ADRs formalmente estruturadas e futuras decisões.

## ADR-0001: Uso de C99

**Status:** ACEITO | **Data:** 2026 | **Modificado:** Não

**Contexto:**
O projeto precisa ser uma biblioteca pequena, portável e com mínimas dependências. Linguagens e versões do padrão influenciam portabilidade, complexidade de build e audiência.

**Decisão:**
Usar C99 como padrão de linguagem obrigatório.

**Justificativa:**

- C99 é suportado por todos os compiladores modernos (GCC, Clang, MSVC)
- Evita recursos C11+ que reduzem portabilidade
- Mantém compatibilidade com sistemas legados
- Simples compilação com `gcc -std=c99`

**Consequências:**

- Máxima portabilidade
- Mínimo overhead de compilação
- Sem alguns recursos modernos (threads nativas, atomic ops)
- Thread-safety manual

**Referências:**

- ISO/IEC 9899:1999
- [Rationale for International Standard Programming Language C](http://www.open-std.org/jtc1/sc22/wg14/www/C99RationaleV5.10.pdf)

---

## ADR-0002: API Baseada em Estrutura para Informações Estáticas

**Status:** ACEITO | **Data:** 2026 | **Modificado:** Não

**Contexto:**
A API deve expor múltiplas informações da CPU (modelo, frequência, cores, flags, etc.). As opções são: múltiplas funções individuais, ponteiro para estrutura alocada, ou estrutura em stack.

**Decisão:**
Retornar `cpu_info_t *` alocada com `calloc()` que o chamador libera com `cpu_free_info()`.

**Justificativa:**

- Uma chamada única para obter todas as informações
- Aplicação controla ciclo de vida
- Funciona bem com linguagens que envolvem C (FFI)
- Requer gerenciamento manual de memória

**Alternativas Consideradas:**

1. Funções individuais (`cpu_get_model()`, `cpu_get_frequency()`, etc.)
 - Múltiplas chamadas, mais overhead
2. Estrutura em stack retornada por valor
 - Não funciona em C para estruturas grandes
 - Difícil de estender sem quebrar ABI

**Consequências:**

- Aplicação deve chamar `cpu_free_info()` para evitar vazamento
- Necessário `calloc()` bem-sucedido antes de usar estrutura
- Estrutura é imutável (read-only para aplicação)

**Referências:**

- D-004: Alocação explícita

---

## ADR-0003: Interface `/proc` para Dados de CPU

**Status:** ACEITO | **Data:** 2026 | **Modificado:** Não

**Contexto:**
Para obter dados de CPU em Linux, as opções incluem: `/proc/cpuinfo`, `/sys/`, APIs específicas de kernel, bibliotecas de terceiros (libcpuid, hwinfo), ou chamadas de sistema customizadas.

**Decisão:**
Usar `/proc/cpuinfo` para informações estáticas e `/proc/stat` para contadores, com fallback para `sysfs` quando necessário.

**Justificativa:**

- Interface padrão POSIX-like disponível em todos os Linux
- Sem dependências externas
- Simples parsing de texto
- Nenhuma permissão elevada necessária
- Formato pode variar por arquitetura e versão de kernel

**Alternativas Consideradas:**

1. `/sys/devices/system/cpu/` (sysfs moderno)
 - Mais estruturado
 - Menos portável em kernels antigos
2. `libcpuid` ou equivalente
 - Mais funcionalidades
 - Dependência externa
3. Chamadas de sistema (p.ex., `cpuid` em x86)
 - Acesso direto ao hardware
 - Específico de arquitetura

**Consequências:**

- Dependência do kernel Linux e versão `/proc`
- Parsing defensivo necessário para lidar com variações
- Impossível funcionar em non-Linux sem ajustes maiores
- Dados refletem estado no momento da leitura

**Referências:**

- D-003: Interface `/proc`
- [Linux /proc Filesystem](https://man7.org/linux/man-pages/man5/proc.5.html)

---

## ADR-0004: Alocação Explícita vs Implícita

**Status:** ACEITO | **Data:** 2026 | **Modificado:** Não

**Contexto:**
Para retornar estruturas alocadas dinamicamente em C, a biblioteca pode: alocar e o chamador libera, ou alocar e fornecer função para liberar internamente.

**Decisão:**
`cpu_get_info()` aloca com `calloc()` e o chamador libera com `cpu_free_info()`.

**Justificativa:**

- Flexibilidade: aplicação decide quando liberar
- Compatível com linguagens dinamicamente tipadas (Python, Node.js)
- Permite reutilizar mesma estrutura se necessário
- Responsabilidade compartilhada de memória
- Risco de vazamento se aplicação esquecer de liberar

**Alternativas Consideradas:**

1. Retornar estrutura em stack (por valor)
 - Não funciona para estruturas complexas/grandes
2. Passar buffer pré-alocado como argumento
 - Sem ambiguidade de ownership
 - API menos intuitiva
3. Alocador customizável (malloc wrapper)
 - Flexibilidade de alocação
 - Complexidade adicional

**Consequências:**

- Testes devem verificar vazamentos com Valgrind/AddressSanitizer
- Documentação deve destacar necessidade de `cpu_free_info()`
- Aplicações desatentas podem ter memory leaks
- Permite padrões de pool de objetos se necessário

**Referências:**

- D-004: Alocação explícita

---

## ADR-0005: Cálculo de Uso por Deltas (Não Snapshot Imediato)

**Status:** ACEITO | **Data:** 2026 | **Modificado:** Não

**Contexto:**
Para calcular uso de CPU (porcentagem), é necessário comparar dois pontos no tempo. A API pode: exigir duas leituras manuais do chamador, ou manter estado interno.

**Decisão:**
`cpu_get_usage()` mantém estado interno (contexto padrão) com leituras anteriores; a primeira chamada estabelece referência (retorna 0.0f), chamadas subsequentes retornam delta.

**Justificativa:**

- API simples: uma única função para obter uso
- Não requer gerenciamento manual de tempo/leituras
- Intuitivo para usuários inexperientes
- Estado compartilhado não thread-safe
- Primeira leitura sempre retorna 0.0f

**Alternativas Consideradas:**

1. Exigir duas leituras manuais
 - Sem estado compartilhado
 - API menos conveniente
2. `cpu_get_usage_snapshot()` + `cpu_get_usage_delta()`
 - Melhor controle
 - Mais complexo
3. Timestamp automático com cache
 - Evita primeira leitura 0.0f
 - Espera oculta, comportamento surpresa

**Consequências:**

- `cpu_init()` e `cpu_cleanup()` necessários para reset
- Múltiplas threads compartilhando contexto padrão terão race conditions
- `cpu_usage_context_t` permite contadores independentes por thread
- Documentação deve advertir sobre uso concorrente

**Referências:**

- D-005: Uso por deltas
- RF-005: Cálculo de uso agregado
- RF-006: Contextos independentes

---

## ADR-0006: Monitor Separado (Sem Dependência de Ncurses no Núcleo)

**Status:** ACEITO | **Data:** 2026 | **Modificado:** Não

**Contexto:**
A biblioteca pode incluir interface gráfica/terminal nativamente ou deixá-la separada. Isso afeta tamanho do núcleo, dependências e flexibilidade.

**Decisão:**
Núcleo (`libcpu.a`) sem ncurses; monitor é aplicação separada em `examples/monitor.c` que linkava-se com `-lncurses`.

**Justificativa:**

- Núcleo sem dependências opcionais
- Aplicações podem usar a biblioteca sem ncurses
- Fácil substituir monitor por interface Web, JSON, etc.
- Teste de núcleo sem ncurses instalado
- Requer app separada para função de exemplo

**Alternativas Consideradas:**

1. Incluir ncurses no núcleo
 - Força dependência em todas as aplicações
2. Plugin system para UIs
 - Muito flexível
 - Overhead adicional
3. Apenas library, sem exemplo
 - Mínimo
 - Menos usável para iniciantes

**Consequências:**

- Compilação deve condicionalizar `examples/` para ncurses
- Núcleo permanece compilável em ambientes sem ncurses
- Contribuidores podem adicionar novas UIs facilmente
- Teste não depende de ncurses estar disponível

**Referências:**

- D-006: Monitor separado

---

## ADR-0007: Implementação C portátil para contadores

**Status:** SUBSTITUÍDO | **Data:** 2026-10-08

**Contexto:** A documentação histórica afirmava que a soma dos contadores de
`/proc/stat` usava assembly inline em x86_64. A implementação atual não contém
um caminho assembly: a soma é C e o macro `CPU_READER_DISABLE_ASM` não seleciona
uma implementação diferente.

**Decisão:** Manter a implementação C como única implementação até que um
benchmark reproduzível demonstre uma melhoria material e sustentável que
justifique código específico de arquitetura. Não anunciar otimização assembly
nem usar o modo `test-portable` como evidência de dois caminhos distintos.

**Consequências:** `make test-portable` permanece temporariamente como alias
compatível para a suíte C portátil. `make benchmark` fornece medições locais
informativas, sem limiar rígido de CI. Uma otimização futura deve ter
implementação comparável, teste de equivalência e resultados documentados.

---

## ADR-0008: Diagnóstico de erro legado e snapshots explícitos

**Status:** ACEITO COM COMPATIBILIDADE LEGADA | **Data:** 2026-10-08

**Contexto:** A API original `cpu_get_last_error_code()` /
`cpu_get_last_error()` expõe apenas o "último erro". Estado global permitia
diagnósticos cruzados entre threads e a mensagem podia ser apagada por uma
operação posterior.

**Decisão:** Manter a API antiga com armazenamento local à thread em GCC/Clang.
Adicionar `cpu_error_info_t`, `cpu_error_info_clear()` e variantes `*_ex` para
as operações fallíveis: cada variante copia código e mensagem para um objeto
pertencente ao chamador. O snapshot permanece estável até que o próprio
chamador o limpe ou sobrescreva.

**Limites:** O armazenamento interno da API legada continua local à thread; o
snapshot explícito evita que o consumidor dependa dele depois do retorno.
Contextos de uso explícitos continuam sendo propriedade do consumidor e não são
seguros para acesso concorrente sem sincronização. Variáveis de ambiente de
override devem ser configuradas antes de iniciar threads.

**Consequências:** Novas APIs fallíveis devem preferir uma saída de erro explícita.
A família antiga permanece por compatibilidade, mas deve ser considerada legada.
O contrato de ownership e o ciclo de vida do snapshot são documentados em
`include/cpu.h`.

**Validação:** testes verificam isolamento entre threads e que um snapshot de
erro continua válido depois de uma chamada bem-sucedida posterior.

---

## ADR-0009: Versionamento Semântico com Commits Convencionais

**Status:** ACEITO | **Data:** 2026 | **Modificado:** Não

**Contexto:**
O projeto precisa de versioning previsível e automatizado. As opções incluem: manual (CHANGELOG + arquivo VERSION), conventional commits com automação, ou tags semânticas manuais.

**Decisão:**
Usar Conventional Commits com automação de tags: `feat` = MINOR, `fix`/`refactor`/`bugfix` = PATCH, outros = nenhuma tag.

**Justificativa:**

- Automação remove erro manual
- Histórico de commits auto-documentado
- Compatível com ferramentas (semantic-release, etc.)
- Versão inicial `v0.1.0` (não v1.0.0, reflexo de estado BETA)
- Requer disciplina nos commits
- Não pode retroativamente ajustar versões
- Sem controle fino sobre pré-releases

**Alternativas:**

1. CHANGELOG manual + arquivo VERSION
 - Total controle
 - Errorprone, não escalável
2. Tags manuais por release
 - Simples
 - Sem automação
3. Semantic Versioning Com GitHub Releases
 - Mais formal
 - Requer passos adicionais

**Consequências:**

- Cada push para `main` pode criar tag automática
- Padrão de commit é obrigatório para releases
- Documentação deve reforçar convenção
- CI/CD scriptável para release automático
- Changelog pode ser gerado do histórico de commits

**Referências:**

- [Conventional Commits](https://www.conventionalcommits.org/)
- [Semantic Versioning](https://semver.org/)
- D-010: Versionamento por commits convencionais

---

## ADR-0010: Variáveis de Ambiente Injetáveis para Teste

**Status:** ACEITO | **Data:** 2026 | **Modificado:** Não

**Contexto:**
Para testar comportamento em diferentes máquinas/configurações sem risco, a biblioteca pode hardcode caminhos ou permitir override via variáveis de ambiente.

**Decisão:**
Permitir override de caminhos via `CPU_READER_*_PATH`:

- `CPU_READER_CPUINFO_PATH`
- `CPU_READER_PROC_STAT_PATH`
- `CPU_READER_CPU_TEMP_PATH`
- `CPU_READER_CPU_FREQ_PATH`
- `CPU_READER_LOADAVG_PATH`

**Justificativa:**

- Teste sem recompilar
- Usa fixtures de arquivo para casos edge
- Sem impacto de performance (checagem uma vez)
- Nenhuma dependência adicional
- Segurança: nenhuma validação de caminho (apenas teste/dev)

**Consequências:**

- Documentação deve indicar uso apenas para teste
- Código de produção não deve definir essas variáveis
- Permite criar suite de teste com múltiplas fixtures
- Performance: fopen() chamado por função, aceitável

**Referências:**

- RF-011: Caminhos injetáveis
- Fixtures de teste em `tests/`

---

## ADR-0011: Requisitos Não-Funcionais Formalizados (Novo)

**Status:** PROPOSTO | **Data:** 2026-09-04

**Contexto:**
O projeto até agora documentou RNFs informalmente. Uma lista formalizada melhora clareza, priorização e tomada de decisão arquitetural.

**Decisão:**
Criar documento `doc/requisitos-nao-funcionais.md` com 12 RNFs categorizados: compatibilidade, performance, segurança, documentação, testabilidade.

**Justificativa:**

- Critérios de aceitação formais para cada RNF
- Priorização clara (ALTA, MÉDIA, BAIXA)
- Rastreamento de status ( / ️ / )
- Base para decisões arquiteturais futuras

**Consequências:**

- RNFs podem ser discutidos e revisados por contribuidores
- Release checklist pode usar RNFs como validação
- Futuras ADRs podem referenciarem RNFs
- Maior clareza sobre commitments do projeto

**Status Atual:**

- RNF-001 a RNF-012 definidas
- Próximas ações: CI/CD formal, benchmarks, multi-arquitetura

**Referências:**

- [doc/requisitos-nao-funcionais.md](requisitos-nao-funcionais.md)

---

## ADR-0012: Requisitos Funcionais Consolidados (Novo)

**Status:** PROPOSTO | **Data:** 2026-09-04

**Contexto:**
Requisitos funcionais estavam fragmentados em múltiplos documentos. Uma consolidação melhora discoverability e validação.

**Decisão:**
Criar documento `doc/requisitos-funcionais.md` com 12 RFs implementados (RF-001 a RF-012) e 3 futuras (RF-013 a RF-015).

**Justificativa:**

- Um lugar único para entender todas as funcionalidades
- Exemplos de código em cada RF
- Relacionamento com testes e exemplos
- Roadmap claro das próximas RF

**Consequências:**

- Contribuidores sabem que adicionar RF requer atualizar este doc
- Testes devem referenciar RF correspondente
- Release notes podem ser geradas deste doc

**Status Atual:**

- RF-001 a RF-012 documentadas
- RF-013 (cores), RF-014 (cache), RF-015 (thread-safe) planejadas

**Referências:**

- [doc/requisitos-funcionais.md](requisitos-funcionais.md)

---

## ADR-0013: Design Document Estruturado (Proposto)

**Status:** PROPOSTO | **Data:** 2026-09-04

**Contexto:**
Decisões e explicações de design estão em múltiplos arquivos. Um único documento estruturado melhora onboarding e comunicação.

**Decisão:**
Criar/expandir `doc/design.md` com seções: Visão, Componentes, Padrões, Trade-offs, Roadmap.

**Benefícios:**

- Arquitetura clara para novos contribuidores
- Justificativa das escolhas por trás de cada decisão
- Espaço para discussão de alternativas
- Roadmap visual de evolução

**Status:** Em preparação

---

## Matriz de ADRs

| ID | Título | Status | Data | Prioridade |
| -------- | ------------------ | -------- | ---------- | ---------- |
| ADR-0001 | C99 | ACEITO | 2026-01 | ALTA |
| ADR-0002 | API Estrutura | ACEITO | 2026-01 | ALTA |
| ADR-0003 | Interface /proc | ACEITO | 2026-01 | ALTA |
| ADR-0004 | Alocação Explícita | ACEITO | 2026-01 | ALTA |
| ADR-0005 | Deltas de Uso | ACEITO | 2026-01 | ALTA |
| ADR-0006 | Monitor Separado | ACEITO | 2026-01 | MÉDIA |
| ADR-0007 | C portátil para contadores | SUBSTITUÍDO | 2026-10-08 | BAIXA |
| ADR-0008 | Diagnóstico legado por thread | PARCIAL/TRANSITÓRIO | 2026-10-08 | MÉDIA |
| ADR-0009 | Versionamento | ACEITO | 2026-01 | MÉDIA |
| ADR-0010 | Injeção Teste | ACEITO | 2026-01 | BAIXA |
| ADR-0011 | RNFs Formalizados | PROPOSTO | 2026-09-04 | MÉDIA |
| ADR-0012 | RFs Consolidadas | PROPOSTO | 2026-09-04 | MÉDIA |
| ADR-0013 | Design Estruturado | PROPOSTO | 2026-09-04 | BAIXA |
| ADR-0014 | Separação de módulos | ACEITO | 2026-10-08 | MÉDIA |
| ADR-0015 | Revisão final da API de erros explícitos | PROPOSTO | 2026-10-08 | MÉDIA |

---

## ADR-0014: Separação dos módulos de domínio e telemetria

**Status:** ACEITO | **Data:** 2026-10-08

**Contexto:** `src/cpu_info.c` acumulava parsing de topologia, temperatura,
frequência e `loadavg`, aumentando o acoplamento entre domínios.

**Decisão:** Separar parsing de CPU/topologia em `src/cpu_info.c`, telemetria de
`sysfs` e `/proc/loadavg` em `src/cpu_telemetry.c`, cálculo de uso em
`src/cpu_usage.c` e wrappers/contratos comuns em `src/cpu.c`. O contrato
interno permanece em `src/cpu_internal.h`; consumidores dependem apenas de
`include/cpu.h`.

**Consequências:** Cada módulo tem responsabilidade mais clara, fixtures podem
testar cada fonte de dados e a biblioteca principal continua sem ncurses. Não
foi adicionada uma camada de plugins sem necessidade demonstrada.

**Validação:** GCC/Clang, sanitizers, hardening e runtime ARM64 no CI; benchmark
local informativo sem limiar rígido.

---

## Próximas ADRs Esperadas (Backlog)

- **ADR-0015:** Revisão final da API de erros explícitos e compatibilidade ABI antes da versão 1.0
- **ADR-0016:** Caching de resultados
- **ADR-0017:** Suporte a múltiplas distribuições
- **ADR-0018:** API bindings (Python, Node.js)
- **ADR-0019:** Logging estruturado
- **ADR-0020:** Otimizações SIMD
- **ADR-0021:** Integração contínua e release automation

---

## Processo para Novas ADRs

1. **Identificar problema:** Decisão arquitetural ou design significativa
2. **Criar proposta:** ADR-XXXX com contexto, decisão, justificativa
3. **Discutir:** GitHub Issues/PR com stakeholders
4. **Aceitar/Rejeitar:** Merge quando consenso for atingido
5. **Documentar:** Adicionar a este arquivo
6. **Reverter se necessário:** Marcar como REVERTIDO com nova ADR (ex: ADR-0008-v2)
