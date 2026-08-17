# Decisões de Design de Software - CPU Reader

## 1. Document Control

| Propriedade  | Valor                   |
| ------------ | ----------------------- |
| Versão       | 1.0                     |
| Data         | 2026-08-16              |
| Status       | Aprovado                |
| Autor        | Tech Lead               |
| Stakeholders | Dev Team, QA, Tech Lead |

---

## 2. Overview

Este documento captura as decisões arquiteturais e de design tomadas para o projeto CPU Reader. Cada decisão inclui contexto, alternativas consideradas, e rationale.

---

## 3. Decisões Arquiteturais

### AD-001: Linguagem C99

**Situação**: Escolher linguagem de programação para a biblioteca.

**Opcões Consideradas**:

1. **C99** (Escolhida) - Simples, eficiente, portável
2. C11 - Mais recursos, menos compatível com sistemas antigos
3. C++ - Mais complexo, mais overhead
4. Rust - Segurança, mais novo, menos adoção

**Decisão**: **Usar C99**

**Rationale**:

- C99 é amplamente suportado em sistemas Linux embarcados
- Máxima compatibilidade com sistemas antigos (kernel 2.6+)
- Eficiência crítica para operações de leitura de sistema
- Simplicidade de interface para integração
- Sem dependências de runtime

**Consequências**:

- ✓ Compatibilidade excelente
- ✓ Performance máxima
- ✓ Tamanho de binário mínimo
- ✗ Sem resources de linguagens mais modernas
- ✗ Gerenciamento manual de memória

**Mitigação**:

- Usar valgrind e Address Sanitizer para detectar memory leaks
- Code review rigoroso
- Testes de cobertura >= 80%

---

### AD-002: Arquitetura em Camadas

**Situação**: Definir estrutura interna da biblioteca.

**Opcões Consideradas**:

1. **Monolítico em camadas** (Escolhida) - Simples, coeso
2. Microserviços - Overhead desnecessário
3. Plugin-based - Complexidade adicional
4. Flat structure - Sem organização clara

**Decisão**: **Arquitetura em 3 camadas**

```
┌─────────────────────┐
│  Camada de API      │ (include/cpu.h)
├─────────────────────┤
│  Camada de Lógica   │ (src/cpu.c)
├─────────────────────┤
│  Camada de Sistema  │ (/proc filesystem)
└─────────────────────┘
```

**Rationale**:

- Separação clara de responsabilidades
- Fácil de manter e estender
- Sem acoplamento entre camadas
- Interface estável para usuários

**Consequências**:

- ✓ Código bem organizado
- ✓ Fácil de debugar
- ✓ Componentes reutilizáveis
- ✗ Pequeno overhead de função calls

**Documentação**: [doc/arquitetura.md](arquitetura.md)

---

### AD-003: Leitura via /proc Filesystem

**Situação**: Como obter dados da CPU do sistema.

**Opcões Consideradas**:

1. **/proc filesystem** (Escolhida) - Portable, simples
2. Chamadas de sistema (syscalls) - Complexo, menos portable
3. Bibliotecas externas - Dependência extra
4. IOCTL - Específico do kernel, difícil de usar

**Decisão**: **Usar /proc/cpuinfo e /proc/stat**

**Arquivos**:

- `/proc/cpuinfo` - Informações estáticas
- `/proc/stat` - Dados dinâmicos de CPU
- `/proc/thermal_zone/` - Informações de temperatura

**Rationale**:

- Funciona em qualquer distribuição Linux
- Sem dependências de bibliotecas externas
- Fácil de parsear (formato texto)
- Permite leitura de dados brutos

**Consequências**:

- ✓ Máxima compatibilidade
- ✓ Sem dependências
- ✓ Funcionamento offline possível
- ✗ Formato pode variar entre kernels
- ✗ Requer tratamento de casos edge

**Mitigação**:

- Parser robusto com fallback
- Suporte a múltiplas versões de kernel
- Testes em Linux 4.4+

---

### AD-004: Estrutura de Dados - cpu_info_t

**Situação**: Definir formato de retorno de dados.

**Opcões Consideradas**:

1. **Struct cpu_info_t** (Escolhida) - Tipado, eficiente
2. Dinamicamente tipado (JSON) - Flexível, overhead
3. Array de valores - Sem semântica
4. Múltiplas funções - Muitas chamadas

**Decisão**: **Usar struct cpu_info_t em C99**

```c
typedef struct {
    int cores;
    int threads;
    float frequency_mhz;
    float usage_percent;
    float temperature_c;
    char model[256];
    char flags[512];
} cpu_info_t;
```

**Rationale**:

- Type-safe, compilador valida
- Eficiente em memória e acesso
- Interface clara para usuários
- Facilita extensão futura

**Consequências**:

