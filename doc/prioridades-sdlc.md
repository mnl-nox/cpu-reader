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

## Próxima prioridade: robustez da biblioteca (P1 - Semana 1-2)

1. [x] Adicionar testes para falha de arquivos e parsing com fontes de dados injetáveis.
2. [x] Verificar o comportamento em arquiteturas e formatos de `/proc` diferentes (com fixtures).
3. [ ] Executar testes com AddressSanitizer e verificar vazamentos em CI.
4. [ ] Adicionar suite de testes para cobertura >80%.
5. [ ] Validar em múltiplas distribuições Linux (Alpine, Debian, Ubuntu, RHEL).

## Próxima prioridade: recursos de CPU (P2 - Semana 3-4)

1. [ ] Adicionar métricas por núcleo (`cpu_get_core_info()` e `cpu_get_core_usage()`).
2. [ ] Definir claramente a diferença entre processadores lógicos e núcleos físicos.
3. [x] Melhorar o tratamento e a exposição de erros.
4. [ ] Adicionar cache configurável para resultados de leitura.
5. [ ] Implementar API thread-safe para contextos compartilhados.

## Próxima prioridade: performance e otimização (P3 - Semana 5-6)

1. [ ] Benchmarks de leitura de `/proc/cpuinfo` e `/proc/stat`.
2. [ ] Otimizar parsing com buffering.
3. [ ] Avaliar alternativas a assembly inline (intrinsics, simd).
4. [ ] Implementar modo low-power para dispositivos embarcados.

## Próxima prioridade: integração contínua (P4 - Semana 7-8)

1. [ ] Configurar GitHub Actions ou CI equivalente.
2. [ ] Adicionar lint (clang-tidy, cppcheck).
3. [ ] Adicionar cobertura com gcov.
4. [ ] Criar workflow de release automático.
5. [ ] Construir binários para múltiplas plataformas.

## Manutenção do projeto

- Atualizar o README e os documentos quando a API mudar.
- Manter os comandos documentados sincronizados com o `Makefile`.
- Manter o núcleo sem dependência de ncurses.
- Manter o workflow de tags limitado à branch `main` e aos prefixos de commit documentados.
- Revisar ADRs antes de mudanças arquiteturais significativas.
- Manter requisitos funcionais e não funcionais sincronizados.
