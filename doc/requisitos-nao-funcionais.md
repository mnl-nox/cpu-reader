# Requisitos Não Funcionais (RNF) do CPU Reader

## RNF-001: Compatibilidade com C99

**Descrição:** Todo o código da biblioteca deve ser compatível com o padrão C99.

**Critérios de Aceitação:**

- Compilação sem erros com `gcc -std=c99 -Wall -Wextra -Wpedantic`
- Compilação sem erros com `clang -std=c99 -Wall -Wextra -Wpedantic`
- Sem uso de extensões C11, C17 ou propriedades de compilador específicas (exceto inline assembly controlado)

**Prioridade:** ALTA | **Status:** Implementado

---

## RNF-002: Compatibilidade com Linux

**Descrição:** A biblioteca deve funcionar em qualquer distribuição Linux com kernel moderno (3.10+).

**Critérios de Aceitação:**

- Funciona em Debian/Ubuntu (14.04+)
- Funciona em RHEL/CentOS (7.0+)
- Funciona em Alpine (3.10+)
- Disponibilidade de `/proc/cpuinfo` e `/proc/stat` é obrigatória
- Graciosidade em máquinas sem sensores de temperatura (retorna `-1.0f`)

**Prioridade:** ALTA | **Status:** Implementado

---

## RNF-003: Sem dependências externas no núcleo

**Descrição:** O núcleo da biblioteca (`libcpu.a`) deve depender apenas de libc. Ncurses é opcional, apenas para a aplicação de exemplo.

**Critérios de Aceitação:**

- Compilação sem dependências de ncurses, ncursesw ou bibliotecas gráficas
- Todas as funções de leitura usam apenas chamadas POSIX
- O monitor é um consumidor separado da API

**Prioridade:** ALTA | **Status:** Implementado

---

## RNF-004: Compatibilidade em múltiplas arquiteturas

**Descrição:** O código deve compilar e funcionar em x86_64, arm64, armv7, e outras arquiteturas.

**Critérios de Aceitação:**

- Fallback C funcionando para arquiteturas que não têm assembly inline
- Testes passando em pelo menos x86_64 e arm64
- Sem suposições sobre endianness ou tamanho de tipos

**Prioridade:** MÉDIA | **Status:** ️ Parcialmente implementado (x86_64 otimizado, fallback em outras)

---

## RNF-005: Performance

**Descrição:** Leituras devem ser rápidas o suficiente para uso em aplicações interativas.

**Critérios de Aceitação:**

- `cpu_get_info()` completa em < 10ms em máquinas típicas
- `cpu_get_usage()` completa em < 5ms
- Monitor atualiza com latência < 100ms
- Sem alocações desnecessárias por chamada

**Prioridade:** MÉDIA | **Status:** Implementado (sem benchmarks formais)

---

## RNF-006: Segurança de memória

**Descrição:** A biblioteca deve ser segura contra overflow de buffer, double-free e vazamentos.

**Critérios de Aceitação:**

- Sem erros detectáveis por AddressSanitizer
- Sem erros detectáveis por Valgrind
- Buffers de tamanho fixo (`model[256]`, `flags[512]`) com parsing defensivo
- Testes com `-fsanitize=address -fsanitize=undefined`

**Prioridade:** ALTA | **Status:** ️ Implementado, sem CI formalmente configurada

---

## RNF-007: Tratamento de erros consistente

**Descrição:** Todas as funções devem reportar falhas de forma consistente.

**Critérios de Aceitação:**

- Valores de retorno bem definidos: `NULL`, `-1.0f` ou `-1` conforme o tipo
- Sistema de erro global (`cpu_get_last_error_code()` e `cpu_get_last_error()`)
- Mensagens de erro descritivas em inglês

**Prioridade:** ALTA | **Status:** Implementado

---

## RNF-008: Gerenciamento de recursos

**Descrição:** A biblioteca não deve manter arquivos abertos, conexões ou estado persistente desnecessário.

**Critérios de Aceitação:**

- Cada chamada abre e fecha seus arquivos
- Nenhum arquivo `/proc` mantido aberto entre chamadas
- Contextos podem ser inicializados e limpos múltiplas vezes

**Prioridade:** ALTA | **Status:** Implementado

---

## RNF-009: Documentação e exemplos

**Descrição:** Código deve incluir exemplos funcionais e documentação clara.

**Critérios de Aceitação:**

- README com instruções de compilação e uso
- Exemplos em `examples/monitor.c`
- Testes ilustrando cada função em `tests/test_cpu.c`
- Comentários em funções públicas
- Documentação de design em `doc/`

**Prioridade:** MÉDIA | **Status:** Implementado

---

## RNF-010: Testabilidade

**Descrição:** O código deve ser fácil de testar com dados injetados.

**Critérios de Aceitação:**

- Variáveis de ambiente para redirecionar arquivos (CPU*READER*\*\_PATH)
- Teste básico executável com `make test`
- Cobertura de testes > 80% (aspiração)

**Prioridade:** MÉDIA | **Status:** Implementado (cobertura ainda a medir)

---

## RNF-011: Versionamento semântico

**Descrição:** Releases devem seguir versionamento semântico com automação por commits convencionais.

**Critérios de Aceitação:**

- Tags automáticas no formato `vMAJOR.MINOR.PATCH`
- Commits `feat` incrementam MINOR
- Commits `fix`, `bugfix`, `refactor` incrementam PATCH
- Primeiro release é `v0.1.0`

**Prioridade:** MÉDIA | **Status:** Implementado (primeiro release `v0.0.0` em breve)

---

## RNF-012: Maintainability

**Descrição:** O código deve ser fácil de manter e evoluir.

**Critérios de Aceitação:**

- Módulos com responsabilidades claras (cpu.c, cpu_info.c, cpu_usage.c)
- Funções internas bem documentadas em cpu_internal.h
- Sem copy-paste significativo
- Comentários em decisões não óbvias

**Prioridade:** MÉDIA | **Status:** Implementado

---

## Matriz de Prioridades RNF

| ID | Requisito | Prioridade | Status |
| ------- | ------------------------- | ---------- | ------ |
| RNF-001 | C99 | ALTA | |
| RNF-002 | Linux | ALTA | |
| RNF-003 | Sem deps externas | ALTA | |
| RNF-004 | Multi-arquitetura | MÉDIA | ️ |
| RNF-005 | Performance | MÉDIA | |
| RNF-006 | Segurança de memória | ALTA | ️ |
| RNF-007 | Tratamento de erros | ALTA | |
| RNF-008 | Gerenciamento de recursos | ALTA | |
| RNF-009 | Documentação | MÉDIA | |
| RNF-010 | Testabilidade | MÉDIA | |
| RNF-011 | Versionamento | MÉDIA | |
| RNF-012 | Maintainability | MÉDIA | |

---

## Próximas ações para RNF

1. **RNF-004 Melhorado:** Compilar e testar em arm64, armv7 em CI.
2. **RNF-005 Formalizado:** Adicionar benchmarks com `perf` e documentar latências.
3. **RNF-006 CI:** Integrar AddressSanitizer e Valgrind em CI/CD.
4. **RNF-010 Medida:** Usar `gcov` para medir cobertura formal.
