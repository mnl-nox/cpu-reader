# Design Document - CPU Reader

**Versão:** 2.0  
**Data:** 2026-09-04  
**Status:** ATIVO  
**Último revisor:** Arquiteto do Projeto

---

## 1. Visão

### 1.1 Objetivo

Fornecer uma biblioteca C99 leve e portável para consultar informações de CPU em sistemas Linux, incluindo uso agregado, temperatura, frequência de clock e processos ativos.

### 1.2 Público-alvo

- Aplicações de monitoramento de sistema
- Ferramentas de profiling e benchmarking
- Dashboards de performance
- Utilitários de linha de comando
- Bindings para linguagens de script (Python, Node.js, Go)

### 1.3 Restrições

- Apenas Linux (kernel 3.10+)
- Sem dependências externas no núcleo
- C99 apenas
- Sem garantias de thread-safety nesta versão
- Estruturas de tamanho fixo (não dinâmicas para todos os campos)

---

## 2. Arquitetura de Alto Nível

### 2.1 Diagrama de Componentes

```
┌─────────────────────────────────────────┐
│         Aplicação do Usuário            │
│      (monitor.c, apps externas)         │
└────────────────┬────────────────────────┘
                 │
    ┌────────────┴────────────┐
    │                         │
    v                         v
include/cpu.h (API pública)   examples/monitor.c (app ncurses)
    │                         │
    └────────────┬────────────┘
                 │
    ┌────────────v────────────┐
    │   src/cpu.c (Fachada)   │ (gerencia ciclo de vida e estado global)
    └────────────┬────────────┘
                 │
    ┌────────────┴──────────────────┬──────────────────┐
    │                               │                  │
    v                               v                  v
src/cpu_info.c              src/cpu_usage.c      src/cpu_internal.h
(leitura estática)          (cálculo dinâmico)   (interfaces internas)
    │                               │
    v                               v
/proc/cpuinfo          /proc/stat + /proc/loadavg
/sys (temp, freq)      sysfs (freq, temp)
```

### 2.2 Fluxo de Dados

```
Aplicação
   ↓
cpu_init() ──→ inicializa contexto padrão
   ↓
┌────────────────────────────────┐
│ cpu_get_info()                 │ → lê /proc/cpuinfo → CPU info (cores, modelo, flags)
│ cpu_get_usage()                │ → lê /proc/stat → uso (%)
│ cpu_get_temperature()          │ → lê sysfs → temp (°C)
│ cpu_get_clock_speed()          │ → lê sysfs/cpuinfo → freq (MHz)
│ cpu_get_active_processes()     │ → lê /proc/loadavg → processos (#)
└────────────────────────────────┘
   ↓
cpu_cleanup() ──→ libera estado do contexto padrão
   ↓
[Erro: cpu_get_last_error_code() + cpu_get_last_error()]
```

---

## 3. Componentes Detalhados

### 3.1 Núcleo: `include/cpu.h` (API Pública)

**Responsabilidade:** Contrato público da biblioteca.

**Tipos:**

```c
typedef enum {
    CPU_ERROR_NONE,              // Sucesso
    CPU_ERROR_INVALID_ARGUMENT,  // Argumento inválido
    CPU_ERROR_FILE_OPEN,         // Falha ao abrir arquivo
    CPU_ERROR_PARSE,             // Falha no parsing
    CPU_ERROR_MEMORY,            // Falha em alocação
    CPU_ERROR_UNSUPPORTED        // Operação não suportada
} cpu_error_t;

typedef struct {
    int cores;                    // Processadores lógicos
    int threads;                  // Threads online
    int active_processes;         // Processos ativos (0 se indisponível)
    float frequency_mhz;          // Frequência em MHz
    float usage_percent;          // Uso agregado (0-100, de contexto padrão)
    float temperature_c;          // Temperatura em Celsius (0 se indisponível)
    char model[256];              // Nome da CPU
    char flags[512];              // Flags de CPU
} cpu_info_t;

typedef struct {
    unsigned long long previous_total;
    unsigned long long previous_idle;
    int has_previous;
} cpu_usage_context_t;
```

**Funções Públicas:**

