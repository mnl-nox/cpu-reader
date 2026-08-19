# Casos de uso do CPU Reader

## 1. Exibir informações da CPU

Uma aplicação chama `cpu_get_info()`, exibe modelo, número de processadores, threads, frequência e flags, e chama `cpu_free_info()` ao terminar de usar a estrutura.

**Resultado:** informações básicas do processador ficam disponíveis para a aplicação.

## 2. Medir uso agregado

Uma aplicação chama `cpu_get_usage()` periodicamente. A primeira leitura estabelece a referência; as chamadas seguintes calculam o percentual com base nos deltas de `/proc/stat`.

**Resultado:** a aplicação recebe uma estimativa de uso agregado entre `0` e `100`.

## 3. Monitorar no terminal

O executável `build/cpu-monitor` combina `cpu_get_info()` e `cpu_get_usage()`, atualiza a tela a cada segundo e encerra ao receber `q`.

**Resultado:** informações básicas e uso atual são exibidos em uma interface ncurses.

## 4. Tratar falhas de leitura

Quando `/proc/cpuinfo` não pode ser aberto ou a alocação falha, `cpu_get_info()` retorna `NULL`. Quando `/proc/stat` não pode ser lido, `cpu_get_usage()` retorna `-1.0f`.

**Resultado:** a aplicação pode detectar a falha sem acessar ponteiros inválidos.

## Fora do escopo atual

O projeto ainda não fornece dashboard web, alertas, balanceamento de carga, ajuste de frequência, histórico, logs ou leitura efetiva de temperatura.
