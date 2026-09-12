# Software Requirements Specification (SRS) - CPU Reader

**Versão:** 0.1.1
**Data:** 2026-09-04 
**Status:** APROVADO 
**Autor:** Equipe de Desenvolvimento

---

## 1. Introdução

### 1.1 Objetivo do Documento

Este documento especifica de forma completa e formal os requisitos de software para a biblioteca CPU Reader. Serve como contrato entre stakeholders, desenvolvedores e QA.

### 1.2 Escopo do Produto

**Definição:** Biblioteca C99 para monitoramento de CPU em Linux.

**Incluso:**

- Leitura de informações estáticas (cores, modelo, flags)
- Cálculo de uso agregado (%)
- Leitura de temperatura
- Leitura de velocidade de clock
- Contagem de processos ativos
- Tratamento robusto de erros
- Monitor de exemplo com ncurses

**Não Incluso:**

- Métricas por núcleo (futuro)
- Web API
- Suporte a Windows/macOS
- Sincronização automática
- Logging estruturado

---

## 2. Referências Normativas

- ISO/IEC 9899:1999 (C99 Standard)
- IEEE 802.1Q (não aplicável, mencionado para referência)
- [Conventional Commits](https://www.conventionalcommits.org/)
- [Semantic Versioning](https://semver.org/)
- Linux kernel documentation: `/proc` filesystem

---

## 3. Definições, Acrônimos e Abreviaturas

| Termo | Definição |
| --------------- | ---------------------------------------------------------- |
| **CPU** | Central Processing Unit / Unidade Central de Processamento |
| **MHz** | Megahertz (frequência) |
| **sysfs** | Pseudo-filesystem `/sys` do Linux |
| **thread-safe** | Seguro para acesso concurrent de múltiplas threads |
| **ABI** | Application Binary Interface |
| **API** | Application Programming Interface |
| **RF** | Requisito Funcional |
| **RNF** | Requisito Não-Funcional |
| **ADR** | Architecture Decision Record |

---

## 4. Visão Geral do Produto

### 4.1 Perspectiva do Produto

A biblioteca é um componente de baixo nível para aplicações que precisam monitorar CPU. Dependências:

```
Aplicações do Usuário
 ↓
libcpu.a (biblioteca CPU Reader)
 ↓
Linux kernel (/proc, /sys)
```

### 4.2 Funções Principais

| Função | Descrição |
| ---------------- | --------------------------------------------- |
| Inicializar | `cpu_init()` prepara estado global |
| Ler Informações | `cpu_get_info()` obtém cores, modelo, flags |
| Calcular Uso | `cpu_get_usage()` calcula % de uso |
| Ler Temperatura | `cpu_get_temperature()` obtém temp °C |
| Ler Clock | `cpu_get_clock_speed()` obtém freq MHz |
| Contar Processos | `cpu_get_active_processes()` retorna contagem |
| Tratar Erros | `cpu_get_last_error_code()` e mensagem |
| Limpar | `cpu_cleanup()` libera recursos |

### 4.3 Características do Usuário

- **Desenvolvedores C/C++:** Integram biblioteca em projetos
- **DevOps:** Usam monitor para observabilidade
- **Pesquisadores:** Usam dados para benchmarks
- **Bindings (Python/Node.js):** Expõem API para script

### 4.4 Restrições Operacionais

- **SO:** Linux kernel 3.10+
- **Compiladores:** GCC 4.8+, Clang 3.5+
- **Padrão:** C99
- **Dependências:** nenhuma (exceto ncurses para monitor)
- **Threads:** não thread-safe nesta versão

---

## 5. Requisitos Específicos

### 5.1 Requisitos Funcionais por Prioridade

#### **ALTA PRIORIDADE**

**RF-001: Inicialização**

- `cpu_init()` deve preparar contexto padrão
- Retorna 0 em sucesso, não-0 em erro
- Pode ser chamada múltiplas vezes
- Status: IMPLEMENTADO

**RF-002: Limpeza**

- `cpu_cleanup()` libera recursos internos
- Zera contexto padrão
- Pode ser chamada múltiplas vezes
- Status: IMPLEMENTADO

**RF-003: Ler Informações Estáticas**

- `cpu_get_info()` retorna `cpu_info_t*` com:
 - `cores`: Processadores lógicos
 - `threads`: Threads online
 - `model`: Nome da CPU (até 255 chars)
 - `frequency_mhz`: Frequência (MHz)
 - `flags`: Flags de CPU (até 511 chars)
 - `active_processes`: Processos ativos (opcional)
 - `temperature_c`: Temperatura (opcional)
- Retorna NULL em falha
- Status: IMPLEMENTADO

**RF-004: Liberação de Memória**

- `cpu_free_info()` libera estrutura
- Aceita NULL sem erro
- Status: IMPLEMENTADO

**RF-005: Calcular Uso Agregado**

- `cpu_get_usage()` retorna 0-100 (%) ou -1.0f em erro
- Primeira chamada: 0.0f (referência)
- Chamadas subsequentes: % real
- Status: IMPLEMENTADO

**RF-010: Tratamento de Erros**

- `cpu_get_last_error_code()` retorna `cpu_error_t`
- `cpu_get_last_error()` retorna mensagem (até 255 chars)
- Zerado em sucesso
- Status: IMPLEMENTADO

#### **MÉDIA PRIORIDADE**

**RF-006: Contextos de Uso Independentes**

- `cpu_usage_context_t` para múltiplas instâncias
- `cpu_usage_context_init()`, `_cleanup()`, `_get_usage()`
- Cada contexto mantém state independente
- Status: IMPLEMENTADO

**RF-007: Temperatura**

- `cpu_get_temperature()` em Celsius
- Lê sysfs, retorna -1.0f se indisponível
- Status: IMPLEMENTADO

**RF-008: Velocidade de Clock**

- `cpu_get_clock_speed()` em MHz
- Fallback sysfs → /proc/cpuinfo
- Retorna -1.0f em falha
- Status: IMPLEMENTADO

**RF-009: Processos Ativos**

- `cpu_get_active_processes()` retorna contagem
- De `/proc/loadavg`, terceiro campo
- Retorna -1 em falha
- Status: IMPLEMENTADO

**RF-011: Injeção de Dados para Teste**

- Variáveis de ambiente: `CPU_READER_*_PATH`
- Sem recompilação
- Status: IMPLEMENTADO

#### **BAIXA PRIORIDADE**

**RF-012: Otimização x86_64**

- Assembly inline para soma de contadores
- Fallback C em outras plataformas
- Status: IMPLEMENTADO

---

### 5.2 Requisitos Não-Funcionais

#### **Compatibilidade (ALTA)**

**RNF-001: C99**

- Compilar sem erros: `gcc -std=c99 -Wall -Wextra -Wpedantic`
- Status: CONFORMANTE

**RNF-002: Linux**

- Funciona em Debian, RHEL, Alpine
- Kernel 3.10+
- Status: CONFORMANTE

**RNF-003: Sem Dependências Externas (Núcleo)**

- libcpu.a sem ncurses, zlib, libxml, etc.
- Apenas POSIX C
- Status: CONFORMANTE

#### **Performance (MÉDIA)**

**RNF-005: Performance**

- `cpu_get_info()` < 10ms
- `cpu_get_usage()` < 5ms
- Monitor < 100ms latência
- Status: ATENDE

#### **Segurança de Memória (ALTA)**

**RNF-006: Segurança de Memória**

- Sem overflow de buffer (buffers fixos com limites)
- Sem double-free (calloc + free)
- Sem vazamentos (testado com Valgrind)
- Status: ️ IMPLEMENTADO (sem CI formal)

#### **Tratamento de Erros (ALTA)**

**RNF-007: Tratamento de Erros**

- Valores bem definidos: NULL, -1.0f, -1
- Códigos de erro: `cpu_error_t`
- Mensagens descritivas
- Status: CONFORMANTE

#### **Documentação (MÉDIA)**

**RNF-009: Documentação**

- README com compilação e uso
- Exemplos em examples/
- Testes em tests/
- Comentários em funções públicas
- Status: CONFORMANTE

#### **Testabilidade (MÉDIA)**

**RNF-010: Testabilidade**

- Variáveis de ambiente para injeção
- `make test` executável
- Status: CONFORMANTE

---

## 6. Requisitos de Interface

### 6.1 Interface de Usuário

Não há UI integrada. Monitor ncurses é exemplo separado.

### 6.2 Interface de Hardware

**Dependências:**

- CPU com `/proc/cpuinfo` acessível (todo Linux moderno)
- Sensor de temperatura (opcional, sysfs)
- Frequência de clock (opcional, sysfs)

### 6.3 Interface de Software

**Include:** `#include <cpu.h>`

**Linking:** `gcc ... -lcpu` ou `gcc ... libcpu.a`

**Tipos:**

```c
typedef enum { CPU_ERROR_* } cpu_error_t;
typedef struct { int cores; ... } cpu_info_t;
typedef struct { unsigned long long ...; } cpu_usage_context_t;
```

**Funções:** 13 funções públicas (vide API em cpu.h)

### 6.4 Interface de Comunicação

**Nenhuma.** Biblioteca é local, não faz rede.

---

## 7. Atributos do Sistema

### 7.1 Confiabilidade

- Funções retornam valores definidos em caso de falha
- Sem core dumps por falhas de I/O
- Tratamento defensivo de parsing

### 7.2 Disponibilidade

- Sem dependência de rede
- Funciona offline
- Acesso a `/proc` é rápido (kernel buffer)

### 7.3 Segurança

- **Sem elevação de privilégio** necessária
- Lê apenas `/proc/sys` públicos
- Sem escrita para arquivos de sistema

### 7.4 Manutenibilidade

- Módulos bem separados (cpu.c, cpu_info.c, cpu_usage.c)
- Sem copy-paste significativo
- Fácil adicionar nova métrica

### 7.5 Portabilidade

- C99 compatível
- Sem compilação condicional (exceto assembly x86_64)
- Testa em x86_64 e ARM (aspiração)

---

## 8. Critérios de Aceitação

### 8.1 Compilação

- [ ] `make` compila sem erros
- [ ] `make test` compila e executa sem falhas
- [ ] `make clean` remove build/ e libcpu.a

### 8.2 API

- [ ] `cpu_get_info()` retorna dados válidos
- [ ] `cpu_get_usage()` varia ao longo do tempo
- [ ] `cpu_get_temperature()` retorna temp ou -1.0f
- [ ] `cpu_get_clock_speed()` retorna freq ou -1.0f
- [ ] `cpu_get_active_processes()` retorna contagem ou -1
- [ ] Falhas retornam valores esperados
- [ ] Erros reportáveis via `cpu_get_last_error_*()`

### 8.3 Memória

- [ ] Nenhum vazamento em Valgrind `--leak-check=full`
- [ ] Nenhum erro em `gcc -fsanitize=address`
- [ ] `cpu_free_info()` não causa segfault

### 8.4 Monitor

- [ ] `build/cpu-monitor` inicia sem ncurses error
- [ ] Atualiza a tela cada ~1 segundo
- [ ] Tecla 'q' encerra

---

## 9. Glossário

| Termo | Significado |
| ------------- | ------------------------------------------------------------ |
| **Delta** | Diferença entre duas leituras |
| **Context** | Estado mantido entre chamadas |
| **Fixture** | Dados de teste injetados |
| **Sanitizer** | Ferramenta de detecção de erros (AddressSanitizer, etc.) |
| **sysfs** | Pseudo-filesystem para exposição de atributos de dispositivo |

---

## 10. Apêndices

### 10.1 Matriz de Rastreabilidade

| RF | Teste | Exemplo | Doc | Status |
| ------ | ---------- | --------- | --------- | ------ |
| RF-001 | test_cpu.c | monitor.c | cpu.h | |
| RF-002 | test_cpu.c | monitor.c | cpu.h | |
| RF-003 | test_cpu.c | monitor.c | cpu.h | |
| RF-004 | test_cpu.c | monitor.c | cpu.h | |
| RF-005 | test_cpu.c | monitor.c | cpu.h | |
| RF-006 | test_cpu.c | - | cpu.h | |
| RF-007 | test_cpu.c | monitor.c | cpu.h | |
| RF-008 | test_cpu.c | monitor.c | cpu.h | |
| RF-009 | test_cpu.c | monitor.c | cpu.h | |
| RF-010 | test_cpu.c | - | cpu.h | |
| RF-011 | test_cpu.c | - | README | |
| RF-012 | test_cpu.c | - | design.md | |

### 10.2 Diagrama de Dependências

```
Aplicação do Usuário
 ├─ include/cpu.h
 ├─ libcpu.a (src/cpu.c + src/cpu_info.c + src/cpu_usage.c)
 └─ Linux kernel (/proc/*, /sys/*)

Monitor (exemplo)
 ├─ include/cpu.h
 ├─ libcpu.a
 ├─ ncurses (-lncurses)
 └─ Linux kernel
```

### 10.3 Limitações Conhecidas

| Limitação | Impacto | Workaround |
| ---------------------------- | ------------------------------------------ | ------------------------------------- |
| Não thread-safe | Uso concorrente requer sincronização | Usar `cpu_usage_context_t` por thread |
| Sem métricas por núcleo | Não pode monitorar núcleos individualmente | Futuro RF-013 |
| Temperatura opcional | Nem todas máquinas têm sensor | Retorna -1.0f |
| `/proc` específico Linux | Não funciona em Windows | Usar libcpuid ou equivalente |
| Buffers fixos (model, flags) | Nomes muito longos truncados | Aumentar tamanho em header |

### 10.4 Riscos Identificados

| Risco | Probabilidade | Impacto | Mitigação |
| ------------------------------------- | ------------- | ------- | -------------------------- |
| Parsing `/proc` quebra em novo kernel | MÉDIA | ALTO | Teste em múltiplos kernels |
| Vazamento de memória | BAIXA | MÉDIO | Valgrind + CI |
| Performance degrada | BAIXA | MÉDIO | Benchmarks + CI |
| Incompatibilidade ARM64 | BAIXA | MÉDIO | Teste em arm64 CI |

---

## 11. Histórico de Revisões

| Versão | Data | Autor | Mudanças |
| ------ | ---------- | --------- | -------------------------------------------------------- |
| 1.0 | 2026-01 | Tim | Versão inicial (RF-001 a RF-010) |
| 2.0 | 2026-09-04 | Arquiteto | Consolidação de RF-011 a RF-012, RNF-001 a RNF-012, ADRs |

---

**Data de Aprovação:** 2026-09-04 
**Próxima Revisão:** 2026-12-31 
**Revisor:** Lead Técnico do Projeto
