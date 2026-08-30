# Casos de uso do CPU Reader

## 1. Exibir informações da CPU

Uma aplicação chama `cpu_get_info()`, exibe modelo, número de processadores, threads, frequência, processos ativos, temperatura e flags, e chama `cpu_free_info()` ao terminar de usar a estrutura.

**Resultado:** informações básicas do processador ficam disponíveis para a aplicação.

## 2. Medir uso agregado

Uma aplicação chama `cpu_get_usage()` periodicamente. A primeira leitura estabelece a referência; as chamadas seguintes calculam o percentual com base nos deltas de `/proc/stat`.

**Resultado:** a aplicação recebe uma estimativa de uso agregado entre `0` e `100`.

## 3. Monitorar no terminal

O executável `build/cpu-monitor` combina `cpu_get_info()`, `cpu_get_usage()`, `cpu_get_clock_speed()` e `cpu_get_temperature()`, atualiza a tela a cada segundo e encerra ao receber `q`.

**Resultado:** informações básicas e uso atual são exibidos em uma interface ncurses.

## 4. Tratar falhas de leitura

Quando uma fonte de dados não pode ser lida, as funções retornam seu valor de erro (`NULL`, `-1.0f` ou `-1`, conforme a API). A aplicação pode consultar `cpu_get_last_error_code()` e `cpu_get_last_error()` antes de nova chamada para obter o relatório.

**Resultado:** a aplicação pode detectar a falha sem acessar ponteiros inválidos.

## Fora do escopo atual

O projeto ainda não fornece dashboard web, alertas, balanceamento de carga, ajuste de frequência, histórico, logs ou métricas individuais por núcleo.
