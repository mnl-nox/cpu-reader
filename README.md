# CPU Reader

Biblioteca C99 e monitor de CPU para Linux. O projeto oferece uma API em C, um monitor de terminal opcional e uma interface desktop GTK4 empacotável em Debian.

[Diagnóstico e confiança das métricas](doc/diagnostico-hardware.md) · [Matriz de suporte Linux](doc/support-matrix.md) · [Documentação completa](doc/README.md) · [Wiki do projeto](wiki/Home.md) · [Site](site/index.html) · [Build](BUILD.md) · [Validação](VALIDATION.md) · [Segurança](SECURITY.md)

## O que o projeto faz

- Lê modelo, topologia lógica/física e flags da CPU.
- Calcula uso agregado da CPU a partir de <code>/proc/stat</code>.
- Consulta temperatura e frequência quando o kernel expõe sensores compatíveis; rejeita valores fora das faixas de plausibilidade documentadas.
- Conta processos em execução por meio de <code>/proc/loadavg</code>.
- Fornece snapshots de erro explícitos por meio das funções da família <code>*_ex</code>.
- Oferece uma API aditiva de diagnóstico por métrica em <code>include/cpu_diagnostics.h</code>, com status, origem provável e erro independente.
- Inclui um exemplo de terminal e uma aplicação GTK4 com atualização automática.
- Gera um pacote <code>.deb</code> para a arquitetura da máquina de build.

> **Estado:** Beta. Temperatura, frequência e detalhes da topologia dependem do kernel, do hardware e das permissões disponíveis. Valores ausentes são reportados como indisponíveis, não como zero.

## Instalação rápida — Debian/Ubuntu

Para compilar localmente, instale os requisitos e gere o pacote. Para usar somente a biblioteca, GTK4 não é necessário; veja os alvos `make core` e `make test`:

~~~sh
sudo apt update
sudo apt install -y git build-essential pkg-config libgtk-4-dev dpkg-dev
git clone https://github.com/mnl-nox/cpu-reader.git
cd cpu-reader
make package-deb
sudo apt install ./build/cpu-reader_*.deb
cpu-reader
~~~

O pacote inclui o monitor GTK4, a biblioteca estática `libcpu.a`, os cabeçalhos públicos e metadados `pkg-config` para consumidores C. O pacote de build é gerado para a arquitetura local; não é uma release oficial por si só.

## Compilar e testar a biblioteca

~~~sh
make core
make test
make test-sanitize
make test-security
~~~

Para compilar o monitor de terminal, instale também <code>libncurses-dev</code> e execute <code>make monitor</code>. Para compilar e validar a interface GTK4, consulte [BUILD.md](BUILD.md).

## Exemplo de API de diagnóstico

O cabeçalho `cpu_diagnostics.h` fornece status e erros independentes para cada métrica. A chamada pode retornar um relatório parcial quando sensores não existem; consulte [a referência da API de diagnóstico](wiki/Diagnostic-API.md) e [os limites de confiança](doc/diagnostico-hardware.md).

## Exemplo de API

~~~c
#include <stdio.h>
#include "cpu.h"

int main(void) {
    cpu_info_t *info = cpu_get_info();
    if (info == NULL) {
        fprintf(stderr, "Não foi possível consultar a CPU\n");
        return 1;
    }

    printf("Modelo: %s\n", info->model);
    printf("Núcleos físicos: %d\n", info->physical_cores);
    printf("Processadores lógicos: %d\n", info->logical_processors);
    cpu_free_info(info);
    return 0;
}
~~~

Consulte [wiki/API.md](wiki/API.md) para o ciclo de vida da API, semântica dos retornos e tratamento de erros. Para um relatório parcial que preserve a validade individual de cada métrica, use [a API de diagnóstico](wiki/Diagnostic-API.md).

## Estrutura do projeto

~~~text
include/cpu.h        API pública
src/                 implementação da biblioteca
examples/            monitores de terminal e GTK4
tests/               testes funcionais, de plataforma e interface
bench/               microbenchmark
packaging/           launcher desktop, ícone e metadados AppStream
doc/                 arquitetura, requisitos, ADRs e UML
wiki/                manual completo em páginas Markdown
site/                site estático responsivo
.github/workflows/   CI, validação e deploy do site
~~~

## Métricas e limitações

| Métrica | Fonte principal | Quando indisponível |
|---|---|---|
| Uso agregado | <code>/proc/stat</code> | erro de leitura/parsing; primeira amostra estabelece baseline |
| Modelo e topologia | <code>/proc/cpuinfo</code> e informações do sistema | campos podem faltar; topologia física desconhecida pode ser zero |
| Temperatura | <code>/sys/class/thermal</code> e interfaces compatíveis em sysfs | retorno <code>-1.0</code> |
| Frequência | sysfs; fallback para <code>/proc/cpuinfo</code> | retorno <code>-1.0</code> |
| Processos em execução | <code>/proc/loadavg</code> | retorno <code>-1</code> |

A biblioteca é voltada para Linux e usa interfaces de sistema como <code>/proc</code> e <code>/sys</code>. O núcleo não depende de ncurses nem de GTK4; essas bibliotecas são usadas apenas pelos respectivos frontends.

## Documentação e suporte

- [Manual de instalação e uso](wiki/Installation.md)
- [Tutorial passo a passo](wiki/Tutorial.md)
- [Arquitetura](doc/arquitetura.md)
- [Requisitos funcionais](doc/requerimentos.md)
- [Requisitos não funcionais](doc/requisitos-nao-funcionais.md)
- [Decisões arquiteturais (ADRs)](doc/architecture-decision-records.md)
- [Diagramas UML](doc/uml-diagrams.md)
- [Guia de validação](VALIDATION.md)
- [Política de segurança](SECURITY.md)
- [Histórico de alterações](CHANGELOG.md)

## Licença

Distribuído sob a licença MIT. Consulte [LICENSE](LICENSE).
