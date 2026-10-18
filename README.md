<div align="center">

<img src="site/assets/cpu-reader-logo.svg" width="88" height="88" alt="CPU Reader logo">

# CPU Reader

**Biblioteca C e ferramentas de diagnóstico de CPU para Linux.**

Leitura de métricas locais com estados explícitos, tratamento de erros e limites de confiança documentados.

[Documentação](doc/README.md) · [Tutorial](wiki/Tutorial.md) · [API](wiki/API.md) · [Segurança](SECURITY.md) · [Contribuir](CONTRIBUTING.md)

[![CI](https://github.com/mnl-nox/cpu-reader/actions/workflows/ci.yml/badge.svg)](https://github.com/mnl-nox/cpu-reader/actions/workflows/ci.yml)
[![Auditoria de distribuições](https://github.com/mnl-nox/cpu-reader/actions/workflows/distro-audit.yml/badge.svg)](https://github.com/mnl-nox/cpu-reader/actions/workflows/distro-audit.yml)
[![Site](https://github.com/mnl-nox/cpu-reader/actions/workflows/pages.yml/badge.svg)](https://github.com/mnl-nox/cpu-reader/actions/workflows/pages.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform: Linux](https://img.shields.io/badge/platform-Linux-111827.svg)](doc/support-matrix.md)

</div>

> **Estado do projeto: Beta.** APIs e empacotamento continuam em evolução. Não trate métricas ausentes como zero nem use o projeto como única fonte para decisões de segurança ou integridade de hardware.

## Visão geral

CPU Reader é um projeto C orientado a Linux que consulta interfaces do sistema operacional, principalmente `/proc` e `/sys`. O repositório contém uma biblioteca estática, um monitor de terminal, uma aplicação GTK4 opcional e ferramentas de validação.

- **Informações da CPU:** modelo, flags e topologia identificável pelo sistema.
- **Uso agregado:** cálculo por amostras sucessivas de `/proc/stat`.
- **Temperatura e frequência:** leitura quando o kernel e os drivers expõem interfaces compatíveis.
- **Diagnóstico estruturado:** estado, erro, mensagem e origem disponível por métrica em `include/cpu_diagnostics.h`.
- **Validação:** testes funcionais, concorrência, sanitizers, hardening e smoke tests por distribuição/arquitetura.

A disponibilidade e a semântica dos dados dependem do kernel, do driver, do firmware e do ambiente de execução. A API de diagnóstico pode retornar um relatório parcial; verifique o estado individual de cada métrica.

## Começar rapidamente

### Compilar a biblioteca e executar os testes

Requisitos básicos: compilador C99 (GCC ou Clang), GNU Make e libc Linux.

```sh
git clone https://github.com/mnl-nox/cpu-reader.git
cd cpu-reader

make core
make test
```

O artefato de desenvolvimento local é `libcpu.a`. Para sanitizers e flags de hardening:

```sh
make test-sanitize
make test-security
```

Esses alvos limpam e recompilam artefatos. Não os execute simultaneamente no mesmo diretório de trabalho.

### Debian / Ubuntu: aplicativo GTK4 e pacote local

```sh
sudo apt update
sudo apt install -y build-essential gcc make pkg-config libgtk-4-dev dpkg-dev

make package-deb
sudo apt install ./build/cpu-reader_*.deb
cpu-reader
```

O pacote é gerado para a arquitetura local e inclui o aplicativo, a biblioteca estática, os cabeçalhos públicos e os metadados `pkg-config`. Um pacote gerado localmente ou um artefato de CI **não equivale a uma release oficial**.

O monitor de terminal é opcional e requer `libncurses-dev`:

```sh
sudo apt install -y libncurses-dev
make monitor
./build/cpu-monitor
```

Consulte [BUILD.md](BUILD.md) para todos os componentes, dependências e alvos Make.

## Exemplo: relatório de diagnóstico em C

O exemplo usa a API aditiva. O relatório pode ser montado mesmo quando uma métrica opcional está indisponível; consulte o status antes de interpretar o valor.

```c
#include <stdio.h>
#include "cpu_diagnostics.h"

int main(void) {
    cpu_diagnostic_report_t report;
    cpu_error_info_t error;

    if (cpu_get_diagnostic_report(&report, &error) != 0) {
        fprintf(stderr, "Falha ao coletar diagnóstico: %s\n",
                error.message);
        return 1;
    }

    if (report.info_available) {
        printf("CPU: %s\n", report.model);
        printf("Núcleos físicos: %d; lógicos: %d\n",
               report.physical_cores, report.logical_processors);
    }

    if (report.temperature_c.status == CPU_DIAG_OK) {
        printf("Temperatura: %.1f °C (origem: %s)\n",
               report.temperature_c.value,
               report.temperature_c.source);
    } else {
        printf("Temperatura indisponível/ inválida: %s\n",
               report.temperature_c.message);
    }

    return 0;
}
```

Para compilar a partir do repositório após `make core`:

```sh
cc -std=c99 -Wall -Wextra -Iinclude exemplo.c -L. -lcpu -o exemplo
./exemplo
```

Referências: [API de diagnóstico](wiki/Diagnostic-API.md) · [contratos da API e concorrência](wiki/API.md).

## Métricas, origem e limites

| Métrica | Fonte principal | Interpretação importante |
|---|---|---|
| Uso da CPU | `/proc/stat` | A primeira amostra estabelece o baseline; é preciso uma amostra posterior para calcular a variação. |
| Modelo e topologia | `/proc/cpuinfo` e informações do sistema | A topologia física pode ser desconhecida; zero não prova ausência de núcleos. |
| Temperatura | zonas em `/sys/class/thermal` reconhecidas | Sensores podem não existir ou não representar a temperatura de todo o pacote. |
| Frequência | interfaces cpufreq/sysfs; fallback documentado | Pode refletir uma interface específica e não necessariamente todos os núcleos. |
| Processos executáveis | `/proc/loadavg` | É uma observação do kernel, não uma lista completa de processos. |

Os filtros de plausibilidade detectam alguns valores absurdos; não validam a calibração nem provam que a leitura corresponde a um sensor físico específico. O projeto não realiza atestação de firmware, kernel ou hardware.

## Documentação

- [Guia de compilação e instalação](BUILD.md)
- [Tutorial passo a passo](wiki/Tutorial.md)
- [Referência da API C](wiki/API.md)
- [API de diagnóstico](wiki/Diagnostic-API.md)
- [Arquitetura e decisões técnicas](doc/arquitetura.md)
- [Matriz de suporte por distribuição](doc/support-matrix.md)
- [Validação e testes](VALIDATION.md)
- [Modelo de confiança do diagnóstico](doc/diagnostico-hardware.md)
- [Segurança e divulgação responsável](SECURITY.md)
- [Perguntas frequentes](wiki/FAQ.md)
- [Histórico de alterações](CHANGELOG.md)

## Segurança e privacidade

A biblioteca consulta interfaces locais do sistema e não precisa de privilégios de root para as operações normais. A disponibilidade depende das permissões e do isolamento imposto pelo host.

Relatórios podem conter modelo de CPU, flags, topologia, caminhos de sensores e métricas do equipamento. **Revise e remova dados sensíveis antes de publicar logs, relatórios ou saídas de diagnóstico.** Não inclua informações pessoais, dumps completos de ambiente ou arquivos de sistema em issues públicas.

A telemetria observada não é uma prova criptográfica de integridade. Consulte [SECURITY.md](SECURITY.md) antes de usar o projeto em auditorias ou ambientes críticos.

## Contribuição

Issues e pull requests são bem-vindos. Antes de abrir uma alteração:

1. Leia [CONTRIBUTING.md](CONTRIBUTING.md) e verifique se já existe uma issue relacionada.
2. Descreva o problema com passos reproduzíveis e o ambiente mínimo necessário.
3. Execute `make test`; para mudanças de parsing ou memória, execute também sanitizers e hardening.
4. Não anexe dados identificáveis do host. Prefira fixtures sintéticas e valores redigidos.

Use o formulário de [issues](https://github.com/mnl-nox/cpu-reader/issues) para bugs e propostas. Relatos sensíveis devem seguir o procedimento de [segurança](SECURITY.md).

## Estrutura do repositório

```text
include/             cabeçalhos públicos
src/                 núcleo da biblioteca
examples/            monitor terminal e GTK4
tests/               testes funcionais, plataforma, diagnóstico e concorrência
bench/               microbenchmark informativo
packaging/           pacote Debian, launcher e metadados
doc/                 arquitetura, requisitos e limites de confiança
wiki/                tutoriais, API, FAQ e solução de problemas
site/                documentação web estática
.github/workflows/   CI, auditoria de distribuições e publicação do site
```

## Licença

CPU Reader é distribuído sob a [licença MIT](LICENSE).