| Função                           | Descrição             | Retorno               |
| -------------------------------- | --------------------- | --------------------- |
| `cpu_init()`                     | Inicializa biblioteca | 0 sucesso, não-0 erro |
| `cpu_cleanup()`                  | Libera recursos       | void                  |
| `cpu_get_info()`                 | Lê informações de CPU | `cpu_info_t*` ou NULL |
| `cpu_free_info(ptr)`             | Libera estrutura      | void                  |
| `cpu_get_usage()`                | Calcula uso (%)       | 0-100 ou -1.0f        |
| `cpu_get_temperature()`          | Lê temperatura        | temp_c ou -1.0f       |
| `cpu_get_clock_speed()`          | Lê frequência         | freq_mhz ou -1.0f     |
| `cpu_get_active_processes()`     | Conta processos       | count ou -1           |
| `cpu_usage_context_init(ctx)`    | Inicializa contexto   | 0 sucesso             |
| `cpu_usage_context_cleanup(ctx)` | Limpa contexto        | void                  |
| `cpu_get_usage_context(ctx)`     | Calcula uso/contexto  | 0-100 ou -1.0f        |
| `cpu_get_last_error_code()`      | Código do último erro | `cpu_error_t`         |
| `cpu_get_last_error()`           | Mensagem de erro      | `const char*`         |

---

### 3.2 Leitura de Informações: `src/cpu_info.c`

**Responsabilidade:** Ler e parsear `/proc/cpuinfo` e complementar com dados de sysfs.

**Funções Internas:**

```c
cpu_info_t *cpu_read_info(void);           // Aloca e lê
void cpu_parse_cpuinfo(FILE*, cpu_info_t*); // Parse
float cpu_read_temperature(int retry);     // Tenta sysfs
float cpu_read_clock_speed(int retry);     // Tenta sysfs, fallback cpuinfo
int cpu_read_active_processes(int retry);  // Lê /proc/loadavg
```

**Fluxo:**

```
cpu_get_info()
   ↓
calloc(sizeof(cpu_info_t))  [malloc]
   ↓
fopen(CPU_READER_CPUINFO_PATH ou "/proc/cpuinfo")
   ↓
cpu_parse_cpuinfo()
   - Procura "processor" → cores
   - Procura "model name" → model[256]
   - Procura "cpu MHz" → frequency_mhz
   - Procura "flags" → flags[512]
   ↓
cpu_read_temperature()  [optional]
   ↓
cpu_read_clock_speed()  [optional]
   ↓
cpu_read_active_processes()  [optional]
   ↓
sysconf(_SC_NPROCESSORS_ONLN) → threads
   ↓
fclose()
   ↓
return cpu_info_t*
```

---

### 3.3 Cálculo de Uso: `src/cpu_usage.c`

**Responsabilidade:** Ler `/proc/stat`, somar contadores, calcular delta.

**Funções Internas:**

```c
float cpu_get_usage_context(cpu_usage_context_t *ctx);
void cpu_read_stat_line(unsigned long long *counters);
#ifdef __x86_64__
unsigned long long cpu_sum_counters_asm(unsigned long long *counters);
#else
unsigned long long cpu_sum_counters_c(unsigned long long *counters);
#endif
```

**Fórmula:**

```
Entrada: linha "cpu" de /proc/stat
user, nice, system, irq, softirq, steal, guest, guest_nice

Soma:
  total = user + nice + system + irq + softirq + steal + guest + guest_nice
  idle  = idle + iowait

Delta:
  delta_total = total_atual - total_anterior
  delta_idle  = idle_atual - idle_anterior
  delta_busy  = delta_total - delta_idle

Uso:
  uso = (delta_busy / delta_total) * 100.0

Primeira chamada:
  Armazena total/idle
  Retorna 0.0f
```

**Otimizações:**

- x86_64: inline assembly para somar 8 contadores
- Fallback C para outras arquiteturas
- Ambas as versões testadas para equivalência

---

### 3.4 Fachada: `src/cpu.c`

**Responsabilidade:** Ciclo de vida global, estado padrão, relatório de erros.

**Variáveis Globais:**

```c
static cpu_usage_context_t default_usage_context;  // Estado padrão
static cpu_error_t last_error_code;                 // Erro global
static char last_error_message[256];                // Mensagem
```

**Fluxo:**

```
cpu_init()
  → cpu_usage_context_init(&default_usage_context)

cpu_get_usage()
  → cpu_get_usage_context(&default_usage_context)

cpu_cleanup()
  → cpu_usage_context_cleanup(&default_usage_context)
```

**Gerenciamento de Erros:**

```c
void cpu_set_last_error(cpu_error_t code, const char *format, ...)
void cpu_clear_last_error(void)
```

Toda chamada que pode falhar chama `cpu_set_last_error()` em erro ou `cpu_clear_last_error()` em sucesso.

---

### 3.5 Aplicação de Exemplo: `examples/monitor.c`

**Responsabilidade:** UI interativa com ncurses.

**Dependência:** `-lncurses` (opcional, não no núcleo).

**Fluxo:**

```
ncurses_init()
  ↓
loop:
  cpu_get_info()          → Exibe cores, threads, modelo, flags
  cpu_get_usage()         → Exibe uso %
  cpu_get_temperature()   → Exibe temp °C
  cpu_get_clock_speed()   → Exibe freq MHz
  ↓
  sleep(1 segundo)
  ↓
  Tecla 'q' → sair

cpu_free_info()
ncurses_cleanup()
```

