# Diagramas UML - CPU Reader

**Versão:** 0.1.1
**Data:** 2026-09-04  
**Formato:** Mermaid

---

## 1. Diagrama de Classes

```mermaid
classDiagram
    class cpu_error_t {
        <<enumeration>>
        CPU_ERROR_NONE
        CPU_ERROR_INVALID_ARGUMENT
        CPU_ERROR_FILE_OPEN
        CPU_ERROR_PARSE
        CPU_ERROR_MEMORY
        CPU_ERROR_UNSUPPORTED
    }

    class cpu_info_t {
        <<structure>>
        -cores: int
        -threads: int
        -active_processes: int
        -frequency_mhz: float
        -usage_percent: float
        -temperature_c: float
        -model[256]: char
        -flags[512]: char
    }

    class cpu_usage_context_t {
        <<structure>>
        -previous_total: unsigned long long
        -previous_idle: unsigned long long
        -has_previous: int
    }

    class CPULibrary {
        <<module>>
        -default_usage_context: cpu_usage_context_t
        -last_error_code: cpu_error_t
        -last_error_message[256]: char

        +cpu_init(): int
        +cpu_cleanup(): void
        +cpu_get_info(): cpu_info_t*
        +cpu_free_info(info: cpu_info_t*): void
        +cpu_get_usage(): float
        +cpu_get_temperature(): float
        +cpu_get_clock_speed(): float
        +cpu_get_active_processes(): int
        +cpu_usage_context_init(ctx: cpu_usage_context_t*): int
        +cpu_usage_context_cleanup(ctx: cpu_usage_context_t*): void
        +cpu_get_usage_context(ctx: cpu_usage_context_t*): float
        +cpu_get_last_error_code(): cpu_error_t
        +cpu_get_last_error(): const char*
    }

    class CPUInfoModule {
        <<module>>
        +cpu_read_info(): cpu_info_t*
        -cpu_parse_cpuinfo(file: FILE*, info: cpu_info_t*): void
        +cpu_read_temperature(retry: int): float
        +cpu_read_clock_speed(retry: int): float
        +cpu_read_active_processes(retry: int): int
    }

    class CPUUsageModule {
        <<module>>
        +cpu_get_usage_context(ctx: cpu_usage_context_t*): float
        -cpu_read_stat_line(counters: unsigned long long*): void
        #ifdef __x86_64__
        -cpu_sum_counters_asm(counters: unsigned long long*): unsigned long long
        #else
        -cpu_sum_counters_c(counters: unsigned long long*): unsigned long long
        #endif
    }

    class CPUInternalModule {
        <<module>>
        +cpu_clear_last_error(): void
        +cpu_set_last_error(code: cpu_error_t, format: const char*, ...): void
    }

    CPULibrary --> cpu_error_t : uses
    CPULibrary --> cpu_info_t : returns
    CPULibrary --> cpu_usage_context_t : uses
    CPULibrary --> CPUInfoModule : calls
    CPULibrary --> CPUUsageModule : calls
    CPULibrary --> CPUInternalModule : calls
    CPUInfoModule --> cpu_info_t : returns
    CPUUsageModule --> cpu_usage_context_t : uses
```

---

## 2. Diagrama de Componentes

