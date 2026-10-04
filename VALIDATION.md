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
- relatório de erros
- caminhos injetáveis por variáveis de ambiente

Execute:

```bash
make test
make test-portable
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
- Fallback C para arquiteturas não x86_64: validado por `make test-portable`

## Matriz de compatibilidade

| Plataforma | Núcleo | Monitor | Implementação |
| --- | --- | --- | --- |
| Linux x86_64 | Suportado | Opcional | Assembly otimizado ou C |
| Linux aarch64/arm64 | Suportado | Opcional | C portátil |
| Linux armv7 | Suportado | Opcional | C portátil |