- ✓ Type safety
- ✓ Performance
- ✓ Interface clara
- ✗ Menos flexível que JSON
- ✗ Tamanho fixo de strings

**Mitigação**:

- Documentar campos claramente
- Validação de dados em tempo de leitura
- Versioning de struct para futuro

---

### AD-005: Gerenciamento de Memória - malloc/free

**Situação**: Como alocar e liberar memória dinamicamente.

**Opcões Consideradas**:

1. **malloc/free explícito** (Escolhida) - Controle total
2. Stack allocation apenas - Limitado, sem flexibilidade
3. Memory pool - Complexo, overkill
4. Garbage collection - Não disponível em C

**Decisão**: **Usar malloc/free com validação**

**Padrão**:

```c
cpu_info_t* info = malloc(sizeof(cpu_info_t));
if (!info) return NULL;
// ... usar ...
free(info);
```

**Rationale**:

- Explicita propriedade da memória
- Suporta objetos de tamanho variável
- Padrão em C
- Facilita debug com valgrind

**Consequências**:

- ✓ Controle total
- ✓ Detectável com ferramentas
- ✗ Risco de memory leaks
- ✗ Responsabilidade do usuário

**Mitigação**:

- Exemplos documentados
- Testes com valgrind
- Address Sanitizer em CI
- Funções cleanup explícitas

---

### AD-006: Tratamento de Erros - Códigos Numéricos

**Situação**: Como retornar erros para aplicação.

**Opcões Consideradas**:

1. **Códigos de erro numéricos** (Escolhida) - Simples, padrão em C
2. Exceções - Não existe em C puro
3. Valores sentinela (-1, NULL) - Ambíguo
4. Callbacks - Complexo

**Decisão**: **Usar códigos de erro convencionais**

```c
#define CPU_SUCCESS    0
#define CPU_ERROR      -1
#define CPU_ENOMEM     -2
#define CPU_EPERM      -3
#define CPU_ENOTSUP    -4
```

**Rationale**:

- Padrão em C
- Simples de verificar (if retval != 0)
- Compatível com errno
- Fácil de documentar

**Consequências**:

- ✓ Padrão C
- ✓ Simples
- ✓ Documentável
- ✗ Sem stack trace
- ✗ Requer documentação de cada erro

**Documentação**: Função deve documentar todos os códigos de erro possíveis.

---

### AD-007: Interface Pública - Mínimo de Funções

**Situação**: Quantas funções expor na API pública.

**Opcões Consideradas**:

1. **Mínimo (~6 funções)** (Escolhida) - Simples, fácil de usar
2. Completo (20+) - Flexível, complexo
3. Maximalista (50+) - Muito overhead
4. Single function - Muito restritivo

**Decisão**: **Expor apenas 6 funções públicas**

```c
cpu_info_t* cpu_get_info(void);
float cpu_get_usage(void);
float cpu_get_temperature(void);
void cpu_free_info(cpu_info_t* info);
int cpu_init(void);
void cpu_cleanup(void);
```

**Rationale**:

- Princípio KISS (Keep It Simple, Stupid)
- Fácil de aprender para usuários
- Fácil de manter para desenvolvedores
- Superfície mínima de API

**Consequências**:

- ✓ Interface clara
- ✓ Fácil de usar
- ✓ Fácil de documentar
- ✗ Menos flexibilidade
- ✗ Requisitos futuros podem precisar extensão

**Mitigação**:

- Versioning semântico de API
- Callbacks internos para extensões futuras
- Documentar mecanismo de extensão

---

### AD-008: Armazenamento em Cache - Simples

**Situação**: Como cachear dados de leitura de /proc.

**Opcões Consideradas**:

1. **Cache simples com timestamp** (Escolhida) - Balanceado
2. Sem cache - Sempre relê, ineficiente
3. Cache agressivo - Dados stale
4. Cache com TTL configurável - Complexidade extra

**Decisão**: **Cache com TTL de 1 segundo**

```c
// Interno ao cpu.c
static cpu_info_t* cache = NULL;
static time_t cache_timestamp = 0;
#define CACHE_TTL_MS 1000
```

**Rationale**:

- Reduz I/O ao filesystem
- 1 segundo é suficiente para maioria dos casos
- Simples de implementar
- Trade-off entre freshness e performance

**Consequências**:

- ✓ Performance melhorada
- ✓ Simples
- ✓ Previsível
- ✗ Dados podem ter até 1s de atraso
- ✗ Não é thread-safe atualmente

**Mitigação**:

- Documentar que dados têm até 1s de atraso
- Futuro: implementar mutex para thread-safety

---

### AD-009: Build System - Make

**Situação**: Qual ferramenta de build usar.

**Opcões Consideradas**:

