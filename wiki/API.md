# Referência da API C

A API pública está declarada em `include/cpu.h` e `include/cpu_diagnostics.h`. Inclua os cabeçalhos públicos; não inclua arquivos internos de `src/`. A API e a biblioteca estão em evolução beta.

## Exemplo mínimo

```c
#include <stdio.h>
#include "cpu.h"

int main(void)
{
    cpu_info_t *info = cpu_get_info();

    if (info == NULL) {
        fprintf(stderr, "Falha ao consultar CPU: %s\n",
                cpu_get_last_error());
        return 1;
    }

    printf("Modelo: %s\n", info->model);
    cpu_free_info(info);
    return 0;
}
```

Compile a partir da raiz do repositório:

```sh
make core
cc -std=c99 -Wall -Wextra -Wpedantic -Iinclude exemplo.c -L. -lcpu -o exemplo
./exemplo
```

## Funções principais

| Função | Descrição |
|---|---|
| `cpu_init()` / `cpu_cleanup()` | Inicializa ou reinicia o contexto padrão de amostragem por thread. |
| `cpu_get_info()` | Retorna um snapshot alocado de informações de CPU ou `NULL`. |
| `cpu_free_info()` | Libera o snapshot retornado por `cpu_get_info()`. Aceita `NULL`. |
| `cpu_get_usage()` | Calcula o uso agregado entre amostras no contexto padrão. |
| `cpu_usage_context_init()` | Inicializa um contexto explícito de amostragem independente. |
| `cpu_get_usage_context()` | Calcula o uso agregado usando um contexto explícito. |
| `cpu_usage_context_cleanup()` | Reinicia/limpa o contexto explícito. |
| `cpu_get_temperature()` | Consulta temperatura em graus Celsius quando disponível. |
| `cpu_get_clock_speed()` | Consulta frequência atual em MHz quando disponível. |
| `cpu_get_active_processes()` | Retorna a contagem runnable informada por `/proc/loadavg`. |
| Variantes `*_ex` | Associam um snapshot de erro à operação por meio de `cpu_error_info_t`. |

Para o relatório estruturado por métrica, consulte [API de diagnóstico](Diagnostic-API.md).

## Retornos e valores sentinela

- `cpu_get_info()`: ponteiro para snapshot ou `NULL` em falha.
- `cpu_get_usage*()`: percentual calculado entre 0 e 100; `-1.0f` em falha. A primeira amostra de um contexto estabelece baseline e retorna `0.0f` na API legada.
- `cpu_get_temperature()` e `cpu_get_clock_speed()`: `-1.0f` quando indisponíveis ou inválidas.
- `cpu_get_active_processes()`: `-1` em falha.
- `physical_cores == 0`: topologia física desconhecida ou indisponível, não ausência de núcleos.
- `logical_processors`: quantidade de entradas de processador lógico identificadas na fonte lida.
- `threads`: campo legado baseado em processadores online visíveis via `sysconf`; não é sinônimo obrigatório de `logical_processors`.

Não apresente valores sentinela como medições. Para novas APIs, o projeto está migrando para estados explícitos por métrica.

## Ownership e ciclo de vida

- O consumidor é responsável por liberar cada snapshot `cpu_info_t *` com `cpu_free_info()`.
- O snapshot retornado não deve ser usado depois de liberado.
- Estruturas `cpu_error_info_t` fornecidas ao chamador são de propriedade do chamador.
- Não mantenha ponteiros para buffers locais depois do retorno de uma função.
- Sempre verifique o retorno antes de ler os campos de uma estrutura.

## Erros

As funções legadas `cpu_get_last_error_code()` e `cpu_get_last_error()` fornecem o diagnóstico da operação recente na thread atual. Consulte-as imediatamente após a chamada que falhou.

As variantes `*_ex` recebem um `cpu_error_info_t *` pertencente ao chamador e tornam o erro associado à operação explícito. Use `cpu_error_info_clear()` quando desejar limpar a estrutura. Veja os tipos e contratos exatos nos cabeçalhos públicos.

## Contextos e concorrência

| Recurso | Contrato |
|---|---|
| Contexto padrão | O estado de amostragem é local à thread no toolchain suportado; chamadas na mesma thread compartilham o baseline padrão. |
| `cpu_usage_context_t` | Use um contexto independente por thread. Se for compartilhado, o consumidor deve sincronizar todo acesso mutável. |
| `cpu_error_info_t` | A estrutura pertence ao chamador; não compartilhe a mesma instância mutável sem sincronização. |
| Variáveis `CPU_READER_*_PATH` | Configure antes de iniciar threads. Alterar o ambiente concorrentemente com leituras não é seguro. |

A existência de estado local à thread não torna um mesmo contexto explícito seguro para uso concorrente. A biblioteca não oferece um lock implícito para contextos pertencentes ao consumidor.

## Fontes de dados para testes

A implementação suporta overrides de caminho para algumas fontes, incluindo `CPU_READER_CPUINFO_PATH`, `CPU_READER_PROC_STAT_PATH`, `CPU_READER_LOADAVG_PATH`, `CPU_READER_CPU_TEMP_PATH`, `CPU_READER_THERMAL_PATH` e `CPU_READER_CPU_FREQ_PATH`. Consulte [arquitetura](../doc/arquitetura.md) e os testes para os formatos exatos.

Configure overrides antes de iniciar threads e use arquivos de fixture controlados. Não use esses overrides como mecanismo de configuração de segurança para processos não confiáveis.

## Limites da API atual

A API existente fornece informações de CPU e uso agregado, temperatura/frequência opcionais, contagem runnable e diagnóstico. A coleta pública de uso por CPU lógica, memória/swap/PSI, rede e I/O por dispositivo é planejada, não deve ser inferida da interface GTK. Acompanhe o [roadmap](../doc/system-telemetry-roadmap.md).

A biblioteca observa dados expostos pelo sistema operacional. Não fornece atestação criptográfica de hardware, kernel ou firmware, nem garante calibração física de sensores.
