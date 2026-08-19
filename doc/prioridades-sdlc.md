# Prioridades de desenvolvimento do CPU Reader

Este roadmap parte do estado atual do repositório, que já possui biblioteca, monitor, testes básicos e build com Make.

## Concluído

- [x] Estrutura `include/`, `src/`, `examples/`, `tests/` e `build/`.
- [x] API pública para informações, uso, temperatura, inicialização e liberação.
- [x] Leitura de `/proc/cpuinfo`.
- [x] Cálculo de uso agregado com `/proc/stat`.
- [x] Monitor de terminal com ncurses.
- [x] Teste básico executável com `make test`.
- [x] Build reprodutível com `make all` e limpeza com `make clean`.

## Próxima prioridade: robustez da biblioteca

1. Separar o estado do cálculo de uso em um contexto por instância ou documentar uma API thread-safe.
2. Adicionar testes para falha de arquivos, parsing e primeira leitura de uso.
3. Verificar o comportamento em arquiteturas e formatos de `/proc` diferentes.
4. Executar testes com AddressSanitizer e verificar vazamentos.

## Próxima prioridade: recursos de CPU

1. Implementar leitura de temperatura usando interfaces Linux disponíveis.
2. Adicionar métricas por núcleo.
3. Definir claramente a diferença entre processadores lógicos e núcleos físicos.
4. Melhorar o tratamento e a exposição de erros.

## Manutenção do projeto

- Atualizar o README e os documentos quando a API mudar.
- Manter os comandos documentados sincronizados com o `Makefile`.
- Manter o núcleo sem dependência de ncurses.
- Adicionar CI somente quando houver uma configuração de testes automatizados estável.
