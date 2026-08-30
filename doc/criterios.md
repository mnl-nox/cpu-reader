# Critérios de aceitação do CPU Reader

## Build

- [x] `make all` compila `libcpu.a` e `build/cpu-monitor` com C99 e as flags de warnings do projeto.
- [x] `make test` compila e executa `build/test_cpu`.
- [x] `make clean` remove os artefatos `build/` e `libcpu.a`.

## API

- [x] `cpu_get_info()` retorna uma estrutura válida em um Linux com `/proc/cpuinfo`.
- [x] A estrutura contém processadores, threads online, modelo, frequência e flags quando as chaves existem no arquivo.
- [x] A estrutura inclui processos ativos e temperatura quando as fontes Linux correspondentes estão disponíveis.
- [x] `cpu_free_info()` libera a estrutura retornada.
- [x] `cpu_get_usage()` retorna valor entre `0` e `100` para leituras válidas.
- [x] Falhas de leitura de informações retornam `NULL`.
- [x] Falhas de leitura de uso retornam `-1.0f`.
- [x] Falhas expõem código e mensagem por `cpu_get_last_error_code()` e `cpu_get_last_error()`.
- [x] Variáveis `CPU_READER_CPUINFO_PATH`, `CPU_READER_PROC_STAT_PATH`, `CPU_READER_CPU_TEMP_PATH`, `CPU_READER_CPU_FREQ_PATH` e `CPU_READER_LOADAVG_PATH` permitem redirecionar as leituras para fixtures de teste.
- [x] A soma de contadores usa assembly inline em `x86_64` com fallback C nas demais plataformas.

## Monitor

- [x] `build/cpu-monitor` inicia em um terminal com ncurses instalado.
- [x] O monitor atualiza os dados aproximadamente a cada segundo.
- [x] A tecla `q` encerra a aplicação.

## Limitações conhecidas

- [ ] A temperatura depende de sensores expostos por `sysfs`; em máquinas sem sensor compatível, `cpu_get_temperature()` retorna `-1.0f`.
- [ ] Não existem testes por núcleo, de portabilidade ampla ou de performance.
- [ ] Não há medição formal de cobertura, execução com sanitizers ou integração contínua.
- [ ] A API de uso não é thread-safe.
