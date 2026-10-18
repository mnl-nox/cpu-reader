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

## Checklist de auditoria de máquina

Antes de usar os resultados para inventário, conformidade ou decisões operacionais:

- Trate métricas opcionais como desconhecidas quando o retorno for `-1.0f` (temperatura/frequência) ou `-1` (processos), conforme a função.
- Não interprete `physical_cores == 0` como zero núcleos: significa que a topologia física não pôde ser determinada.
- Registre a versão da biblioteca, arquitetura, kernel e origem de cada métrica no sistema consumidor. A API atual não expõe a proveniência térmica estruturada; consulte [issue #37](https://github.com/mnl-nox/cpu-reader/issues/37).
- Não execute o coletor como root apenas para tentar obter sensores; respeite as permissões e a visibilidade disponibilizadas pelo kernel/contêiner.
- Não trate uma única amostra como evidência histórica. Para auditoria, persista timestamp, unidade, estado de disponibilidade e contexto da coleta na aplicação chamadora.
- Use o checklist como orientação técnica, não como certificação de segurança ou garantia de integridade do host.

A biblioteca é um coletor local de métricas; não é uma raiz de confiança e não prova que o kernel, o firmware ou os dados expostos pelo host são íntegros.
