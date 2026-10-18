# Arquitetura do CPU Reader

## Objetivo e limites

CPU Reader é uma biblioteca C99 para Linux que lê métricas do sistema em
`/proc` e `sysfs`, além de um monitor opcional em ncurses e uma interface desktop GTK4. O núcleo não depende
de ncurses, GTK4 nem de bibliotecas de interface. O projeto usa extensões de
armazenamento local à thread de GCC/Clang para manter compatibilidade com a API
legada enquanto permanece compilável em modo C99.

## Camadas e responsabilidades

- **Contrato público — `include/cpu.h`:** tipos, ownership de snapshots,
  contextos de amostragem e funções públicas.
- **Fachada e compatibilidade — `src/cpu.c`:** conversões numéricas estritas,
  wrappers públicos, estado de amostragem padrão por thread e diagnóstico
  legado por thread.
- **Informações/topologia — `src/cpu_info.c`:** lê `/proc/cpuinfo`, identifica
  processadores lógicos e conta pares únicos `physical id`/`core id` quando
  fornecidos.
- **Telemetria — `src/cpu_telemetry.c`:** encapsula leitura de temperatura
  (`sysfs` térmico ou override), clock atual (`cpufreq` com fallback para
  `cpu MHz`) e processos executáveis via `/proc/loadavg`.
- **Uso — `src/cpu_usage.c`:** lê os oito primeiros contadores agregados da
  linha `cpu` em `/proc/stat`, valida todos os tokens adicionais e calcula o
  delta entre amostras por contexto.
- **Frontend terminal — `examples/monitor.c`:** interface ncurses; consome a API pública
  e libera cada snapshot.
- **Frontend desktop — `examples/gtk_monitor.c`:** dashboard GTK4 que atualiza as métricas periodicamente.
- **Pacote Debian — `packaging/` e alvo `make package-deb`:** executável, launcher, ícone e metadados AppStream.
- **Site e manual — `site/` e `wiki/`:** guia de instalação, tutorial ilustrado e referência técnica.
- **Validação — `tests/` e `bench/`:** fixtures determinísticas, testes de
  concorrência/portabilidade e microbenchmark informativo.

## Contratos de dados

- `cpu_info_t` é alocada pela biblioteca e liberada pelo consumidor com
  `cpu_free_info()`. A biblioteca não mantém ponteiro para o snapshot após
  retorná-lo.
- `logical_processors` conta entradas `processor` do arquivo lido.
  `threads` é campo legado que reporta o número online retornado por
  `sysconf(_SC_NPROCESSORS_ONLN)`; os dois podem divergir em ambientes
  isolados/containers e não devem ser tratados como sinônimos.
- `physical_cores` conta pares únicos de IDs físicos/núcleo. Se a plataforma
  não fornece ambos os IDs, zero significa **desconhecido**, não zero núcleos.
- `current_frequency_mhz` é uma observação atual; não significa frequência base.
  Frequência e temperatura são opcionais e usam `-1.0f` quando indisponíveis.
- A primeira chamada a um contexto de uso estabelece a referência e retorna
  `0.0f`. Chamadas subsequentes retornam o percentual derivado dos deltas.
- A API legada de erro (`cpu_get_last_error*`) armazena o último diagnóstico
  por thread em GCC/Clang. Variantes `*_ex` copiam o diagnóstico para um
  `cpu_error_info_t` pertencente ao chamador; esse snapshot não muda após
  chamadas posteriores. O consumidor deve limpar/reutilizar o objeto quando
  desejar, usando `cpu_error_info_clear()`.
- `cpu_get_usage()`, `cpu_init()` e `cpu_cleanup()` usam contexto padrão
  por thread. Contextos explícitos pertencem ao consumidor e exigem
  sincronização externa se compartilhados entre threads.

## Portabilidade validada

