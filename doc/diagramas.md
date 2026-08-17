# Diagramas - CPU Reader

## 1. Diagrama de Componentes

```
┌─────────────────────────────────────────────────────────┐
│                   Aplicação do Usuário                   │
└────────────────────┬────────────────────────────────────┘
                     │
                     ├─ cpu_get_info()
                     ├─ cpu_get_usage()
                     └─ cpu_get_temperature()
                     │
┌────────────────────▼────────────────────────────────────┐
│              CPU Reader Library (libcpu)                 │
├─────────────────────────────────────────────────────────┤
│                                                           │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │   Parser     │  │  Calculator  │  │   Cache      │  │
│  │              │  │              │  │  Manager     │  │
│  │ - Parse Info │  │ - CPU Usage  │  │ - Store Data │  │
│  │ - Parse Stat │  │ - Frequency  │  │ - Validate   │  │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘  │
│         │                 │                  │           │
│         └─────────────────┼──────────────────┘           │
│                           │                              │
│                    ┌──────▼─────────┐                    │
│                    │  Data Store    │                    │
│                    │   (Memory)     │                    │
│                    └────────────────┘                    │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│         Linux System Interface (/proc)                   │
├─────────────────────────────────────────────────────────┤
│  /proc/cpuinfo  │  /proc/stat  │  /proc/thermal  │ ...  │
└─────────────────────────────────────────────────────────┘
```

## 2. Diagrama de Sequência - Leitura de Informações

```
Usuário          Aplicação       CPU Reader      Sistema
   │                 │                │              │
   │─ Requisita      │                │              │
   │  cpu_get_info() │                │              │
   │                 ├─ Chama        │              │
   │                 │  cpu_info()   │              │
   │                 │                ├─ Abre       │
   │                 │                │ /proc/cpuinfo
   │                 │                ├─ Lê dados   │
   │                 │                │◄─────────────┤
   │                 │                │              │
   │                 │                ├─ Parse       │
   │                 │                │              │
   │                 │                ├─ Aloca       │
   │                 │                │ Memória      │
   │                 │                │              │
   │                 │                ├─ Popula      │
   │                 │                │ Estrutura    │
   │                 │                │              │
   │                 │◄─ Retorna      │              │
   │                 │ cpu_info_t*    │              │
   │◄─ Recebe        │                │              │
   │  Informações    │                │              │
   │                 │                │              │
```

## 3. Diagrama de Fluxo - Cálculo de Uso de CPU

```
                    ┌─────────────────┐
                    │  cpu_get_usage()│
                    └────────┬────────┘
                             │
                      ┌──────▼─────────┐
                      │ Abrir /proc/stat
                      └────────┬────────┘
                             │
                      ┌──────▼─────────────────┐
                      │ Ler user, nice, system,
                      │ idle, iowait, irq, etc.
                      └────────┬────────────────┘
                             │
                      ┌──────▼─────────────────┐
                      │ Calcular delta desde   │
                      │ última leitura         │
                      └────────┬────────────────┘
                             │
                      ┌──────▼──────────────────────┐
                      │ total = user + nice +       │
                      │ system + irq + softirq +    │
                      │ idle + iowait               │
                      └────────┬───────────────────┘
                             │
                      ┌──────▼──────────────────────┐
                      │ trabalho = total - idle     │
                      └────────┬───────────────────┘
                             │
                      ┌──────▼──────────────────────┐
                      │ percentual = (trabalho /    │
                      │ total) * 100                │
                      └────────┬───────────────────┘
                             │
                      ┌──────▼────────────────┐
                      │ Armazenar cache       │
                      └────────┬─────────────┘
                             │
                      ┌──────▼──────────────┐
                      │ Retornar percentual │
                      └──────────────────────┘
```

## 4. Diagrama de Estado - Ciclo de Vida

