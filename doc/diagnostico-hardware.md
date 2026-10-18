# Diagnóstico de hardware e confiança das métricas

A API de diagnóstico (include/cpu_diagnostics.h) agrega uma fotografia de CPU e o estado de cada métrica sem transformar ausência de sensor em zero. Ela é aditiva: não altera as estruturas nem os contratos da API legada em include/cpu.h.

## Como interpretar o relatório

- CPU_DIAG_OK: valor lido e aceito pelo parser da biblioteca.
- CPU_DIAG_BASELINE: a leitura de uso agregado estabeleceu apenas a referência inicial; não é uma medição útil de utilização. Faça outra amostra após um intervalo usando um contexto persistente para calcular uso.
- CPU_DIAG_UNAVAILABLE: fonte não fornecida pelo kernel/ambiente ou sensor não suportado.
- CPU_DIAG_ERROR: a leitura falhou ou os dados não passaram na validação. Examine error_code e message.
- source informa o caminho sysfs/procfs efetivamente lido quando disponível; no fallback de frequência, registra o caminho de /proc/cpuinfo. Para temperatura, sensor_type registra o tipo declarado pela zona térmica ou `explicit override` quando um caminho é configurado pelo consumidor. Se nenhuma fonte for selecionada, a string pode ficar vazia. O caminho identifica a interface lida, não constitui atestação criptográfica nem garante que o sensor físico representa todo o pacote da CPU.

A chamada retorna sucesso quando o relatório foi montado, mesmo se alguns campos não estiverem disponíveis. info_available e info_error descrevem a leitura da identificação/topologia; cada métrica tem seu próprio status, mensagem e código de erro.

## Evidência e limites

Os valores são observações do kernel no instante da leitura. Em containers, máquinas virtuais e ambientes restritos, /proc e sysfs podem refletir somente os recursos visíveis ao processo. A biblioteca não prova autenticidade do firmware, integridade do kernel, calibração do sensor, ausência de virtualização ou ausência de adulteração do host. Para auditoria reprodutível, registre o commit, kernel, arquitetura, distribuição, container/VM, permissões, origem dos dados e resultados de CI.

Não compare temperatura de sensores diferentes como se fossem equivalentes. A API valida temperatura sysfs dentro de uma faixa de plausibilidade de 0 a 250 °C e frequência dentro de 0 a 20.000 MHz; esses limites são filtros de sanidade, não especificações térmicas do fabricante. Valores fora da faixa são erro de parsing, não são truncados nem convertidos em zero. A API legada reporta zonas térmicas de CPU reconhecidas pelo nome; a semântica e a calibração dependem do driver. Frequência é uma observação atual/fallback, não a frequência base garantida. Para resultados críticos, confronte com interfaces e ferramentas específicas do fabricante.
