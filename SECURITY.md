# Segurança

## Escopo

O CPU Reader é uma biblioteca local Linux-only. O núcleo lê apenas as
interfaces do sistema `/proc` e `sysfs` e não abre sockets, não envia dados e
não executa comandos externos.

## Controles implementados

- Parsing limitado por buffers de tamanho fixo.
- Fechamento dos arquivos em todas as rotas de leitura.
- Flags de hardening em `make test-security`: proteção de stack,
  `_FORTIFY_SOURCE`, PIE, RELRO e `BIND_NOW`.
- AddressSanitizer e UndefinedBehaviorSanitizer em `make test-sanitize`.
- Cppcheck no serviço Docker `static-analysis`.
- Container de testes executado como usuário sem privilégios e sem portas
  publicadas.
- Testes com arquivos de entrada controlados para erros de parsing e caminhos
  inexistentes.

## Telemetria

Não existe telemetria remota. O target `make telemetry` gera somente
`build/telemetry.json` localmente, com o compilador, a plataforma genérica e o
status dos testes. O arquivo não deve ser enviado automaticamente.

## Relato responsável

Para relatar uma vulnerabilidade, abra uma issue privada ou entre em contato
com os mantenedores antes de publicar detalhes exploráveis. Inclua versão,
plataforma, passos para reproduzir e impacto observado.