---

## 4. Padrões de Design Utilizados

### 4.1 Fachada (Facade)

`cpu.c` é uma fachada que expõe API unificada sobre componentes internos.

```
Aplicação
   ↓
cpu_get_info()  ─────────────────┐
                                 ├─→ Distribuição via cpu.c
cpu_get_usage() ─────────────────┤
cpu_get_temperature() ───────────┘

cpu.c chama internamente:
  cpu_read_info()
  cpu_get_usage_context()
  cpu_read_temperature()
```

### 4.2 Strategy (Estratégia)

Soma de contadores usa strategy: assembly inline vs C.

```c
#ifdef __x86_64__
  sum = cpu_sum_counters_asm(counters);
#else
  sum = cpu_sum_counters_c(counters);
#endif
```

### 4.3 Context (Contexto)

`cpu_usage_context_t` permite manter estado independente por thread/aplicação.

```c
cpu_usage_context_t ctx1, ctx2;
float uso1 = cpu_get_usage_context(&ctx1);
float uso2 = cpu_get_usage_context(&ctx2);
// ctx1 e ctx2 não interferem
```

### 4.4 Repository Pattern (dados)

Cada módulo encapsula acesso a uma "fonte de dados": cpuinfo, stat, sysfs.

---

## 5. Trade-offs de Design

| Decisão            | Vantagem      | Desvantagem           | Alternativa      |
| ------------------ | ------------- | --------------------- | ---------------- |
| C99                | Portabilidade | Sem C11 features      | C++              |
| `/proc`            | Sem deps      | Específico Linux      | libcpuid         |
| Global error       | Simples       | Não thread-safe       | thread_local     |
| Deltas             | API simples   | Primeira leitura 0.0f | Manual snapshots |
| Fixed-size buffers | Sem malloc    | Tamanho limitado      | malloc dinâmico  |
| Monitor separado   | Núcleo limpo  | Sem UI nativa         | UI integrada     |
| Assembly x86_64    | Performance   | Complexidade          | Só C             |

---

## 6. Diagrama de Classes UML

```
┌─────────────────────────────┐
│  cpu_error_t (enum)         │
├─────────────────────────────┤
│ CPU_ERROR_NONE              │
│ CPU_ERROR_INVALID_ARGUMENT  │
│ CPU_ERROR_FILE_OPEN         │
│ CPU_ERROR_PARSE             │
│ CPU_ERROR_MEMORY            │
│ CPU_ERROR_UNSUPPORTED       │
└─────────────────────────────┘

┌─────────────────────────────┐
│  cpu_info_t (struct)        │
├─────────────────────────────┤
│ + cores: int                │
│ + threads: int              │
│ + active_processes: int     │
│ + frequency_mhz: float      │
│ + usage_percent: float      │
│ + temperature_c: float      │
│ + model[256]: char          │
│ + flags[512]: char          │
└─────────────────────────────┘

┌──────────────────────────────────────┐
│  cpu_usage_context_t (struct)        │
├──────────────────────────────────────┤
│ + previous_total: unsigned long long │
│ + previous_idle: unsigned long long  │
│ + has_previous: int                  │
└──────────────────────────────────────┘

┌──────────────────────────────────┐
│  Module: cpu (Fachada)           │
├──────────────────────────────────┤
│ - default_usage_context (static) │
│ - last_error_code (static)       │
│ - last_error_message[256]        │
├──────────────────────────────────┤
│ + cpu_init(): int                │
│ + cpu_cleanup(): void            │
│ + cpu_get_info(): cpu_info_t*    │
│ + cpu_free_info(): void          │
│ + cpu_get_usage(): float         │
│ + cpu_get_temperature(): float   │
│ + cpu_get_clock_speed(): float   │
│ + cpu_get_active_processes(): int│
│ + cpu_usage_context_init(): int  │
│ + cpu_usage_context_cleanup()    │
│ + cpu_get_usage_context(): float │
│ + cpu_get_last_error_code()      │
│ + cpu_get_last_error(): const*   │
└──────────────────────────────────┘
```

---

## 7. Diagrama de Sequência: Leitura de Informações

```
Aplicação          cpu_get_info()      cpu_info.c         /proc/cpuinfo
    │                   │                  │                    │
    ├──call────────────>│                  │                    │
    │                   ├──allocate──────>│                    │
    │                   │  cpu_info_t      │                    │
    │                   │<──────────────────┤                    │
    │                   ├──open────────────────────────────────>│
    │                   │<──────────────────────────────────────┤ FILE*
    │                   ├──parse(FILE)────>│                    │
    │                   │  cores            │                    │
    │                   │  model            │                    │
    │                   │  flags            │                    │
    │                   │                   ├──read sysfs──────>│
    │                   │                   │<──temperature─────┤
    │                   │                   ├──read sysfs──────>│
    │                   │                   │<──frequency───────┤
    │                   ├──close────────────────────────────────>│
    │                   │<──────────────────────────────────────┤
    │<──return cpu_info_t*
    │
    ├──free────────────>│ cpu_free_info()
    │                   ├──free memory─────>│
    │                   │<──────────────────┤
    │<──return void──────┤
```

