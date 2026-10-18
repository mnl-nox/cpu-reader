# Matriz de suporte Linux

## Escopo

CPU Reader é uma biblioteca C99/POSIX-oriented para Linux que coleta observações disponíveis em `/proc` e `sysfs`. Não é realista prometer que toda métrica estará disponível em toda distribuição, kernel, arquitetura, máquina virtual ou contêiner.

## Matriz de validação automatizada

| Ambiente | Compilação/testes do núcleo | Status da evidência |
|---|---|---|
| Ubuntu LTS, GCC | `make test` | CI nativa |
| Ubuntu LTS, Clang | `make test` | CI nativa |
| Ubuntu LTS, sanitizers | `make test-sanitize` | CI |
| Ubuntu LTS, hardening | `make test-security` | CI |
| Ubuntu LTS, ARM64 | `make test` | Runner nativo quando disponível |
| ARMv7 Linux | testes cross-compiled sob QEMU | Emulação; não comprova hardware físico |
| Fedora (imagem corrente) | `make test` | Smoke test de distribuição |
| Alpine Linux / musl (imagem corrente) | `make test` | Smoke test de libc alternativa |

O workflow `.github/workflows/distro-audit.yml` mantém os smoke tests de distribuição. A versão das imagens de Fedora/Alpine é móvel; para reproduções de auditoria, registre o digest da imagem e o SHA do commit usado no run. O resultado da CI é a evidência primária; esta tabela não substitui a inspeção do histórico dos workflows.

## Dependências

- Compilador GCC ou Clang com suporte ao modo C99 e à extensão de TLS usada pela API legada.
- GNU Make e libc Linux.
- `ncurses` apenas para o monitor terminal.
- GTK4 e `pkg-config` apenas para a interface desktop.
- Ferramentas de empacotamento Debian apenas para gerar o pacote `.deb`.

O núcleo não deve exigir privilégios de root para as operações normais. A disponibilidade de sensores depende das interfaces e permissões expostas pelo host.

## Semântica de métricas ausentes

- Temperatura e frequência indisponíveis: `-1.0f`, conforme a API legada.
- Processos indisponíveis: `-1`, conforme a API legada.
- Topologia física desconhecida: `physical_cores == 0`.
- Primeira amostra de uso: estabelece baseline e retorna `0.0f`.

Não converta sentinelas em zero nem interprete ausência de sensor como temperatura normal. Consulte [SECURITY.md](../SECURITY.md) antes de usar os dados em auditorias de máquina.

## Critérios de declaração de suporte

Uma plataforma só deve ser descrita como validada quando:
1. A compilação e os testes automatizados passaram num ambiente identificável.
2. As métricas opcionais ausentes e os erros de leitura foram exercitados por fixtures.
3. A arquitetura e o tipo de validação (nativo ou emulado) estão registrados.
4. A documentação não extrapola o resultado observado para todo hardware da mesma família.

## Fora do escopo de garantia

A biblioteca não fornece atestação de integridade de firmware/kernel, não garante acesso a sensores escondidos pelo hypervisor e não substitui ferramentas de diagnóstico específicas do fabricante.
