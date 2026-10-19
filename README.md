<div align="center">

<img src="site/assets/cpu-reader-logo.svg" width="88" height="88" alt="CPU Reader logo">

# CPU Reader

**Biblioteca C99 e ferramentas de diagnóstico de CPU para Linux**

Leituras locais de métricas, API de diagnóstico com estados explícitos e ferramentas para desenvolvimento e validação.

[Documentação técnica](doc/README.md) · [Tutorial](wiki/Tutorial.md) · [API C](wiki/API.md) · [Perguntas frequentes](wiki/FAQ.md) · [Segurança](SECURITY.md)

[![CI](https://github.com/mnl-nox/cpu-reader/actions/workflows/ci.yml/badge.svg)](https://github.com/mnl-nox/cpu-reader/actions/workflows/ci.yml)
[![Auditoria de distribuições](https://github.com/mnl-nox/cpu-reader/actions/workflows/distro-audit.yml/badge.svg)](https://github.com/mnl-nox/cpu-reader/actions/workflows/distro-audit.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Plataforma: Linux](https://img.shields.io/badge/platform-Linux-111827.svg)](doc/support-matrix.md)

</div>

> **Estado:** beta em evolução. O núcleo implementado monitora principalmente CPU; a expansão para telemetria completa do sistema está planejada e será entregue por etapas. Não interprete métricas indisponíveis como zero nem use o projeto como única fonte para decisões críticas.

## 1. Visão geral

CPU Reader é uma biblioteca C99 para consultar informações expostas localmente pelo Linux, principalmente por `/proc` e `/sys`. O repositório inclui uma biblioteca estática, um monitor de terminal opcional, uma interface GTK4 opcional e ferramentas de validação.

### Capacidades implementadas

- Informações de CPU, modelo, flags e topologia quando expostas pelo kernel.
- Uso agregado da CPU calculado a partir de amostras sucessivas de `/proc/stat`.
- Leitura opcional de temperatura e frequência, conforme interfaces de kernel e drivers disponíveis.
- Relatório de diagnóstico por métrica, com status, código de erro, mensagem e fonte quando disponível.
- Contextos de amostragem independentes e contratos documentados para concorrência.
- Testes funcionais, testes de plataforma/concorrência, sanitizers, hardening e empacotamento Debian.

### Escopo que ainda está em evolução

A biblioteca **não deve ser considerada, nesta versão, uma solução completa de monitoramento de sistema**. Coleta pública de CPU por núcleo lógico, memória/swap, PSI, inventário detalhado de processos, taxas de rede e I/O de armazenamento estão no roadmap. O dashboard GTK não substitui uma API pública de coleta; novos coletores devem ser implementados no núcleo e consumidos pelos frontends.

Consulte o [roadmap de telemetria](doc/system-telemetry-roadmap.md) e as [issues do projeto](https://github.com/mnl-nox/cpu-reader/issues).

## 2. Começar rapidamente

### Requisitos do núcleo

- Linux com interfaces `/proc` e `/sys` apropriadas.
- GCC ou Clang com suporte a C99.
- GNU Make e libc de desenvolvimento.

### Compilar e testar

```sh
git clone https://github.com/mnl-nox/cpu-reader.git
cd cpu-reader

make clean
make core
make test
```

O alvo `make core` produz `libcpu.a`. Para verificações adicionais:

```sh
make test-sanitize
make test-security
make test-portable
```

Os alvos de sanitizers, hardening e portabilidade podem limpar e recompilar os artefatos. **Não os execute simultaneamente no mesmo diretório de trabalho.**

### Monitores opcionais

Monitor de terminal, dependente de ncurses:

```sh
sudo apt install -y libncurses-dev
make monitor
./build/cpu-monitor
```

Aplicação desktop, dependente de GTK4:

```sh
sudo apt install -y pkg-config libgtk-4-dev
make gtk-monitor
./build/cpu-reader-gtk
```

Para gerar o pacote Debian local:

```sh
sudo apt install -y dpkg-dev
make package-deb
ls -lh build/cpu-reader_*.deb
```

Um pacote local ou artefato de CI não equivale a uma release oficial. Consulte [BUILD.md](BUILD.md) para dependências e alvos disponíveis.

## 3. Exemplo mínimo em C

Este exemplo consulta um snapshot de informações da CPU e libera a memória corretamente.

```c
#include <stdio.h>
#include "cpu.h"

int main(void)
{
    cpu_info_t *info = cpu_get_info();

    if (info == NULL) {
        fprintf(stderr, "Não foi possível consultar a CPU: %s\n",
                cpu_get_last_error());
        return 1;
    }

    printf("Modelo: %s\n", info->model);
    printf("Processadores lógicos identificados: %d\n",
           info->logical_processors);

    if (info->physical_cores > 0) {
        printf("Núcleos físicos identificados: %d\n",
               info->physical_cores);
    } else {
        puts("Topologia física: desconhecida ou indisponível");
    }

    cpu_free_info(info);
    return 0;
}
```

Compile a partir da raiz do clone:

```sh
make core
cc -std=c99 -Wall -Wextra -Wpedantic -Iinclude exemplo.c -L. -lcpu -o exemplo
./exemplo
```

Para aplicações que precisam associar erros à chamada correta, prefira as variantes `*_ex` e a [API de diagnóstico](wiki/Diagnostic-API.md).

## 4. Métricas e semântica

| Métrica | Fonte atual | Como interpretar |
|---|---|---|
| Modelo, flags e topologia | `/proc/cpuinfo` e interfaces do sistema | A topologia física pode não ser exposta; zero nesse campo significa desconhecido/indisponível. |
| Uso agregado da CPU | `/proc/stat` | É calculado por deltas. A primeira amostra estabelece o baseline; amostras posteriores permitem estimar o uso no intervalo. |
| Temperatura | Interfaces térmicas compatíveis em `/sys/class/thermal` ou override configurado | Sensor ausente, virtualização e drivers podem impedir a leitura. O valor não prova calibração física. |
| Frequência atual | Interfaces cpufreq/sysfs com fallback documentado | Não equivale necessariamente à frequência base nem representa todos os núcleos. |
| Processos executáveis | `/proc/loadavg` | É a contagem runnable informada pelo kernel, não um inventário completo de processos. |

A API de diagnóstico distingue valor válido, baseline, indisponibilidade e erro. Consulte o status individual antes de interpretar o valor. Valores sentinela como `-1.0f` não devem ser apresentados como medições.

## 5. Documentação e relatórios

- [Índice da documentação técnica](doc/README.md)
- [Compilação, instalação e empacotamento](BUILD.md)
- [Tutorial de uso](wiki/Tutorial.md)
- [Referência da API C](wiki/API.md)
- [API de diagnóstico](wiki/Diagnostic-API.md)
- [Perguntas frequentes (Q&A)](wiki/FAQ.md)
- [Arquitetura atual](doc/arquitetura.md)
- [Roadmap de telemetria de sistema](doc/system-telemetry-roadmap.md)
- [Guia de QA, validação e relatórios](doc/qa-and-reporting.md)
- [Matriz de suporte](doc/support-matrix.md)
- [Requisitos funcionais](doc/requerimentos.md)
- [Requisitos não funcionais](doc/requisitos-nao-funcionais.md)
- [Modelo de confiança do diagnóstico](doc/diagnostico-hardware.md)
- [Validação local e CI](VALIDATION.md)
- [Histórico de alterações](CHANGELOG.md)
- [Política de segurança](SECURITY.md)
- [Como contribuir](CONTRIBUTING.md)

## 6. Segurança, privacidade e confiança

O núcleo consulta interfaces locais do sistema e normalmente não precisa de privilégios de root. Não execute com privilégios elevados sem uma justificativa concreta. Um valor lido de `/proc` ou `/sys` é uma observação fornecida pelo kernel e pelo ambiente; não é uma prova criptográfica da integridade de hardware, firmware ou sistema operacional.

Relatórios podem revelar modelo do processador, flags, topologia, versão do kernel, caminhos de sensores e métricas do equipamento. Antes de publicar logs ou anexos em issues, remova nomes de host, identificadores e dados desnecessários. Para vulnerabilidades, siga [SECURITY.md](SECURITY.md) em vez de publicar detalhes sensíveis em uma issue pública.

## 7. Qualidade e contribuição

Antes de abrir um pull request:

1. Leia [CONTRIBUTING.md](CONTRIBUTING.md) e procure uma issue existente.
2. Mantenha a alteração focada e documente contratos, unidades, ownership e concorrência.
3. Use fixtures determinísticas para casos de parsing e erro; evite dependência de hardware real.
4. Execute `make test`; para mudanças sensíveis a memória ou parsing, execute também `make test-sanitize` e `make test-security`.
5. Informe exatamente quais comandos foram executados e seus resultados. Não declare testes aprovados sem evidência.

A integração de novos recursos segue issues e PRs iterativos com critérios de aceite. O núcleo deve continuar independente de GTK4 e ncurses.

## 8. Estrutura do repositório

```text
include/             cabeçalhos públicos da biblioteca
src/                 implementação do núcleo
examples/            monitores e exemplos de uso
tests/               testes funcionais, diagnóstico e concorrência
bench/               microbenchmark informativo
packaging/           metadados e empacotamento Debian
doc/                 arquitetura, requisitos, roadmap e QA
wiki/                tutoriais, API, FAQ e solução de problemas
site/                documentação web estática
.github/workflows/   automação de CI e auditorias
```

## Licença

CPU Reader é distribuído sob a [licença MIT](LICENSE).
