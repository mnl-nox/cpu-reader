# Roadmap de telemetria de sistema

## Objetivo

Evoluir o CPU Reader de uma biblioteca de métricas de CPU para uma biblioteca C99 de observabilidade local do Linux. A biblioteca deve ser consumível sem GTK, ncurses, systemd ou serviços externos. Interfaces gráficas e monitores serão clientes da API pública.

Esta roadmap descreve entregas incrementais; não declara métricas planejadas como implementadas.

## Princípios de engenharia

1. **Estado explícito:** cada métrica informa OK, baseline, indisponível ou erro. Ausência nunca é codificada como zero.
2. **Snapshot coerente:** cada leitura inclui timestamp monotônico, unidade, origem conhecida e validade; métricas independentes podem falhar sem invalidar o restante do snapshot.
3. **Ownership explícito:** estruturas de saída pertencem ao chamador ou têm funções de init/destroy claras. Sem ponteiros para buffers temporários.
4. **Concorrência documentada:** contextos de amostragem são independentes; compartilhamento entre threads exige sincronização externa, salvo contrato explícito diferente.
5. **Parsing defensivo:** validar linhas truncadas, tokens extras, conversões numéricas, overflow, contadores regressivos e limites de memória.
6. **Testes determinísticos:** usar fixtures e caminhos de fonte configuráveis. Testes do core não dependem de sensores reais, GPU ou GTK.
7. **Compatibilidade aditiva:** preservar a API legada em include/cpu.h. Novas capacidades entram em cabeçalhos dedicados; mudanças incompatíveis exigem versionamento/ADR.
8. **Sem privilégios desnecessários:** leituras comuns devem funcionar como usuário normal; recursos restritos retornam indisponibilidade documentada.

## Arquitetura-alvo

- **API pública:** estruturas de snapshot, enums de estado, unidades e contratos de ownership.
- **Coletores internos:** módulos isolados para CPU, memória/PSI, processos/carga, rede, filesystem e I/O de bloco.
- **Normalização:** parsing e cálculos de delta separados da apresentação.
- **Diagnóstico:** status e origem por métrica; falhas parciais preservam os valores válidos.
- **Consumidores:** CLI, GTK e aplicações de terceiros dependem apenas dos cabeçalhos públicos e da biblioteca.

## Plano incremental

### Iteração 0 — contrato e critérios
Issue: #45

Definir API, estados, timestamp, unidades, ownership, thread-safety e compatibilidade antes de estabilizar símbolos públicos.

Aceite:
- [ ] API proposta documentada e revisada.
- [ ] Estruturas determinísticas e sem alocações implícitas não documentadas.
- [ ] Estados e unidades não ambíguos.
- [ ] Política de versionamento e compatibilidade descrita.

### Iteração 1 — CPU por processador lógico
Issue: #46

Adicionar contexto explícito de amostragem e coleção dinâmica/limitada de CPUs lógicas. IDs de CPU não serão presumidos contíguos. A primeira amostra será baseline, e regressões/overflow invalidarão somente a métrica afetada.

Aceite:
- [ ] Fixture de /proc/stat com IDs esparsos e campos adicionais.
- [ ] Testes para linha truncada, tokens inválidos, overflow, baseline e contador regressivo.
- [ ] Sem escrita fora de limites ou pressuposto de IDs contíguos.
- [ ] API legada agregada permanece compatível.

### Iteração 2 — memória, swap e pressão
Issue: #47

Adicionar MemTotal/MemAvailable e swap a partir de /proc/meminfo; PSI será opcional.

Aceite:
- [ ] Percentuais calculados apenas com denominador válido.
- [ ] Unidades e escopo host/container documentados.
- [ ] PSI ausente não invalida outras métricas.
- [ ] Fixtures para dados parciais, inválidos e overflow.

### Iteração 3 — carga e processos
Issue: #48

Expor load average, processos runnable e total de processos como conceitos separados.

Aceite:
- [ ] Nomes e unidades corretos na API.
- [ ] Processos que desaparecem durante a leitura não causam falha global.
- [ ] Testes não dependem do estado do host.

### Iteração 4 — rede e armazenamento
Issue: #49

Separar capacidade do filesystem, contadores de I/O por dispositivo e taxas de rede. Taxas exigem duas amostras monotônicas válidas.

Aceite:
- [ ] Contadores absolutos e taxas têm campos distintos.
- [ ] Interface/dispositivo removido ou contador reiniciado invalida o delta afetado.
- [ ] Loopback é controlado por opção.
- [ ] Capacidade de filesystem nunca é rotulada como throughput de disco.

### Iteração transversal — CI e qualidade
Issue: #50

Cada PR deve compilar e executar os testes aplicáveis antes de ser elegível para integração.

Aceite:
- [ ] GCC e Clang quando disponíveis.
- [ ] Testes funcionais, sanitizers e hardening em jobs separados.
- [ ] Core compila sem dependências de GUI.
- [ ] Documentação de unidade, status, origem, ownership e concorrência.

## Definition of Done para cada PR

- Escopo limitado a uma issue/iteração.
- Critérios de aceite marcados com evidências ou logs de CI.
- Testes novos para caminhos de sucesso, falha e limites.
- Sem regressão nos testes existentes e na API legada.
- Documentação atualizada junto com o código.
- Sem declarar teste executado quando ele não foi executado.
- PR permanece draft enquanto compilação e testes não tiverem evidência suficiente.

## Fora do primeiro ciclo

GPU, SMART/temperatura de disco, métricas por cgroup, sensores dependentes de drivers específicos e coleta que exija privilégios serão avaliados em issues separadas após a API base. Não serão simulados quando ausentes.
