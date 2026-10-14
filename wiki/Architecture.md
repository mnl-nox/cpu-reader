# Arquitetura

## Camadas

1. **Consumidores:** aplicações C e os frontends de exemplo.
2. **API pública:** declarações e tipos em <code>include/cpu.h</code>.
3. **Fachada e estado:** <code>src/cpu.c</code>.
4. **Implementações especializadas:** informações da CPU, telemetria e cálculo de uso em arquivos separados.
5. **Interfaces do kernel:** <code>/proc</code> e <code>/sys</code>.

## Módulos

- <code>src/cpu_info.c</code>: parsing de informações e topologia.
- <code>src/cpu_usage.c</code>: cálculo de uso agregado a partir de contadores do kernel.
- <code>src/cpu_telemetry.c</code>: temperatura, clock e load average.
- <code>src/cpu_internal.h</code>: contratos internos.
- <code>examples/monitor.c</code>: interface ncurses.
- <code>examples/gtk_monitor.c</code>: interface GTK4.

## Princípios

- O núcleo não depende de interface gráfica.
- Métricas de sensores são opcionais.
- Caminhos de arquivos alternativos facilitam testes com fixtures.
- A API reporta falhas por valores de retorno documentados.
- O código segue C99.

Para decisões históricas, consulte [ADRs](../doc/architecture-decision-records.md) e [diagramas UML](../doc/uml-diagrams.md).
