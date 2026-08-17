# Requerimentos - CPU Reader

## 1. Requerimentos Funcionais (RF)

### RF-001: Leitura de Informações da CPU

**Descrição**: O sistema deve ser capaz de ler informações estáticas da CPU a partir do sistema operacional.

**Detalhes**:

- Número de cores de processamento
- Número de threads lógicas
- Modelo e fabricante da CPU
- Frequência nominal em MHz
- Cache disponível
- Flags de capacidade do processador

**Prioridade**: Alta  
**Status**: Requerido para MVP  
**Aceitação**: Dados devem ser precisos e validados

---

### RF-002: Monitoramento de Uso de CPU

**Descrição**: O sistema deve calcular a porcentagem de utilização da CPU em tempo real.

**Detalhes**:

- Leitura periódica de `/proc/stat`
- Cálculo de delta entre leituras
- Porcentagem individual por core
- Porcentagem agregada do sistema
- Atualização não-bloqueante

**Prioridade**: Alta  
**Status**: Requerido para MVP  
**Aceitação**: Valor entre 0-100%, atualizado em tempo real

---

### RF-003: Leitura de Temperatura

**Descrição**: O sistema deve ler a temperatura da CPU quando disponível no hardware.

**Detalhes**:

- Acesso a sensores térmicos (`/proc/thermal_zone`)
- Suporte a múltiplos sensores
- Retorno em Celsius
- Graceful fallback se sensor indisponível

**Prioridade**: Média  
**Status**: Requerido para v1.0  
**Aceitação**: Temperatura precisa quando hardware disponível

---

### RF-004: Gerenciamento de Memória

**Descrição**: O sistema deve gerenciar alocação e desalocação de memória de forma segura.

**Detalhes**:

- Alocação dinâmica de estruturas
- Inicialização e limpeza adequada
- Sem vazamento de memória
- Validação de ponteiros

**Prioridade**: Alta  
**Status**: Requerido para MVP  
**Aceitação**: Zero vazamentos detectados com valgrind

---

### RF-005: Tratamento de Erros

**Descrição**: O sistema deve tratar erros de forma robusta e previsível.

**Detalhes**:

- Validação de entrada
- Retorno de código de erro
- Mensagens de erro descritivas
- Recuperação de falhas

**Prioridade**: Alta  
**Status**: Requerido para MVP  
**Aceitação**: Sem crashes, sempre retorna erro válido

---

### RF-006: Interface C Simples

**Descrição**: Fornecer uma API em C fácil de usar e bem documentada.

**Detalhes**:

- Máximo 10 funções públicas
- Nomes intuitivos (cpu*get*\*)
- Documentação com exemplos
- Compatibilidade com C99+

**Prioridade**: Alta  
**Status**: Requerido para MVP  
**Aceitação**: API consistente e documentada

---

### RF-007: Suporte a Múltiplas Arquiteturas

**Descrição**: O sistema deve funcionar em diferentes arquiteturas de processador.

**Detalhes**:

- Suporte x86/x86_64
- Suporte ARM/ARM64
- Suporte RISC-V (futuro)
- Detecção automática de arquitetura

**Prioridade**: Média  
**Status**: Requerido para v1.0  
**Aceitação**: Funciona em pelo menos 2 arquiteturas

---

### RF-008: Logging e Debug

**Descrição**: O sistema deve fornecer capacidades de logging para troubleshooting.

**Detalhes**:

- Níveis de log (INFO, DEBUG, ERROR)
- Saída para stderr
- Timestamp em logs
- Dados detalhados de falha

**Prioridade**: Baixa  
**Status**: Requerido para v1.1  
**Aceitação**: Logs úteis para debug

---

## 2. Requerimentos Não-Funcionais (RNF)

### RNF-001: Performance

**Descrição**: O sistema deve operar com latência e throughput aceitáveis.

**Especificação**:

- Tempo de resposta `cpu_get_info()`: < 10ms
- Tempo de resposta `cpu_get_usage()`: < 50ms
- Tempo de resposta `cpu_get_temperature()`: < 20ms
- CPU utilizada para leitura: < 1%

**Prioridade**: Alta  
**Status**: Requerido para MVP  
**Teste**: Benchmark com chrono.h

---

### RNF-002: Confiabilidade

**Descrição**: O sistema deve ser confiável e resiliente.

**Especificação**:

- Uptime: 99.9% em operação contínua
- Recuperação de erros: 100%
- Sem race conditions
- Sem memory leaks

**Prioridade**: Alta  
**Status**: Requerido para MVP  
**Teste**: Stress test, valgrind, thread sanitizer

---

### RNF-003: Portabilidade

**Descrição**: O sistema deve ser portável entre sistemas Linux.

**Especificação**:

- Compatível com Linux 4.4+
- Sem dependências externas
- Compilação com GCC e Clang
- Funciona em x86, ARM, RISC-V