```mermaid
graph TB
    subgraph Aplicacao["Aplicação do Usuário"]
        APP["Código da Aplicação<br/>main(), chamadas de API"]
    end

    subgraph LibCPU["libcpu.a - Biblioteca CPU Reader"]
        API["include/cpu.h<br/>API Pública"]
        CORE["src/cpu.c<br/>Fachada & Ciclo de Vida"]
        INFO["src/cpu_info.c<br/>Leitura de Informações"]
        USAGE["src/cpu_usage.c<br/>Cálculo de Uso"]
        INTERNAL["src/cpu_internal.h<br/>Funções Internas"]
    end

    subgraph PROC["Pseudo-filesystems Linux"]
        CPUINFO["/proc/cpuinfo<br/>Informações CPU"]
        STAT["/proc/stat<br/>Contadores CPU"]
        LOADAVG["/proc/loadavg<br/>Processos ativos"]
        SYSFS["/sys/...<br/>Temperatura & Freq"]
    end

    subgraph Monitor["examples/monitor.c<br/>Aplicação de Exemplo"]
        NCURSES["ncurses<br/>Interface Terminal"]
    end

    APP -->|#include| API
    APP -->|linking| CORE

    CORE -->|coordena| INFO
    CORE -->|coordena| USAGE
    CORE -->|usa| INTERNAL

    INFO -->|lê| CPUINFO
    INFO -->|lê| LOADAVG
    INFO -->|lê| SYSFS

    USAGE -->|lê| STAT

    Monitor -->|usa| API
    Monitor -->|linking| CORE
    Monitor -->|linking| NCURSES

    style LibCPU fill:#e1f5ff
    style PROC fill:#fff9c4
    style Monitor fill:#f3e5f5
    style Aplicacao fill:#c8e6c9
```

---

## 3. Diagrama de Sequência: Leitura de Informações

```mermaid
sequenceDiagram
    participant App as Aplicação
    participant API as cpu_get_info()<br/>API
    participant Info as cpu_info.c
    participant PROC as /proc/cpuinfo
    participant SYS as sysfs

    App->>API: cpu_get_info()
    activate API

    API->>Info: calloc(sizeof(cpu_info_t))
    activate Info

    Info->>PROC: fopen(path)
    activate PROC
    PROC-->>Info: FILE*
    deactivate PROC

    Info->>Info: parse_cpuinfo()<br/>cores, model, flags, frequency_mhz

    Info->>PROC: fclose()
    activate PROC
    deactivate PROC

    Info->>SYS: read temperature<br/>(optional)
    activate SYS
    SYS-->>Info: temperature_c or -1.0f
    deactivate SYS

    Info->>SYS: read clock speed<br/>(optional)
    activate SYS
    SYS-->>Info: frequency or -1.0f
    deactivate SYS

    Info-->>API: cpu_info_t*
    deactivate Info

    API-->>App: cpu_info_t*
    deactivate API

    Note over App,API: Em caso de erro:<br/>cpu_get_last_error_code()<br/>retorna código do erro
```

---

## 4. Diagrama de Sequência: Cálculo de Uso

```mermaid
sequenceDiagram
    participant App as Aplicação
    participant API as cpu_get_usage()<br/>API
    participant Usage as cpu_usage.c
    participant PROC as /proc/stat
    participant CTX as cpu_usage_context_t

    App->>API: cpu_init()
    activate API
    API->>Usage: context_init()
    activate Usage
    Usage->>CTX: has_previous = 0
    deactivate Usage
    deactivate API

    App->>API: cpu_get_usage()<br/>1ª vez
    activate API
    API->>Usage: cpu_get_usage_context(default_context)
    activate Usage

    Usage->>PROC: read line "cpu"
    activate PROC
    PROC-->>Usage: user, nice, system, ..., idle, iowait
    deactivate PROC

    Usage->>CTX: store total, idle, has_previous=1
    Usage-->>API: 0.0f (primeira leitura)
    deactivate Usage
    deactivate API

    Note over App: sleep(100ms)

    App->>API: cpu_get_usage()<br/>2ª vez
    activate API
    API->>Usage: cpu_get_usage_context(default_context)
    activate Usage

    Usage->>PROC: read line "cpu"
    activate PROC
    PROC-->>Usage: user, nice, system, ..., idle, iowait
    deactivate PROC

    Usage->>Usage: delta_total = total2 - total1<br/>delta_idle = idle2 - idle1<br/>usage = (delta_total - delta_idle) / delta_total * 100
    Usage->>CTX: store new total, idle
    Usage-->>API: 25.5f (exemplo)
    deactivate Usage
    deactivate API

    App->>API: cpu_cleanup()
    activate API
    API->>Usage: context_cleanup()
    activate Usage
    Usage->>CTX: has_previous = 0
    deactivate Usage
    deactivate API
```

---

## 5. Diagrama de Sequência: Tratamento de Erros

