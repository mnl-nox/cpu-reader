# Contribuindo com o CPU Reader

Obrigado por considerar uma contribuição. O projeto prioriza mudanças pequenas, reproduzíveis e compatíveis com os contratos públicos existentes.

## Antes de começar

1. Abra uma issue para bugs maiores, mudanças de API ou propostas arquiteturais.
2. Confira a [arquitetura](doc/arquitetura.md), a [referência da API](wiki/API.md) e os [limites do diagnóstico](doc/diagnostico-hardware.md).
3. Confirme que a mudança não expõe dados do host nem interpreta uma métrica ausente como zero.

## Ambiente local

Requisitos básicos: GCC ou Clang, GNU Make e libc Linux.

```sh
make clean
make test
make test-sanitize
make test-security
```

Para mudanças no pacote GTK4/Debian, execute também:

```sh
make test-gtk
make test-gtk-ui
make package-deb
```

Não execute simultaneamente alvos que limpam e recompilam no mesmo diretório. Veja [BUILD.md](BUILD.md) e [VALIDATION.md](VALIDATION.md) para dependências e detalhes.

## Diretrizes de código

- Preserve assinaturas e layouts públicos legados; prefira API aditiva quando necessário.
- Trate leitura, parsing, indisponibilidade e valores implausíveis como estados distintos.
- Use fixtures sintéticas para sensores, arquivos incompletos, erros de I/O e condições de corrida.
- Não dependa de acesso root nem de hardware específico nos testes automatizados.
- Documente unidades, origem, propriedade de memória, concorrência e limitações.
- Evite dependências no núcleo que pertençam apenas aos frontends.

## Pull requests

Inclua no PR:

- O problema e a solução proposta.
- Mudanças observáveis na API ou nos contratos.
- Comandos de teste executados e resultados relevantes.
- Limitações conhecidas e plataformas em que foi validado.

Não afirme suporte de hardware ou distribuição apenas porque a compilação passou. Diferencie teste nativo, emulação e execução em hardware real.

## Privacidade e segurança

Antes de publicar logs, revise nomes de host, caminhos pessoais, identificadores, flags e informações de topologia. Envie apenas os dados mínimos para reproduzir o problema.

Para vulnerabilidades ou relatos sensíveis, siga a [política de segurança](SECURITY.md) em vez de publicar detalhes exploráveis numa issue aberta.