---

## 8. Diagrama de Sequência: Cálculo de Uso

```
Aplicação       cpu_get_usage()    cpu_usage.c      /proc/stat
    │                │                 │                │
    ├─cpu_init()───> │                 │                │
    │<──────────────┤ ├─read first────────────────────>│
    │                │ │ (total, idle)  │                │
    │                │ │<──────────────────────────────┤
    │                │ │ store reference                │
    │                │ │ return 0.0f                    │
    │                │<────return 0.0f─┤
    │
    ├─sleep(100ms)──>
    │
    ├─cpu_get_usage()──>│                 │
    │                 ├─read current────────────────────>│
    │                 │ (total2, idle2) │                │
    │                 │<──────────────────────────────────┤
    │                 │ delta_total = total2 - total      │
    │                 │ delta_idle = idle2 - idle         │
    │                 │ usage = (delta_total - delta_idle)│
    │                 │        / delta_total * 100        │
    │                 │ return usage (0..100)             │
    │                 │<────return usage %─┤
    │<──return usage %──┤
    │
    ├─cpu_cleanup()──>  │
    │                 ├─reset context──────────────────>
    │<──return void─────┤
```

---

## 9. Padrão de Erro

```
Operação
   │
   ├─Sucesso: cpu_clear_last_error(), return valor_válido
   │
   └─Erro: cpu_set_last_error(code, msg), return valor_erro
            │
            Aplicação consulta:
            ├─cpu_get_last_error_code() → cpu_error_t
            └─cpu_get_last_error() → const char* (mensagem)
```

---

## 10. Roadmap de Evolução Arquitetural

### Fase 1 (Atual): MVP Robustez

- [x] Biblioteca funcional
- [x] Testes básicos
- [x] Múltiplas plataformas (x86_64, ARM)
- [ ] Cobertura >80%
- [ ] CI/CD formal

### Fase 2: Performance e Features

- [ ] Métricas por núcleo (RF-013)
- [ ] Cache configurável (RF-014)
- [ ] Thread-safety (RF-015)
- [ ] Benchmarks formais

### Fase 3: Integração Contínua

- [ ] GitHub Actions/CI
- [ ] Múltiplas distribuições
- [ ] Release automation
- [ ] API bindings

### Fase 4: Otimizações Avançadas

- [ ] SIMD para parsing
- [ ] Memory mapping para /proc
- [ ] Modo low-power
- [ ] Histórico de métricas

---

## 11. Considerações de Performance

### Crítico

- `cpu_get_usage()`: deve completar em < 5ms
- `cpu_get_info()`: deve completar em < 10ms
- Monitor: latência < 100ms

### Optimizações Implementadas

- Assembly inline para soma x86_64
- Leitura sequencial de `/proc/stat`
- Sem malloc por chamada (alocação uma vez por estrutura)
- Parsing defensivo sem regex

### Benchmarks Futuros

```bash
perf stat ./build/cpu-monitor
# Medir: IPC, cache misses, branch prediction
```

---

## 12. Segurança de Memória

### Proteções

- Buffers fixos: `model[256]`, `flags[512]`
- Parsing defensivo com limites
- `calloc()` inicializa com zero
- `cpu_free_info()` libera alocação

### Testes

```bash
valgrind --leak-check=full ./build/test_cpu
gcc -fsanitize=address,undefined -g ./test_cpu.c
clang -fsanitize=memory ./test_cpu.c
```

---

## 13. Pontos de Extensão Futuros

1. **Callbacks:** `cpu_register_update_callback()` para notificações
2. **Eventos:** Alertas automáticos (temp alta, carga máxima)
3. **Network:** Exposição de métricas via HTTP/Prometheus
4. **Storage:** Histórico persistido em BD/arquivo
5. **Sincronização:** Mutex para thread-safety
6. **Perfis:** Modo "low-power" com cache maior

---

## 14. Documentação Relacionada

- [Requisitos Funcionais (RF)](requisitos-funcionais.md)
- [Requisitos Não-Funcionais (RNF)](requisitos-nao-funcionais.md)
- [Architecture Decision Records (ADR)](architecture-decision-records.md)
- [Casos de Uso](casosdeuso.md)
- [Diagramas](diagramas.md)

---

**Próxima revisão:** Q4 2026  
**Revisor:** Arquiteto do Projeto
