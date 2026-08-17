# Arquitetura do CPU Reader

## Visão Geral

O CPU Reader é uma biblioteca C que segue uma arquitetura modular e em camadas, permitindo fácil manutenção e extensão.

## Camadas da Arquitetura

### 1. Camada de Interface Pública (API)

- Localização: `include/cpu.h`
- Responsabilidade: Expor as funções públicas da biblioteca
- Exemplo de funções:
  - `cpu_info_t* cpu_get_info()`: Obtém informações gerais da CPU
  - `float cpu_get_usage()`: Retorna o uso atual da CPU
  - `float cpu_get_temperature()`: Retorna a temperatura da CPU

### 2. Camada de Implementação

- Localização: `src/cpu.c`
- Responsabilidade: Implementar a lógica de leitura e processamento de dados
- Componentes:
  - Leitura do sistema de arquivos `/proc/cpuinfo`
  - Leitura de dados de desempenho
  - Parsing e processamento de informações
  - Cálculo de métricas

### 3. Camada de Sistema Operacional

- Responsabilidade: Interagir com o sistema operacional
- Interface com `/proc` no Linux
- Chamadas de sistema (syscalls)
- Gerenciamento de recursos

## Estrutura de Dados

### Estrutura Principal: `cpu_info_t`

```c
typedef struct {
    int cores;              // Número de cores
    int threads;            // Número de threads
    float frequency_mhz;    // Frequência em MHz
    float usage_percent;    // Uso em porcentagem
    float temperature_c;    // Temperatura em graus Celsius
    char model[256];        // Modelo da CPU
    char flags[512];        // Flags de suporte
} cpu_info_t;
```

## Fluxo de Dados

```
┌─────────────────────┐
│  Aplicação do       │
│  Usuário            │
└──────────┬──────────┘
           │
           ↓
┌─────────────────────────┐
│  API Pública            │
│  (cpu.h)                │
└──────────┬──────────────┘
           │
           ↓
┌──────────────────────────┐
│  Implementação           │
│  (cpu.c)                 │
│  - Parsing               │
│  - Processamento         │
│  - Cálculos              │
└──────────┬───────────────┘
           │
           ↓
┌──────────────────────────┐
│  Sistema de Arquivos     │
│  /proc/cpuinfo           │
│  /proc/stat              │
└──────────────────────────┘
```

## Padrões de Design

### Padrão Factory

- `cpu_get_info()`: Cria e retorna uma instância de `cpu_info_t`

### Padrão Singleton

- Gerenciador de cache de informações da CPU para evitar leituras desnecessárias

### Padrão Strategy

- Diferentes estratégias de leitura para diferentes versões do Linux

## Dependências

- **libc**: Biblioteca C padrão
- **stdlib.h**: Funções de utilidade
- **stdio.h**: I/O de arquivos
- **string.h**: Manipulação de strings

## Ciclo de Vida de Objetos

1. **Alocação**: `malloc()` para dados dinâmicos
2. **Inicialização**: Preenchimento com dados do sistema
3. **Uso**: Acesso pelas funções de API
4. **Limpeza**: `free()` para liberar memória

## Tratamento de Erros

- Retorno de NULL ou valores inválidos (-1) em caso de erro
- Mensagens de erro padronizadas
- Logging de erros para debug