**Prioridade**: Alta  
**Status**: Requerido para MVP  
**Teste**: Compilação em múltiplas plataformas

---

### RNF-004: Segurança

**Descrição**: O sistema deve ser seguro contra vulnerabilidades comuns.

**Especificação**:

- Sem buffer overflows
- Validação de entrada
- Sem injeção de código
- Acesso seguro a /proc

**Prioridade**: Alta  
**Status**: Requerido para MVP  
**Teste**: SAST, fuzzing, code review

---

### RNF-005: Manutenibilidade

**Descrição**: O sistema deve ser fácil de manter e estender.

**Especificação**:

- Código bem documentado
- Função: máximo 50 linhas
- Complexidade ciclomática: máximo 10
- Cobertura de testes: mínimo 80%

**Prioridade**: Média  
**Status**: Requerido para v1.0  
**Teste**: Complexity metrics, code review

---

### RNF-006: Usabilidade

**Descrição**: O sistema deve ser fácil de usar.

**Especificação**:

- API intuitiva com < 10 funções
- Exemplos completos fornecidos
- Mensagens de erro claras
- Documentação com 5+ exemplos

**Prioridade**: Média  
**Status**: Requerido para MVP  
**Teste**: Usuários externos testam

---

### RNF-007: Escalabilidade

**Descrição**: O sistema deve funcionar em diferentes escalas de hardware.

**Especificação**:

- Funciona com 1 core
- Funciona com 512+ cores
- Memória alocada: < 1MB
- Tamanho binário: < 100KB

**Prioridade**: Média  
**Status**: Requerido para v1.0  
**Teste**: Testes em múltiplos tamanhos de CPU

---

### RNF-008: Compatibilidade

**Descrição**: O sistema deve ser compatível com diferentes versões.

**Especificação**:

- C99 ou superior
- GCC 5.0+, Clang 3.5+
- Linux 4.4+
- Sem breaking changes entre minor versions

**Prioridade**: Média  
**Status**: Requerido para v1.0  
**Teste**: CI/CD com múltiplas versões

---

## 3. Requerimentos de Dados (RD)

### RD-001: Estrutura de Dados Principal

**Descrição**: Definir estrutura de dados para informações da CPU.

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

---

### RD-002: Formato de Leitura

**Descrição**: Dados lidos de `/proc/cpuinfo` em formato específico.

**Campos obrigatórios**:

- processor
- vendor_id
- model name
- cpu cores
- flags

---

### RD-003: Validação de Dados

**Descrição**: Validar integridade de dados lidos.

**Regras**:

- cores > 0 e cores < 1024
- threads > 0 e threads < 4096
- frequency_mhz > 0 e < 10000
- usage_percent >= 0 e <= 100
- temperature_c entre -50 e 150

---

## 4. Requerimentos de Interface (RI)

### RI-001: API Pública

**Descrição**: Definir funções públicas da biblioteca.

**Funções obrigatórias**:

- `cpu_info_t* cpu_get_info()`
- `float cpu_get_usage()`
- `float cpu_get_temperature()`
- `void cpu_free_info(cpu_info_t*)`
- `int cpu_init()`
- `void cpu_cleanup()`

---

### RI-002: Códigos de Erro

**Descrição**: Definir códigos de retorno para erros.

**Códigos**:

- 0: Sucesso
- -1: Erro genérico
- -2: Erro de memória
- -3: Erro de permissão
- -4: Sensor indisponível

---

## 5. Requerimentos de Documentação (RDoc)

### RDoc-001: Documentação de Código

**Descrição**: Cada função deve ter documentação completa.

**Conteúdo obrigatório**:

- Descrição da função
- Parâmetros (se houver)
- Valor de retorno
- Exemplo de uso
- Possíveis erros

---

### RDoc-002: README Completo

**Descrição**: Documento principal com overview do projeto.

**Seções obrigatórias**:

- Objetivos
- O que resolve
- Sobre o projeto
- Tecnologias
- Estrutura
- Quick start
- Licença

---

## 6. Matriz de Rastreabilidade

| ID Requisito | Tipo           | Prioridade | Status | Teste       | Documento           |
| ------------ | -------------- | ---------- | ------ | ----------- | ------------------- |
| RF-001       | Funcional      | Alta       | MVP    | Unit Test   | arquitetura.md      |
| RF-002       | Funcional      | Alta       | MVP    | Unit Test   | casosdeuso.md       |
| RF-003       | Funcional      | Média      | v1.0   | Unit Test   | criterios.md        |
| RNF-001      | Performance    | Alta       | MVP    | Benchmark   | prioridades-sdlc.md |
| RNF-002      | Confiabilidade | Alta       | MVP    | Stress Test | prioridades-sdlc.md |
| RNF-003      | Portabilidade  | Alta       | MVP    | CI/CD       | prioridades-sdlc.md |
| RNF-004      | Segurança      | Alta       | MVP    | SAST        | prioridades-sdlc.md |
