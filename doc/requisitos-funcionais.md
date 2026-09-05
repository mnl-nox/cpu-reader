# Requisitos Funcionais (RF) do CPU Reader - Consolidado

## Visão geral

Este documento consolida todos os requisitos funcionais da biblioteca CPU Reader, organizados por módulo e com critérios de aceitação detalhados. Os requisitos marcados como estão implementados; ️ está parcialmente implementado; e está em escopo futuro.

---

## Módulo 1: Inicialização e Limpeza

### RF-001: Inicialização da biblioteca

**Descrição:** A aplicação deve poder inicializar a biblioteca de forma segura.

**Funcionalidade esperada:**

- `cpu_init()` retorna 0 em sucesso e não-zero em erro
- Zera o contexto padrão de cálculo de uso
- Pode ser chamada múltiplas vezes (idempotente)

**Critérios de Aceitação:**

```c
cpu_init();
float usage1 = cpu_get_usage(); // retorna 0.0f (primeira leitura)
float usage2 = cpu_get_usage(); // retorna valor entre 0-100
assert(usage1 == 0.0f);
assert(usage2 >= 0.0f && usage2 <= 100.0f);
```

**Prioridade:** ALTA | **Status:** Implementado

---

### RF-002: Limpeza da biblioteca

**Descrição:** A aplicação deve poder liberar recursos da biblioteca.

**Funcionalidade esperada:**

- `cpu_cleanup()` libera recursos alocados pela biblioteca
- Zera o estado do contexto padrão
- Pode ser chamada múltiplas vezes sem erro

**Critérios de Aceitação:**

```c
cpu_init();
cpu_cleanup();
cpu_init(); // sem erro
cpu_cleanup();
```

**Prioridade:** ALTA | **Status:** Implementado

---

## Módulo 2: Informações da CPU

### RF-003: Leitura de informações básicas

**Descrição:** A aplicação deve obter modelo, frequência, cores e flags da CPU.

**Funcionalidade esperada:**

- `cpu_get_info()` retorna `cpu_info_t*` com os seguintes campos preenchidos:
 - `cores`: número de processadores lógicos de `/proc/cpuinfo`
 - `threads`: processadores online (via `sysconf(_SC_NPROCESSORS_ONLN)`)
 - `model`: primeira linha `model name` de `/proc/cpuinfo` (até 255 chars)
 - `frequency_mhz`: primeira linha `cpu MHz` de `/proc/cpuinfo`
 - `flags`: primeiro `flags` de `/proc/cpuinfo` (até 511 chars)
 - `active_processes`: preenchido se `/proc/loadavg` for legível, senão 0
 - `temperature_c`: preenchido se sensor `sysfs` for disponível, senão 0

**Critérios de Aceitação:**

```c
cpu_info_t *info = cpu_get_info();
assert(info != NULL);
assert(info->cores > 0);
assert(info->threads > 0);
assert(strlen(info->model) > 0);
assert(info->frequency_mhz > 0);
assert(strlen(info->flags) > 0);
cpu_free_info(info);
```

**Retorno em falha:** `NULL` se `/proc/cpuinfo` não puder ser aberto ou se `calloc()` falhar.

**Prioridade:** ALTA | **Status:** Implementado

---

### RF-004: Liberação de memória

**Descrição:** A aplicação deve liberar a estrutura retornada por `cpu_get_info()`.

**Funcionalidade esperada:**

- `cpu_free_info(cpu_info_t *info)` libera a memória alocada
- Pode receber `NULL` sem erro
- Estrutura não é mais válida após liberação

**Critérios de Aceitação:**

```c
cpu_info_t *info = cpu_get_info();
if (info != NULL) {
 printf("Model: %s\n", info->model);
 cpu_free_info(info);
 info = NULL; // boa prática, mas não verificada pela API
}
cpu_free_info(NULL); // sem erro
```

**Prioridade:** ALTA | **Status:** Implementado

---

## Módulo 3: Uso da CPU

### RF-005: Cálculo de uso agregado

**Descrição:** A aplicação deve obter o percentual de uso agregado da CPU.

**Funcionalidade esperada:**

- `cpu_get_usage()` retorna valor entre `0.0f` e `100.0f`
- Primeira chamada estabelece a referência (lê `/proc/stat` e armazena em contexto padrão)
- Chamadas subsequentes calculam delta em relação à leitura anterior
- Primeira chamada retorna `0.0f`

**Fórmula de cálculo:**

```
total = user + nice + system + irq + softirq + steal + guest + guest_nice
idle = idle + iowait
delta_total = total_atual - total_anterior
delta_idle = idle_atual - idle_anterior
delta_active = delta_total - delta_idle
usage = (delta_active / delta_total) * 100.0f
```

**Critérios de Aceitação:**

```c
cpu_init();
float usage1 = cpu_get_usage();
assert(usage1 == 0.0f); // primeira leitura
usleep(100000); // espera 100ms
float usage2 = cpu_get_usage();
assert(usage2 >= 0.0f && usage2 <= 100.0f);
```