```mermaid
sequenceDiagram
    participant App as Aplicação
    participant API as API pública
    participant Core as cpu.c

    App->>API: cpu_get_info()

    Note over API,Core: Operação falha<br/>(arquivo não encontrado)

    API->>Core: cpu_set_last_error(CPU_ERROR_FILE_OPEN, ...)
    activate Core
    Core->>Core: last_error_code = CPU_ERROR_FILE_OPEN
    Core->>Core: last_error_message = "Could not open /proc/cpuinfo"
    deactivate Core

    API-->>App: NULL (erro)

    Note over App: Aplicação detecta NULL

    App->>API: cpu_get_last_error_code()
    API-->>App: CPU_ERROR_FILE_OPEN (código do erro)

    App->>API: cpu_get_last_error()
    API-->>App: "Could not open /proc/cpuinfo" (mensagem)

    Note over App: Aplicação trata erro<br/>e pode decidir action

    App->>API: cpu_get_info()<br/>retry com outro caminho

    Note over API,Core: Operação sucesso

    API->>Core: cpu_clear_last_error()
    activate Core
    Core->>Core: last_error_code = CPU_ERROR_NONE
    Core->>Core: last_error_message = ""
    deactivate Core

    API-->>App: cpu_info_t* (sucesso)
```

---

## 6. Diagrama de Estado: Contexto de Uso

```mermaid
stateDiagram-v2
    [*] --> UNINIT: cpu_usage_context_init()

    UNINIT --> FIRST_READ: cpu_get_usage_context()

    FIRST_READ --> READY: Armazena contadores
    note right of FIRST_READ
        Retorna 0.0f
        has_previous = 1
    end note

    READY --> CALCULATE: cpu_get_usage_context()

    CALCULATE --> READY: Calcula delta, retorna %
    note right of CALCULATE
        Compara com leitura anterior
        Retorna 0-100%
    end note

    CALCULATE --> CALCULATE: Múltiplas chamadas

    READY --> CLEAN: cpu_usage_context_cleanup()
    CALCULATE --> CLEAN: cpu_usage_context_cleanup()

    CLEAN --> [*]
    note right of CLEAN
        has_previous = 0
        Contadores zerados
    end note
```

---

## 7. Diagrama de Fluxo: Inicialização da Biblioteca

```mermaid
graph TD
    A["cpu_init()"] --> B{Contexto<br/>padrão<br/>válido?}
    B -->|Não| C["cpu_usage_context_init<br/>default"]
    B -->|Sim| D["Retorna 0<br/>sucesso"]
    C --> E{Alocação<br/>OK?}
    E -->|Sim| D
    E -->|Não| F["cpu_set_last_error<br/>CPU_ERROR_MEMORY"]
    F --> G["Retorna -1<br/>erro"]
    D --> H["Estado pronto<br/>para usar"]
    G --> H
```

---

## 8. Diagrama de Fluxo: Obter Uso

```mermaid
graph TD
    A["cpu_get_usage()"] --> B["cpu_get_usage_context<br/>default_context"]
    B --> C["Abrir /proc/stat"]
    C --> D{Arquivo<br/>aberto?}
    D -->|Não| E["cpu_set_last_error<br/>FILE_OPEN"]
    E --> F["Retorna -1.0f"]
    D -->|Sim| G["Ler linha 'cpu'"]
    G --> H{Parse<br/>OK?}
    H -->|Não| I["cpu_set_last_error<br/>PARSE"]
    I --> F
    H -->|Sim| J{has_previous?}
    J -->|Não| K["Armazena contadores<br/>has_previous = 1"]
    K --> L["Retorna 0.0f<br/>1ª leitura"]
    J -->|Sim| M["Calcula delta_total<br/>delta_idle"]
    M --> N["uso = delta_busy<br/>/ delta_total * 100"]
    N --> O["Armazena novos<br/>contadores"]
    O --> P["cpu_clear_last_error"]
    P --> Q["Retorna uso %<br/>0-100"]
    L --> R["Pronto"]
    Q --> R
    F --> R
```