1. **Makefile** (Escolhida) - Simples, portable
2. CMake - Mais poderoso, menos simples
3. Autotools - Complexo, overhead
4. Build manual - Sem reprodutibilidade

**Decisão**: **Usar Makefile simples**

```makefile
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2
LDFLAGS = -lm

all: libcpu.a libcpu.so

libcpu.a: src/cpu.o
	ar rcs $@ $^

libcpu.so: src/cpu.o
	$(CC) -shared -o $@ $^

clean:
	rm -f src/*.o libcpu.* test_cpu
```

**Rationale**:

- Todas as distribuições Linux têm make
- Simples de entender e manter
- Sem dependências extras
- Reprodutível

**Consequências**:

- ✓ Portável
- ✓ Simples
- ✓ Rápido
- ✗ Menos poderoso que CMake
- ✗ Não é ideal para projetos grandes

**Futuro**: Manter compatibilidade se migrar para CMake.

---

### AD-010: Testes - Google Test / Custom Framework

**Situação**: Framework de testes a usar.

**Opcões Consideradas**:

1. **Custom test framework em C** (Escolhida) - Simples, zero dependencies
2. Google Test - Poderoso mas C++
3. Unity Test - Bom mas outra dependência
4. Sem testes - Risco

**Decisão**: **Custom test framework simples**

```c
// test/test.h
#define ASSERT_EQUAL(a, b) \
    if ((a) != (b)) { \
        fprintf(stderr, "FAIL: %s:%d\n", __FILE__, __LINE__); \
        return 1; \
    }

int tests_run = 0;
int tests_failed = 0;
```

**Rationale**:

- Zero dependências externas
- Controle total
- Fácil de estender
- Aprender C ao mesmo tempo

**Consequências**:

- ✓ Sem dependências
- ✓ Simples
- ✓ Porém, menos recursos que frameworks profissionais
- ✗ Precisa manutenção própria

**Mitigação**:

- Framework bem documentado
- Crescer conforme necessidade
- Considerar Unity Test depois se crescer

---

### AD-011: Versionamento - Semantic Versioning

**Situação**: Como versionar o projeto.

**Opcões Consideradas**:

1. **Semantic Versioning (SemVer)** (Escolhida) - Padrão moderno
2. CalVer - Para projetos com releases regulares
3. Sequential - 1, 2, 3, ...
4. Date-based - 2026.09.01

**Decisão**: **Usar SemVer (MAJOR.MINOR.PATCH)**

```
v0.1.0 - MVP inicial
v1.0.0 - Release estável
v1.1.0 - Novas features, compatível
v1.1.1 - Bug fix patch
v2.0.0 - Breaking changes
```

**Rationale**:

- Padrão da indústria
- Comunidade entende imediatamente
- Ferramentas suportam
- Facilita dependências

**Consequências**:

- ✓ Claro para usuários
- ✓ Padrão conhecido
- ✗ Requer disciplina na classificação

---

### AD-012: Documentação - Markdown em Git

**Situação**: Onde e como documentar.

**Opcões Consideradas**:

1. **Markdown em Git (doc/)** (Escolhida) - Versionado, simples
2. Wiki externa - Difícil sincronizar
3. Doxygen - Bom, mais complexo
4. API.md apenas - Incompleto

**Decisão**: **Markdown em doc/ no Git**

```
doc/
├── README.md           (Índice)
├── arquitetura.md      (Design)
├── casosdeuso.md       (Use cases)
├── criterios.md        (Acceptance)
├── diagramas.md        (Visual)
├── requerimentos.md    (Requirements)
├── decisao-design.md   (This file)
└── prioridades-sdlc.md (Planning)
```

**Rationale**:

- Versionado com código
- Fácil de revisar em PR
- Git-native, sem ferramentas extras
- Markdown é universal

**Consequências**:

- ✓ Versionado
- ✓ Simples
- ✓ Integrado no workflow
- ✗ Menos pretty que Doxygen
- ✗ Precisa manutenção manual

---

### AD-013: Segurança - Validação Rigorosa

**Situação**: Nível de validação de entrada.

**Opcões Consideradas**:

1. **Validação rigorosa** (Escolhida) - Seguro, mas overhead
2. Confiança na entrada - Rápido, inseguro
3. Validação soft - Meio-termo

**Decisão**: **Validar toda entrada**

```c
int cpu_init(void) {
    if (access("/proc/cpuinfo", R_OK) != 0) {
        return CPU_EPERM;
    }
    // ... continua ...
}
```

**Rationale**:

- Segurança é crítico
- Previne buffer overflows
- Detecta problemas cedo
- Torna predictável

**Consequências**:

- ✓ Seguro
- ✓ Robusto
- ✗ Pequeno overhead de validação

---

### AD-014: Logging - Stderr e Syslog