```
     ┌─────────────────────────┐
     │   Não Inicializado      │
     └────────────┬────────────┘
                  │ cpu_init()
     ┌────────────▼────────────┐
     │   Inicializando         │
     │  (Alocando Recursos)    │
     └────────────┬────────────┘
                  │
     ┌────────────▼────────────────────┐
     │   Pronto para Uso               │
     │  (Aceitando Requisições)        │
     └────────┬──────────────┬─────────┘
              │              │
              │              │ Erro
    ┌─────────▼──────┐   ┌───▼─────────┐
    │ Em Operação    │   │   Erro      │
    │ (Lendo Dados)  │   │  (Falha)    │
    └─────────┬──────┘   └───┬─────────┘
              │               │
              │     ┌─────────┤
              │     │
    ┌─────────▼─────▼──┐
    │  cpu_cleanup()   │
    └────────┬─────────┘
             │
    ┌────────▼──────────┐
    │  Finalizado       │
    │  (Recursos       │
    │   Liberados)     │
    └───────────────────┘
```

## 5. Diagrama de Arquitetura em Camadas

```
┌─────────────────────────────────────────────────┐
│     CAMADA DE APLICAÇÃO                         │
│  ┌────────────────────────────────────────────┐ │
│  │  Aplicações do Usuário                     │ │
│  │  (Monitoramento, Dashboard, etc.)          │ │
│  └────────────────────────────────────────────┘ │
└────────────────────┬────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────┐
│     CAMADA DE API                               │
│  ┌────────────────────────────────────────────┐ │
│  │  Interface Pública (include/cpu.h)         │ │
│  │  - cpu_get_info()                          │ │
│  │  - cpu_get_usage()                         │ │
│  │  - cpu_get_temperature()                   │ │
│  └────────────────────────────────────────────┘ │
└────────────────────┬────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────┐
│     CAMADA DE LÓGICA                            │
│  ┌────────────────────────────────────────────┐ │
│  │  Implementação (src/cpu.c)                 │ │
│  │  - Parser de dados                         │ │
│  │  - Calculadora de métricas                 │ │
│  │  - Gerenciador de cache                    │ │
│  │  - Validador de dados                      │ │
│  └────────────────────────────────────────────┘ │
└────────────────────┬────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────┐
│     CAMADA DE SISTEMA                           │
│  ┌────────────────────────────────────────────┐ │
│  │  Sistema de Arquivos (/proc)               │ │
│  │  - /proc/cpuinfo                           │ │
│  │  - /proc/stat                              │ │
│  │  - /proc/thermal_zone                      │ │
│  └────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────┘
```

## 6. Diagrama de Estrutura de Dados

```
cpu_info_t
├── cores: int
│   └─ Número de núcleos de processamento
│
├── threads: int
│   └─ Número total de threads lógicas
│
├── frequency_mhz: float
│   └─ Frequência nominal em MHz
│
├── usage_percent: float
│   └─ Porcentagem de utilização (0-100%)
│
├── temperature_c: float
│   └─ Temperatura em Celsius
│
├── model[256]: char[]
│   └─ Descrição/modelo da CPU
│       Ex: "Intel Core i7-9700K"
│
└── flags[512]: char[]
    └─ Flags de capacidade da CPU
        Ex: "fpu vme de pse tsc msr pae mce cx8 apic..."
```

## 7. Diagrama de Comunicação com Sistema

```
CPU Reader
    │
    ├─────────────────────────────┐
    │                             │
    ▼                             ▼
[/proc/cpuinfo]         [/proc/stat]
│                       │
├─ processor           ├─ cpu  user nice system idle iowait
├─ vendor_id           ├─ cpu0 ...
├─ model               ├─ cpu1 ...
├─ model name          └─ ...
├─ stepping
├─ microcode           [/proc/thermal_zone/]
├─ cpu MHz             │
├─ cache size          └─ thermal_zone0/
├─ physical id           └─ temp
├─ siblings
├─ core id
├─ cpu cores
├─ apicid
├─ flags
└─ ...
```