A implementação atual soma contadores em C em todas as arquiteturas; não existe caminho assembly ativo. A CI executa GCC/Clang nativos e portáteis em Linux x86_64, sanitizers e
hardening, além de runtime nativo aarch64/arm64. O fallback C pode ser
compilado em outros alvos. ARMv7 de 32 bits executa os testes sob QEMU na CI; isso não
substitui validação em hardware ARMv7 nativo. O benchmark é informativo e não impõe limites rígidos.

## Variáveis de ambiente para fixtures

- `CPU_READER_CPUINFO_PATH`: fonte alternativa para `/proc/cpuinfo`.
- `CPU_READER_PROC_STAT_PATH`: fonte alternativa para `/proc/stat`.
- `CPU_READER_LOADAVG_PATH`: fonte alternativa para `/proc/loadavg`.
- `CPU_READER_CPU_TEMP_PATH`: arquivo de temperatura direto (milésimos de °C).
- `CPU_READER_THERMAL_PATH`: raiz de fixtures `thermal_zone*/type` e `temp`.
- `CPU_READER_CPU_FREQ_PATH`: arquivo de clock em kHz.

Esses overrides são úteis para testes. Alterar variáveis de ambiente enquanto
outras threads chamam a biblioteca não é seguro; configure-as antes de iniciar
as operações.

## Fluxo de dados

```text
Aplicação/monitor
       |
       v
API pública (include/cpu.h)
       |
       v
Fachada e parsing comum (src/cpu.c)
       |-------------------|---------------------|
       v                   v                     v
cpu_info.c            cpu_usage.c          cpu_telemetry.c
/proc/cpuinfo         /proc/stat           /proc/loadavg + sysfs
       |                   |                     |
       +-------------------+---------------------+
                           v
                 snapshots / valores / erros
```

## Limitações conhecidas

1. A API legada de erros continua disponível por compatibilidade; a família `*_ex`
   fornece snapshots explícitos por operação. Novas funções fallíveis devem
   oferecer o contrato explícito.
2. O estado por thread não torna um mesmo `cpu_usage_context_t` seguro para
   uso concorrente, nem torna chamadas simultâneas a `setenv()/unsetenv()`
   seguras.
3. A temperatura é escolhida pelo nome do tipo do sensor. Sensores não
   reconhecidos como CPU/package são ignorados; se nenhum sensor CPU/package
   válido estiver disponível, a API retorna `-1.0f` em vez de reportar
   silenciosamente a temperatura de outro componente. O nome/origem do sensor
   ainda não é exposto na API.

## Regras para evolução sem acoplamento

- O núcleo não deve depender de GTK, ncurses, systemd, ferramentas de distribuição ou serviços externos.
- Cada leitor de `/proc` ou `sysfs` deve permanecer atrás de uma interface interna pequena; os frontends consomem exclusivamente a API pública.
- Novas métricas devem preferir uma API aditiva e estruturas de resultado com status explícito. Não alterar o layout de `cpu_info_t` nem o significado dos retornos legados numa alteração compatível.
- Os leitores não devem inventar valores: ausência, erro de I/O e conteúdo inválido são estados diferentes e precisam ser documentados.
- A proveniência de uma métrica deve ser transportada como dado (origem/tipo/unidade/estado), não inferida pelo frontend a partir de mensagens de erro.
- Não adicionar cache global, mutex ou dependência de terceiros sem ADR, caso de uso demonstrável e testes que definam o contrato de concorrência.
- Variáveis de ambiente de fixture são configuração de processo; devem ser definidas antes de iniciar threads e nunca alteradas concorrentemente com leituras.

## Contratos de auditoria

A biblioteca fornece observações do sistema operacional, não atestação de hardware. Os dados podem ser omitidos, virtualizados, limitados por namespace/contêiner ou influenciados pelo kernel e firmware. Consumidores que produzem relatórios de auditoria devem registrar timestamp, plataforma, versão da biblioteca, unidade, status e fonte conhecida de cada observação. Até a API térmica estruturada ser implementada, a fonte exata do sensor não é garantida pela API pública.
