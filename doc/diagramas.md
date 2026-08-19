# Diagramas do CPU Reader

## Componentes

```text
+---------------------+
| Aplicacao / teste   |
+----------+----------+
           |
           v
+---------------------+       +----------------+
| include/cpu.h       |------>| libcpu.a       |
| API publica         |       | src/cpu.c      |
+---------------------+       +-------+--------+
                                      |
                         +------------+------------+
                         |                         |
                         v                         v
                  /proc/cpuinfo              /proc/stat
                  informacoes                contadores CPU
```

O monitor (`examples/monitor.c`) usa a mesma biblioteca e acrescenta ncurses apenas no executável.

## Leitura de informações

```text
cpu_get_info()
      |
      v
abre /proc/cpuinfo
      |
      v
calloc(cpu_info_t)
      |
      v
processa processor, model name,
cpu MHz e flags
      |
      +--> retorna cpu_info_t*
      |
      `--> em falha: fecha arquivo e retorna NULL
```

## Cálculo de uso

```text
cpu_get_usage()
      |
      v
lê a linha "cpu" de /proc/stat
      |
      v
soma contadores atuais
      |
      v
calcula delta contra a leitura anterior
      |
      v
uso = (delta_total - delta_idle) / delta_total * 100
      |
      `--> retorna 0..100 ou -1.0f em erro
```

Os contadores anteriores são estáticos e compartilhados pela biblioteca. `cpu_init()` e `cpu_cleanup()` zeram esse estado.
