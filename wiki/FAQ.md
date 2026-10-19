# Perguntas frequentes (Q&A)

Este FAQ descreve o comportamento conhecido da versão beta. A documentação da API e o código são a referência para contratos exatos. Recursos no roadmap são planejados, não garantidos como disponíveis.

## Produto e escopo

### O que é o CPU Reader?

É uma biblioteca C99 para Linux e um conjunto de ferramentas que consultam informações de CPU por interfaces locais do kernel, com diagnóstico estruturado e exemplos de monitoramento.

### O CPU Reader já é um monitor completo de todo o sistema?

Ainda não. O núcleo atual se concentra em informações da CPU, uso agregado, leitura opcional de temperatura/frequência e diagnóstico. Métricas públicas por CPU lógica, memória/swap/PSI, rede e I/O de armazenamento fazem parte do roadmap de expansão. Acompanhe o [roadmap](../doc/system-telemetry-roadmap.md) e as [issues](https://github.com/mnl-nox/cpu-reader/issues).

### Em quais sistemas operacionais funciona?

O projeto é direcionado ao Linux. Ele depende de interfaces como `/proc` e `/sys` e não oferece suporte nativo a Windows ou macOS.

### Preciso usar a interface GTK4?

Não. A biblioteca e os testes do núcleo não dependem de GTK4. O monitor desktop é opcional; o monitor de terminal é outro frontend opcional que depende de ncurses.

## Instalação e compilação

### Como começo rapidamente?

```sh
git clone https://github.com/mnl-nox/cpu-reader.git
cd cpu-reader
make core
make test
```

Consulte [BUILD.md](../BUILD.md) para dependências, monitor de terminal, GTK4 e pacote Debian.

### Quais dependências são obrigatórias?

Para o núcleo: Linux, compilador C99, GNU Make e libc de desenvolvimento. O monitor de terminal requer ncurses; a interface desktop requer GTK4; o empacotamento Debian requer ferramentas como dpkg-dev.

### Posso instalar o pacote Debian em qualquer distribuição?

Não. O pacote é produzido para a arquitetura do ambiente de build e destina-se a Debian e derivados compatíveis com as dependências declaradas. Verifique a [matriz de suporte](../doc/support-matrix.md). Um pacote local ou artefato de CI não é automaticamente uma release oficial.

### Por que um comando Make limpa os artefatos?

Alvos como `make test-sanitize`, `make test-security` e `make test-portable` limpam e recompilam com flags diferentes. Não execute esses alvos simultaneamente no mesmo diretório.

## Métricas e interpretação

### Por que a primeira leitura de uso da CPU é zero ou baseline?

O uso é calculado a partir da diferença entre contadores de `/proc/stat`. Uma única leitura não define um intervalo; a primeira amostra estabelece a referência e uma amostra posterior calcula a variação. Para novos coletores, o roadmap prevê status explícito de baseline.

### Por que o uso agregado pode diferir do monitor do sistema?

Ferramentas podem escolher janelas de amostragem, tratamento de iowait e normalização diferentes. Compare leituras feitas em intervalos semelhantes e verifique a semântica da métrica. O CPU Reader usa os contadores agregados de `/proc/stat` para o cálculo atual.

### Por que a temperatura está indisponível?

O kernel, driver, firmware ou máquina virtual pode não expor uma zona térmica compatível. A implementação atual consulta interfaces térmicas reconhecidas em `/sys/class/thermal`; descoberta geral por `/sys/class/hwmon` ainda não deve ser presumida. Ausência não significa temperatura zero ou normal.

### Por que a frequência está indisponível ou difere de outra ferramenta?

O driver pode não expor uma interface cpufreq acessível, e o fallback pode usar o primeiro valor `cpu MHz` de `/proc/cpuinfo`. Isso pode não representar todos os núcleos nem a frequência base. Consulte [arquitetura](../doc/arquitetura.md).

### Por que o número de núcleos físicos é zero?

O sistema pode não fornecer os identificadores físicos e de núcleo necessários. Nesse caso, zero significa topologia desconhecida/indisponível, não que o processador não tenha núcleos.

### “Processos ativos” significa todos os processos do sistema?

Não. O valor atual vem do campo runnable de `/proc/loadavg`, que indica processos executáveis/na fila de execução. Não é uma listagem nem a contagem total de processos. A documentação da API registra essa semântica para evitar confusão.

### Posso interpretar uma métrica ausente como zero?

Não. Ausência, falha de leitura, conteúdo inválido e valor válido são situações diferentes. Respeite o status e os valores sentinela documentados. Para diagnóstico estruturado, consulte [API de diagnóstico](Diagnostic-API.md).

## API e integração

### Qual cabeçalho devo incluir?

Use os cabeçalhos públicos em `include/`, como `cpu.h` e `cpu_diagnostics.h`. Não inclua arquivos internos de `src/`.

### Como libero os dados retornados?

O ponteiro de `cpu_get_info()` deve ser liberado com `cpu_free_info()`. Consulte a [referência da API](API.md) para ownership e ciclo de vida.

### O que as funções `*_ex` oferecem?

Elas recebem um objeto de erro pertencente ao chamador e associam o diagnóstico à operação, evitando depender apenas do último erro legado por thread. Leia [API de diagnóstico](Diagnostic-API.md).

### A biblioteca é thread-safe?

Os contextos independentes podem ser usados em paralelo quando cada thread possui seu próprio contexto. Um `cpu_usage_context_t` mutável não deve ser compartilhado entre threads sem sincronização externa. Configure variáveis `CPU_READER_*_PATH` antes de iniciar threads; não altere o ambiente concorrentemente com leituras.

### Posso usar a biblioteca como dependência de produção?

É possível integrá-la, mas considere o status beta, os contratos de compatibilidade, os limites de plataforma e a validação necessária para seu caso. Fixe a versão/commit, teste no ambiente-alvo e não trate os resultados como fonte única para decisões críticas.

## Diagnóstico, segurança e privacidade

### Preciso de root?

Não para as operações normais. Não execute como root sem necessidade concreta. Privilégios elevados não garantem que um sensor exista ou que seu valor seja confiável.

### O CPU Reader prova que o hardware é íntegro?

Não. Ele observa dados fornecidos pelo sistema operacional, que podem ser virtualizados, limitados por containers, omitidos ou influenciados pelo kernel, driver e firmware. Não oferece atestação criptográfica nem valida calibração física.

### A biblioteca envia telemetria para fora da máquina?

A biblioteca consulta interfaces locais; o site do projeto é documentação estática. A publicação manual de logs ou relatórios em issues pode expor detalhes do equipamento. Revise e redija saídas antes de compartilhá-las.

### Como reporto um bug?

Abra uma [issue](https://github.com/mnl-nox/cpu-reader/issues) com versão/commit, distribuição, kernel, arquitetura, passos mínimos para reproduzir e comandos de teste executados. Prefira fixtures sintéticas e remova dados pessoais do log.

### Como reporto uma vulnerabilidade?

Siga a política em [SECURITY.md](../SECURITY.md). Não publique detalhes exploráveis em uma issue pública.

## Testes, roadmap e contribuições

### Quais testes devo executar antes de enviar um PR?

No mínimo `make test`. Para parsing, memória ou cálculos de contadores, execute também `make test-sanitize` e `make test-security`. Para GTK4/pacote, execute os alvos correspondentes. Veja [guia de QA e relatórios](../doc/qa-and-reporting.md).

### O que significa “passou na CI”?

Significa que os jobs configurados passaram naquele commit e ambiente. Não garante funcionamento em todo hardware, distribuição ou kernel. Teste em emulação deve ser descrito como emulação, não como hardware nativo.

### Como são integradas novas capacidades?

Por issues com escopo e critérios de aceite, seguidas por PRs pequenos, testes determinísticos e documentação. O [roadmap](../doc/system-telemetry-roadmap.md) define a sequência atual.

### Como contribuo?

Leia [CONTRIBUTING.md](../CONTRIBUTING.md), selecione uma issue, preserve a compatibilidade pública e inclua evidências de validação. Declare claramente o que foi testado e o que continua pendente.
