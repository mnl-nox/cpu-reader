# Prioridades de desenvolvimento do CPU Reader

Este roadmap parte do estado atual do repositório, que já possui biblioteca, monitor, testes básicos e build com Make.

## Concluído

- [x] Estrutura `include/`, `src/`, `examples/`, `tests/` e `build/`.
- [x] API pública para informações, uso, temperatura, clock, processos ativos, inicialização e liberação.
- [x] Leitura de `/proc/cpuinfo`.
- [x] Cálculo de uso agregado com `/proc/stat`.
- [x] Monitor de terminal com ncurses.
- [x] Teste básico executável com `make test`.
- [x] Build reprodutível com `make all` e limpeza com `make clean`.
- [x] Núcleo separado em módulos de informações, uso e fachada da API.
- [x] Testes de campos completos de `cpu_info_t`, contextos nulos e reinicialização.
- [x] Leitura de temperatura, clock atual e processos ativos com fontes de dados injetáveis para testes.
- [x] Workflow de tags semânticas baseado em commits convencionais.

## Próxima prioridade: robustez da biblioteca

1. [x] Adicionar testes para falha de arquivos e parsing com fontes de dados injetáveis.
2. Verificar o comportamento em arquiteturas e formatos de `/proc` diferentes.
3. Executar testes com AddressSanitizer e verificar vazamentos em CI.

## Próxima prioridade: recursos de CPU

1. Adicionar métricas por núcleo.
2. Definir claramente a diferença entre processadores lógicos e núcleos físicos.
3. [x] Melhorar o tratamento e a exposição de erros.

## Manutenção do projeto

- Atualizar o README e os documentos quando a API mudar.
- Manter os comandos documentados sincronizados com o `Makefile`.
- Manter o núcleo sem dependência de ncurses.
- Manter o workflow de tags limitado à branch `main` e aos prefixos de commit documentados.