---

## 9. Matriz de Responsabilidades (RACI)

| Componente         | Iniciar | Ler Info | Calcular Uso | Temperatura | Relatar Erro |
| ------------------ | ------- | -------- | ------------ | ----------- | ------------ |
| **cpu.c**          | R/A     | C        | C            | C           | R/A          |
| **cpu_info.c**     | -       | R/A      | -            | C           | -            |
| **cpu_usage.c**    | -       | -        | R/A          | -           | -            |
| **cpu_internal.h** | -       | -        | -            | -           | R/A          |

**Legenda:** R=Responsável, A=Accountable, C=Consultado

---

## 10. Mapa de Testes por Componente

```mermaid
graph LR
    TC["Test CPU"]

    TC --> T1["test_init_cleanup"]
    TC --> T2["test_get_info"]
    TC --> T3["test_get_usage"]
    TC --> T4["test_get_temperature"]
    TC --> T5["test_get_clock_speed"]
    TC --> T6["test_get_active_processes"]
    TC --> T7["test_error_handling"]
    TC --> T8["test_context_independence"]

    T1 --> CPU_C["cpu.c"]
    T2 --> CPU_INFO["cpu_info.c"]
    T3 --> CPU_USAGE["cpu_usage.c"]
    T4 --> CPU_INFO
    T5 --> CPU_INFO
    T6 --> CPU_INFO
    T7 --> CPU_INTERNAL["cpu_internal.h"]
    T8 --> CPU_USAGE

    style CPU_C fill:#e1f5ff
    style CPU_INFO fill:#e1f5ff
    style CPU_USAGE fill:#e1f5ff
    style CPU_INTERNAL fill:#e1f5ff
```

---

## 11. Hierarquia de Chamadas (Call Graph)

```
cpu_init()
├── cpu_usage_context_init()
│   └── calloc()
└── return 0

cpu_get_info()
├── calloc(cpu_info_t)
├── fopen(path_from_env or /proc/cpuinfo)
├── cpu_parse_cpuinfo()
│   ├── fgets()
│   └── sscanf()
├── cpu_read_active_processes()
│   └── fopen(/proc/loadavg)
├── cpu_read_temperature()
│   └── fopen(sysfs_path)
├── cpu_read_clock_speed()
│   ├── fopen(sysfs_path)
│   └── [fallback] /proc/cpuinfo
├── sysconf(_SC_NPROCESSORS_ONLN)
├── fclose()
└── return cpu_info_t*

cpu_get_usage()
└── cpu_get_usage_context(&default_context)
    ├── fopen(path_from_env or /proc/stat)
    ├── fgets(line_cpu)
    ├── sscanf() ou cpu_sum_counters_asm/c()
    ├── fclose()
    ├── delta calculation
    └── return float

cpu_cleanup()
└── cpu_usage_context_cleanup()
    └── free(if needed)
```

---

## 12. Diagrama de Camadas

```mermaid
graph TB
    subgraph API["Camada de API"]
        API_FN["Funções Públicas<br/>cpu_get_info()<br/>cpu_get_usage()<br/>cpu_get_temperature()"]
    end

    subgraph CORE["Camada de Núcleo"]
        CORE_STATE["Gerenciamento de Estado<br/>default_context<br/>last_error"]
        CORE_COORD["Coordenação de Módulos"]
    end

    subgraph MODULES["Camada de Módulos"]
        MOD_INFO["Módulo Info<br/>cpu_info.c"]
        MOD_USAGE["Módulo Uso<br/>cpu_usage.c"]
        MOD_INTERNAL["Módulo Interno<br/>cpu_internal.h"]
    end

    subgraph DRIVERS["Camada de Drivers/I/O"]
        DRIVER_PROC["/proc Interface"]
        DRIVER_SYSFS["/sys Interface"]
    end

    subgraph KERNEL["Kernel Linux"]
        KDATA["Dados do Sistema<br/>/proc/cpuinfo<br/>/proc/stat<br/>/proc/loadavg<br/>sysfs"]
    end

    API_FN -->|chama| CORE_COORD
    CORE_COORD -->|coordena| MOD_INFO
    CORE_COORD -->|coordena| MOD_USAGE
    MOD_INFO -->|acessa| DRIVER_PROC
    MOD_INFO -->|acessa| DRIVER_SYSFS
    MOD_USAGE -->|acessa| DRIVER_PROC
    DRIVER_PROC -->|lê| KDATA
    DRIVER_SYSFS -->|lê| KDATA
    CORE_STATE -->|gerencia| CORE_COORD
    MOD_INTERNAL -->|suporta| MOD_INFO
    MOD_INTERNAL -->|suporta| MOD_USAGE

    style API fill:#c8e6c9
    style CORE fill:#bbdefb
    style MODULES fill:#fff9c4
    style DRIVERS fill:#f8bbd0
    style KERNEL fill:#ffe0b2
```

