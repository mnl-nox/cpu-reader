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
- [x] Núcleo separado em módulos de informações, uso e fachada da API.
- [x] Testes de campos completos de `cpu_info_t`, contextos nulos e reinicialização.

## Próxima prioridade: robustez da biblioteca

1. Adicionar testes para falha de arquivos e parsing com fontes de dados injetáveis.
2. Verificar o comportamento em arquiteturas e formatos de `/proc` diferentes.
3. Executar testes com AddressSanitizer e verificar vazamentos em CI.

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