**Situação**: Como registrar eventos e erros.

**Opcões Consideradas**:

1. **Stderr + Syslog opcional** (Escolhida) - Flexível
2. Só stderr - Simples
3. Arquivo de log - Gerenciamento
4. Sem logging - Difícil debug

**Decisão**: **Logging para stderr, opcional syslog**

```c
#define CPU_LOG(fmt, ...) \
    fprintf(stderr, "[CPU] " fmt "\n", ##__VA_ARGS__)

#ifdef CPU_DEBUG
    CPU_LOG("Debug info");
#endif
```

**Rationale**:

- Aplicação decide redireção
- Simples mas eficaz
- Debug mode para development

**Consequências**:

- ✓ Flexível
- ✓ Simples
- ✓ Padrão UNIX

---

## 4. Decisões Técnicas

### TD-001: Formato de Retorno de Uso de CPU

**Situação**: Como retornar valor de CPU usage.

**Opcões**:

1. Percentual 0-100 (float) - Intuitivo
2. Ratio 0.0-1.0 - Matemático
3. Inteiro 0-100 - Perda de precisão

**Decisão**: **Float 0-100%**

```c
float cpu_usage = cpu_get_usage();
if (cpu_usage > 80.0f) {
    alert("CPU high");
}
```

---

### TD-002: Suporte a Multi-core

**Situação**: Retornar um valor ou por core.

**Opcões**:

1. Agregado (uma função) - Simples
2. Por core (array) - Complexo
3. Ambos - Mais funções

**Decisão**: **Agregado por enquanto, preparar para expansão futura**

```c
float cpu_get_usage(void);         // Agregado
// Futuro: float* cpu_get_usage_per_core(int* count);
```

---

### TD-003: Formato de Model String

**Situação**: Como retornar model name.

**Decisão**: **String fixa de 256 caracteres**

```c
char model[256];  // Ex: "Intel Core i7-9700K"
```

---

## 5. Compromissos e Trade-offs

### Simplicidade vs Funcionalidade

- **Escolha**: Simplicidade
- **Justificativa**: Começar simples, expandir depois
- **Aceitação**: Menos features inicialmente

### Performance vs Segurança

- **Escolha**: Segurança ligeiramente acima
- **Justificativa**: Segurança é não-negociável
- **Aceitação**: ~5% overhead de validação

### Portabilidade vs Features

- **Escolha**: Portabilidade
- **Justificativa**: Linux é máxima prioridade
- **Aceitação**: Sem Windows inicialmente

### Flexibilidade vs Simplicidade

- **Escolha**: Simplicidade
- **Justificativa**: Interface clara é mais importante
- **Aceitação**: Menos customização

---

## 6. Padrões de Design Utilizados

### Factory Pattern

- `cpu_get_info()` cria nova instância de `cpu_info_t`

### Singleton (interno)

- Cache de dados para evitar leituras desnecessárias

### Resource Acquisition Is Initialization (RAII)

- `cpu_init()` vs `cpu_cleanup()`
- Embora não seja C++ verdadeiro, segue princípio

---

## 7. Decisões Futuras (v1.1+)

### FD-001: Thread-safety

**Status**: Planejado para v1.1  
**Opção**: Usar mutex para cache compartilhado  
**Impacto**: Pequeno overhead de sincronização

### FD-002: Histórico de Dados

**Status**: Planejado para v1.1  
**Opção**: Ring buffer interno  
**Impacto**: Mais memória consumida

### FD-003: Suporte Callback

**Status**: Planejado para v1.2  
**Opção**: Callbacks registrados para eventos  
**Impacto**: Mais complexo

### FD-004: Suporte Windows

**Status**: Planejado para v2.0  
**Opção**: Adaptar para WMI do Windows  
**Impacto**: Mudança arquitetônica

---

## 8. Revisão e Aprovação

| Papel     | Nome | Data       | Assinatura |
| --------- | ---- | ---------- | ---------- |
| Tech Lead | -    | 2026-08-16 | ✓          |
| Architect | -    | 2026-08-16 | ✓          |
| QA Lead   | -    | 2026-08-16 | ✓          |

---

## 9. Histórico de Mudanças

| Versão | Data       | Mudança           | Autor     |
| ------ | ---------- | ----------------- | --------- |
| 1.0    | 2026-08-16 | Documento inicial | Tech Lead |

---

## 10. Referências

- [Arquitetura](arquitetura.md)
- [Requerimentos](requerimentos.md)
- [Prioridades SDLC](prioridades-sdlc.md)
- [The C Programming Language, 2nd Ed.](https://en.wikipedia.org/wiki/The_C_Programming_Language)
- [Linux Kernel Documentation - /proc](https://www.kernel.org/doc/html/latest/filesystems/proc.html)
