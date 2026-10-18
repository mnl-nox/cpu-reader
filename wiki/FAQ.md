# Perguntas frequentes

## O CPU Reader funciona no Windows ou macOS?

Não. A implementação é direcionada ao Linux e consulta interfaces como `/proc` e `/sys`.

## Por que temperatura ou frequência estão indisponíveis?

O kernel e o driver precisam expor uma interface compatível. Máquinas virtuais e containers também podem ocultar sensores. A ausência de uma leitura não significa que a temperatura seja normal nem que a frequência seja zero. Nesta versão, a descoberta automática de temperatura cobre zonas em `/sys/class/thermal`; a varredura automática de `/sys/class/hwmon` ainda não está implementada.

## Preciso de root?

Não para as operações normais. Não execute como root sem uma necessidade específica e compreendida. Permissões adicionais não fazem um sensor indisponível tornar-se confiável.

## Por que a primeira leitura de uso é baseline?

O uso é calculado comparando contadores de `/proc/stat` entre amostras. A primeira chamada estabelece a referência; faça outra leitura depois de um intervalo para obter a variação.

## Posso interpretar valores ausentes como zero?

Não. Valores sentinela e status de indisponibilidade ou erro devem ser preservados. Consulte [a API de diagnóstico](Diagnostic-API.md).

## O pacote Debian funciona em qualquer distribuição?

Não. Ele é gerado para a arquitetura local e destina-se a Debian e derivados compatíveis com as dependências declaradas. Use a [matriz de suporte](../doc/support-matrix.md) para entender a evidência disponível.

## O projeto prova que o hardware é íntegro?

Não. O CPU Reader observa dados fornecidos pelo sistema operacional. Não atesta criptograficamente a integridade de firmware, kernel ou hardware nem garante calibração física do sensor. Consulte [os limites de confiança](../doc/diagnostico-hardware.md).

## O site ou a biblioteca enviam telemetria?

O site é estático e não tem formulário de envio de dados. A biblioteca lê interfaces locais do Linux. Porém, relatórios e logs copiados manualmente para issues podem revelar modelo, flags, topologia ou caminhos de sensores; revise-os e redija dados desnecessários antes de publicar.

## Como reporto um bug ou uma vulnerabilidade?

Para bugs comuns, abra uma [issue](https://github.com/mnl-nox/cpu-reader/issues) com passos reproduzíveis, versão, arquitetura e saída redigida. Para relatos sensíveis, siga [SECURITY.md](../SECURITY.md) e evite publicar detalhes exploráveis numa issue pública.

## Onde encontro a API e como contribuo?

Consulte [API C](API.md), [guia de compilação](../BUILD.md) e [CONTRIBUTING.md](../CONTRIBUTING.md). Execute `make test` antes de enviar uma alteração e informe quais verificações foram realmente executadas.
