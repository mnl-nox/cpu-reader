# Referência da API C

A API pública está declarada em <code>include/cpu.h</code>. Use o cabeçalho público em vez de incluir arquivos internos de <code>src/</code>.

## Ciclo de vida básico

~~~c
#include <stdio.h>
#include "cpu.h"

int main(void) {
    cpu_info_t *info = cpu_get_info();
    if (info == NULL) {
        fprintf(stderr, "Falha ao consultar CPU: %s\\n", cpu_get_last_error());
        return 1;
    }

    printf("%s\\n", info->model);
    cpu_free_info(info);
    return 0;
}
~~~

Snapshots retornados por <code>cpu_get_info()</code> devem ser liberados com <code>cpu_free_info()</code>.

## Funções principais

| Função | Descrição |
|---|---|
| <code>cpu_init()</code> / <code>cpu_cleanup()</code> | Reinicializa o estado do contexto padrão |
| <code>cpu_get_info()</code> | Retorna um snapshot de informações estáticas |
| <code>cpu_get_usage()</code> | Calcula uso agregado entre amostras |
| <code>cpu_usage_context_init()</code> | Inicializa contexto de uso independente |
| <code>cpu_get_usage_context()</code> | Calcula uso para contexto explícito |
| <code>cpu_get_temperature()</code> | Consulta temperatura em graus Celsius |
| <code>cpu_get_clock_speed()</code> | Consulta frequência em MHz |
| <code>cpu_get_active_processes()</code> | Retorna a contagem de processos em execução |
| <code>cpu_get_info_ex()</code> e demais variantes <code>*_ex</code> | Capturam informações de erro em estrutura pertencente ao chamador |

## Valores de retorno

- <code>cpu_get_info()</code>: ponteiro para snapshot ou <code>NULL</code>.
- <code>cpu_get_usage*()</code>: percentual de 0 a 100; primeira amostra estabelece baseline e retorna zero. Erro de leitura/parsing pode retornar <code>-1.0f</code>.
- <code>cpu_get_temperature()</code> e <code>cpu_get_clock_speed()</code>: <code>-1.0f</code> quando indisponíveis.
- <code>cpu_get_active_processes()</code>: <code>-1</code> em falha.
- <code>physical_cores == 0</code>: topologia física desconhecida ou indisponível.

## Contextos e concorrência

| Operação/estado | Contrato |
|---|---|
| Contexto de uso padrão | Estado local à thread em GCC/Clang; chamadas da mesma thread compartilham o baseline. |
| Contexto explícito `cpu_usage_context_t` | Cada thread deve possuir seu próprio contexto ou sincronizar todo acesso compartilhado. A biblioteca não coloca locks no contexto do chamador. |
| Erro legado `cpu_get_last_error*()` | Estado local à thread; consulte imediatamente após a chamada. |
| Snapshot `cpu_error_info_t` via `*_ex` | Pertence ao chamador e permanece estável até ser limpo ou sobrescrito pelo próprio chamador. |
| Variáveis `CPU_READER_*_PATH` | São variáveis de ambiente do processo. Configure-as antes de iniciar threads; não use `setenv()`/`unsetenv()` concorrentemente com leituras. |
| Relatório `cpu_diagnostic_report_t` | Estrutura pertencente ao chamador; use uma instância por operação/thread ou sincronize acessos. |

O núcleo evita locks globais e permite amostragem concorrente com contextos independentes. As funções de leitura podem ser chamadas simultaneamente, mas o contexto mutável e as estruturas de saída continuam sob responsabilidade do consumidor. As strings de origem/sensor do relatório são copiadas para a estrutura retornada, sem ponteiros para buffers internos. Prefira as variantes `*_ex` para associar o diagnóstico à chamada.

## Diagnóstico estruturado\n\nPara um snapshot que mantenha status, código de erro, mensagem e família de fonte para cada métrica, consulte [API de diagnóstico](Diagnostic-API.md) e o cabeçalho público `include/cpu_diagnostics.h`. A API nova é aditiva e não altera o layout de `cpu_info_t`.\n\n## Compilação

~~~sh
make core
~~~

Para vincular a biblioteca estática, inclua <code>include/</code> e aponte o linker para <code>libcpu.a</code>. Consulte os exemplos existentes e o Makefile para as flags utilizadas.