---

## 13. Árvore de Dependências

```
libcpu.a
├── include/cpu.h
│   ├── cpu_error_t (enum)
│   ├── cpu_info_t (struct)
│   ├── cpu_usage_context_t (struct)
│   └── 13 function declarations
│
├── src/cpu.c
│   ├── #include "cpu_internal.h"
│   ├── default_usage_context (static)
│   ├── last_error_code (static)
│   ├── last_error_message (static)
│   └── calls to cpu_usage_context_init/cleanup, etc.
│
├── src/cpu_info.c
│   ├── #include "cpu_internal.h"
│   ├── cpu_read_info()
│   ├── cpu_parse_cpuinfo()
│   ├── cpu_read_temperature()
│   ├── cpu_read_clock_speed()
│   └── cpu_read_active_processes()
│
├── src/cpu_usage.c
│   ├── #include "cpu_internal.h"
│   ├── cpu_get_usage_context()
│   ├── cpu_read_stat_line()
│   ├── #ifdef __x86_64__ cpu_sum_counters_asm()
│   └── cpu_sum_counters_c()
│
└── src/cpu_internal.h
    ├── cpu_clear_last_error()
    ├── cpu_set_last_error()
    ├── Function declarations
    └── Shared constants

Linux POSIX
├── <stdio.h> (fopen, fgets, fclose)
├── <stdlib.h> (malloc, free, calloc)
├── <string.h> (strlen, strcpy)
├── <unistd.h> (sysconf)
└── <sys/sysinfo.h> (optional)

examples/monitor.c
├── #include <cpu.h> (libcpu.a)
├── #include <ncurses.h> (optional)
└── Linux POSIX

tests/test_cpu.c
├── #include <cpu.h> (libcpu.a)
└── Linux POSIX
```

---

## 14. Visão Geral dos Diagramas

| Diagrama     | Tipo           | Foco               | Audiência          |
| ------------ | -------------- | ------------------ | ------------------ |
| Classes      | Estrutural     | Tipos e funções    | Desenvolvedores    |
| Componentes  | Estrutural     | Módulos e deps     | Arquitetos         |
| Seq. Leitura | Comportamental | Fluxo de I/O       | QA/Desenvolvedores |
| Seq. Uso     | Comportamental | Cálculo de delta   | Desenvolvedores    |
| Seq. Erro    | Comportamental | Tratamento de erro | QA/Desenvolvedores |
| Estado       | Comportamental | Ciclo de vida      | Arquitetos         |
| Fluxo Init   | Comportamental | Inicialização      | Desenvolvedores    |
| Fluxo Uso    | Comportamental | Decisões           | Desenvolvedores    |
| RACI         | Organizacional | Responsabilidades  | Gerência           |
| Testes       | Estrutural     | Cobertura          | QA                 |
| Call Graph   | Estrutural     | Hierarquia         | Desenvolvedores    |
| Camadas      | Arquitetural   | Estrutura          | Arquitetos         |
| Dependências | Estrutural     | Linkagem           | Build/DevOps       |

---

**Nota:** Todos os diagramas podem ser renderizados com Mermaid (markdown) ou ferramentas como PlantUML, Lucidchart, etc.
