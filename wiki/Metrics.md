# Métricas e limitações

| Métrica | Fonte | Unidade/semântica | Limitação |
|---|---|---|---|
| Uso agregado | <code>/proc/stat</code> | percentual 0–100 | depende de amostras sucessivas |
| Modelo | <code>/proc/cpuinfo</code> | texto | formato varia por arquitetura |
| Núcleos físicos | campos de topologia disponíveis | contagem estimada | pode ser desconhecida |
| Processadores lógicos | processadores visíveis no sistema | contagem | pode refletir limites do ambiente |
| Temperatura | interfaces térmicas em <code>/sys</code> | °C | sensor/driver opcional |
| Frequência | sysfs, com fallback documentado | MHz | política de frequência pode variar |
| Processos em execução | <code>/proc/loadavg</code> | contagem em execução | snapshot momentâneo |

## Interpretação correta

Uma métrica indisponível não equivale a zero. Os retornos negativos indicam ausência ou falha conforme a função. A frequência pode variar rapidamente por gerenciamento dinâmico de energia. A temperatura apresentada depende do sensor escolhido pelo kernel e pode não representar todos os componentes do processador.

Em containers, VMs ou ambientes com namespaces, as informações visíveis podem refletir a visão do kernel exposta ao processo e não necessariamente o hardware físico completo.