**Retorno em falha:** `-1.0f` se `/proc/stat` não puder ser lido ou parsing falhar.

**Prioridade:** ALTA | **Status:** Implementado

---

### RF-006: Contextos de uso independentes

**Descrição:** A aplicação pode manter múltiplos contextos de cálculo de uso independentes.

**Funcionalidade esperada:**

- `cpu_usage_context_init()` inicializa um contexto fornecido pela aplicação
- `cpu_usage_context_cleanup()` libera recursos do contexto
- `cpu_get_usage_context()` calcula uso para um contexto específico
- Cada contexto mantém seu próprio estado de leitura anterior

**Critérios de Aceitação:**

```c
cpu_usage_context_t ctx1, ctx2;
cpu_usage_context_init(&ctx1);
cpu_usage_context_init(&ctx2);

float u1_1 = cpu_get_usage_context(&ctx1); // 0.0f
float u2_1 = cpu_get_usage_context(&ctx2); // 0.0f

usleep(100000);

float u1_2 = cpu_get_usage_context(&ctx1); // valor real
float u2_2 = cpu_get_usage_context(&ctx2); // valor real (pode diferir de u1_2)

assert(u1_1 == 0.0f && u2_1 == 0.0f);
assert(u1_2 >= 0.0f && u2_2 >= 0.0f);

cpu_usage_context_cleanup(&ctx1);
cpu_usage_context_cleanup(&ctx2);
```

**Prioridade:** MÉDIA | **Status:** Implementado

---

## Módulo 4: Temperatura da CPU

### RF-007: Leitura de temperatura

**Descrição:** A aplicação deve obter a temperatura atual da CPU quando disponível.

**Funcionalidade esperada:**

- `cpu_get_temperature()` tenta ler temperatura de sensores `sysfs`
- Caminhos verificados: `/sys/class/thermal/thermal_zone0/temp` etc.
- Valor em Celsius (dividido por 1000 se necessário)
- Retorna `-1.0f` se sensor não estiver disponível ou se leitura falhar
- Pode ser redirecionado via `CPU_READER_CPU_TEMP_PATH` para teste

**Critérios de Aceitação:**

```c
float temp = cpu_get_temperature();
// Em máquina com sensor: 30.0f <= temp <= 120.0f
// Em máquina sem sensor: temp == -1.0f
assert(temp > -1.1f && (temp == -1.0f || (temp > 0.0f && temp < 150.0f)));
```

**Prioridade:** MÉDIA | **Status:** Implementado

---

## Módulo 5: Velocidade de Clock

### RF-008: Leitura de velocidade de clock

**Descrição:** A aplicação deve obter a frequência de clock atual da CPU.

**Funcionalidade esperada:**

- `cpu_get_clock_speed()` retorna velocidade em MHz
- Tenta ler primeiro de `/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq` (em kHz)
- Fallback para primeiro `cpu MHz` em `/proc/cpuinfo`
- Retorna `-1.0f` se ambos os caminhos falharem
- Pode ser redirecionado via `CPU_READER_CPU_FREQ_PATH` para teste

**Critérios de Aceitação:**

```c
float clock = cpu_get_clock_speed();
// Típico: 800.0f <= clock <= 5000.0f
// Sem sensor: -1.0f
assert(clock > -1.1f && (clock == -1.0f || (clock > 0.0f && clock < 10000.0f)));
```

**Prioridade:** MÉDIA | **Status:** Implementado

---

## Módulo 6: Processos Ativos

### RF-009: Contagem de processos ativos

**Descrição:** A aplicação deve obter o número de processos em estado executável.

**Funcionalidade esperada:**

- `cpu_get_active_processes()` lê `/proc/loadavg` e retorna o terceiro campo (processos running)
- Valor inteiro positivo ou 0
- Retorna `-1` se arquivo não puder ser lido
- Pode ser redirecionado via `CPU_READER_LOADAVG_PATH` para teste

**Critérios de Aceitação:**

```c
int procs = cpu_get_active_processes();
// Típico: 0 <= procs <= num_cpus
// Em falha: -1
assert(procs >= -1);
```

**Prioridade:** MÉDIA | **Status:** Implementado

---

## Módulo 7: Tratamento de Erros

### RF-010: Relatório de falhas

**Descrição:** A aplicação deve poder consultar informações sobre a última falha detectada.

**Funcionalidade esperada:**

- `cpu_get_last_error_code()` retorna um `cpu_error_t`:
 - `CPU_ERROR_NONE`: nenhum erro (sucesso)
 - `CPU_ERROR_INVALID_ARGUMENT`: argumento inválido
 - `CPU_ERROR_FILE_OPEN`: falha ao abrir arquivo
 - `CPU_ERROR_PARSE`: falha no parsing de dados
 - `CPU_ERROR_MEMORY`: falha em alocação de memória
 - `CPU_ERROR_UNSUPPORTED`: operação não suportada no sistema
- `cpu_get_last_error()` retorna string descritiva (até 255 chars)
- Relatório é zerado após chamada bem-sucedida
- Relatório é global (não thread-safe)

