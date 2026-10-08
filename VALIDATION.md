# Validation Guide

Este documento resume como validar o projeto localmente.

## Validação automatizada

O teste integrado cobre os fluxos principais da API pública:

- leitura de informações da CPU
- cálculo de uso agregado
- contextos independentes de uso
- temperatura
- clock atual
- contagem de processos ativos
- isolamento de diagnóstico legado por thread
- topologia completa, incompleta e ausente
- caminhos injetáveis por variáveis de ambiente

Execute:

```bash
make test
make test-portable
make test-sanitize
make test-security
make benchmark
make telemetry
```

## Validação manual

1. Compile o projeto com `make`.
2. Execute o monitor com `./build/cpu-monitor`.
3. Confirme que as informações da CPU são exibidas.
4. Pressione `q` para encerrar.

## Cenários de falha esperados

- Se `/proc/cpuinfo` ou `/proc/stat` não puderem ser lidos, a API deve retornar
  `NULL` ou `-1.0f` conforme a função.
- Se não houver sensor de temperatura compatível, `cpu_get_temperature()`
  retorna `-1.0f`.
- Se não houver caminho válido para clock atual, `cpu_get_clock_speed()`
  tenta o fallback em `/proc/cpuinfo`.

## Status atual

- Biblioteca principal: validada pelo teste integrado
- Monitor ncurses: compilável com ncurses de desenvolvimento instalado
- Overrides de teste: cobertos pelos testes automatizados
- Implementação C: validada em x86_64 com GCC/Clang, em runtime nativo aarch64/arm64 e sob QEMU para ARMv7; a emulação não substitui hardware ARMv7 nativo. `make test-portable` é um alias de compatibilidade, não uma implementação distinta.
- Memória e comportamento indefinido: validados por `make test-sanitize`
- Hardening de compilação e linkedição: validado por `make test-security`
- Microbenchmark: `make benchmark`, resultado local informativo sem limiar rígido
- Telemetria local: validada pela geração de `build/telemetry.json`

## Execução com Docker

```bash
docker compose build
docker compose run --rm gcc
docker compose run --rm monitor
docker compose run --rm portable
docker compose run --rm sanitizers
docker compose run --rm security
docker compose run --rm static-analysis
docker compose run --rm telemetry
```

O serviço `static-analysis` usa Cppcheck. O container é atualizado durante o
build, executa como usuário sem privilégios, não publica portas e os testes não
enviam dados para serviços externos.

## Matriz de compatibilidade

| Plataforma | Núcleo | Monitor | Implementação |
| --- | --- | --- | --- |
| Linux x86_64 | Validado em CI | Opcional | C |
| Linux aarch64/arm64 | Validado em runtime na CI | Opcional | C |
| Linux armv7 (32-bit) | Testado sob QEMU; não hardware nativo | Opcional | C |
