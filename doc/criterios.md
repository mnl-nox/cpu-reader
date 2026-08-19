# Critérios de aceitação do CPU Reader

## Build

- [x] `make all` compila `libcpu.a` e `build/cpu-monitor` com C99 e as flags de warnings do projeto.
- [x] `make test` compila e executa `build/test_cpu`.
- [x] `make clean` remove os artefatos `build/` e `libcpu.a`.

## API

- [x] `cpu_get_info()` retorna uma estrutura válida em um Linux com `/proc/cpuinfo`.
- [x] A estrutura contém processadores, threads online, modelo, frequência e flags quando as chaves existem no arquivo.
- [x] `cpu_free_info()` libera a estrutura retornada.
- [x] `cpu_get_usage()` retorna valor entre `0` e `100` para leituras válidas.
- [x] Falhas de leitura de informações retornam `NULL`.
- [x] Falhas de leitura de uso retornam `-1.0f`.

## Monitor

- [x] `build/cpu-monitor` inicia em um terminal com ncurses instalado.
- [x] O monitor atualiza os dados aproximadamente a cada segundo.
- [x] A tecla `q` encerra a aplicação.

## Limitações conhecidas

- [ ] `cpu_get_temperature()` ainda não lê sensores e retorna `-1.0f`.
- [ ] Não existem testes por núcleo, de temperatura, de portabilidade ou de performance.
- [ ] Não há medição formal de cobertura, execução com sanitizers ou integração contínua.
- [ ] A API de uso não é thread-safe.