**Critérios de Aceitação:**

```c
// Sucesso limpa o erro anterior
cpu_get_info(); // sucesso
assert(cpu_get_last_error_code() == CPU_ERROR_NONE);

// Falha registra erro
cpu_set_last_error(CPU_READER_CPUINFO_PATH, "/nonexistent");
cpu_info_t *info = cpu_get_info();
assert(info == NULL);
assert(cpu_get_last_error_code() == CPU_ERROR_FILE_OPEN);
assert(strlen(cpu_get_last_error()) > 0);
```

**Prioridade:** ALTA | **Status:** Implementado

---

## Módulo 8: Injeção de Dados para Testes

### RF-011: Caminhos injetáveis

**Descrição:** A biblioteca deve permitir redirecionar as fontes de dados para teste.

**Funcionalidade esperada:**

- Variáveis de ambiente permitem substituir caminhos padrão:
 - `CPU_READER_CPUINFO_PATH`: substitui `/proc/cpuinfo`
 - `CPU_READER_PROC_STAT_PATH`: substitui `/proc/stat`
 - `CPU_READER_CPU_TEMP_PATH`: substitui `/sys/class/thermal/...`
 - `CPU_READER_CPU_FREQ_PATH`: substitui `/sys/devices/system/cpu/.../scaling_cur_freq`
 - `CPU_READER_LOADAVG_PATH`: substitui `/proc/loadavg`
- Permitir uso de fixtures de teste sem recompilar
- Sem impacto em performance (checagem apenas uma vez por função)

**Critérios de Aceitação:**

```bash
# Criar arquivo de fixture
mkdir -p /tmp/test_fixtures
echo "processor\t: 0
model name\t: Intel(R) Core(TM)" > /tmp/test_fixtures/cpuinfo

# Executar com injeção
CPU_READER_CPUINFO_PATH=/tmp/test_fixtures/cpuinfo ./build/test_cpu

# Biblioteca deve usar o arquivo injetado
```

**Prioridade:** MÉDIA | **Status:** Implementado

---

## Módulo 9: Otimizações de Performance

### RF-012: Soma otimizada de contadores (x86_64)

**Descrição:** Em x86_64 com GCC/Clang, usar assembly inline para somar contadores de CPU.

**Funcionalidade esperada:**

- Detecção em tempo de compilação: `#ifdef __x86_64__` com GCC/Clang
- Implementação em inline assembly que soma 8 contadores 64-bit
- Fallback C equivalente para outras arquiteturas
- Sem impact de performance (operação é pequena)
- Validação: ambas as versões retornam o mesmo resultado

**Critérios de Aceitação:**

```c
// Compilado em x86_64 com GCC/Clang
unsigned long long counters[8] = {1000, 200, 300, 0, 0, 0, 0, 0};
unsigned long long sum = cpu_sum_counters_asm(counters);
assert(sum == 1500);

// Em arm64, usa fallback C
```

**Prioridade:** BAIXA | **Status:** Implementado

---

## Matriz de Requisitos Funcionais

| ID | Módulo | Requisito | Prioridade | Status |
| ------ | ------------- | ----------------------- | ---------- | ------ |
| RF-001 | Inicialização | Inicializar biblioteca | ALTA | |
| RF-002 | Inicialização | Limpar biblioteca | ALTA | |
| RF-003 | Informações | Ler informações básicas | ALTA | |
| RF-004 | Informações | Liberar memória | ALTA | |
| RF-005 | Uso | Calcular uso agregado | ALTA | |
| RF-006 | Uso | Contextos independentes | MÉDIA | |
| RF-007 | Temperatura | Ler temperatura | MÉDIA | |
| RF-008 | Clock | Ler velocidade | MÉDIA | |
| RF-009 | Processos | Contar processos ativos | MÉDIA | |
| RF-010 | Erros | Reportar falhas | ALTA | |
| RF-011 | Testes | Injetar caminhos | MÉDIA | |
| RF-012 | Performance | Otimizar x86_64 | BAIXA | |

---

## Próximas RF (Escopo Futuro)

### RF-013: Métricas por núcleo

**Descrição:** Retornar uso e informações por núcleo de CPU individual.

**Funcionalidade:**

```c
cpu_core_info_t *cpu_get_core_info(int core_id);
float cpu_get_core_usage(int core_id);
```

**Status:** Futuro (P2 - Semana 3-4)

---

### RF-014: Cache configurável

**Descrição:** Opção de cachear resultados por período configurável.

**Status:** Futuro (P3 - Semana 5-6)

---

### RF-015: Thread-safety automática

**Descrição:** API thread-safe com sincronização interna.

**Status:** Futuro (P3 - Semana 5-6)

---

## Validação de Requisitos

Todos os RF implementados têm:

- Testes em `tests/test_cpu.c`
- Exemplos de uso em `examples/monitor.c`
- Documentação em `include/cpu.h`
- Critérios de aceitação verificados

Para executar testes:

```bash
make test # Compila e executa tests/test_cpu.c
```
