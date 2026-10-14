# Contribuição

## Fluxo recomendado

1. Abra uma issue descrevendo o problema ou a proposta.
2. Crie uma branch curta e específica.
3. Faça uma alteração pequena e documente mudanças de comportamento.
4. Adicione ou atualize testes.
5. Execute os testes relevantes localmente.
6. Abra um pull request com motivação, resumo e evidências de validação.

## Convenções

- Preserve compatibilidade C99.
- Não introduza dependências de GTK4/ncurses no núcleo.
- Documente valores de retorno e casos de indisponibilidade.
- Não versionar artefatos de build, binários locais, diretórios temporários ou dados da máquina.
- Atualize o manual quando alterar a API, instalação, dependências ou comportamento.
- Use mensagens de commit descritivas.

## Antes de abrir o PR

~~~sh
make clean
make test
make test-sanitize
make test-security
~~~

Se alterar a interface ou o pacote, execute também os smoke tests GTK4 e o build Debian. Não declare que uma validação foi concluída se o comando não foi executado ou se o CI não passou.
