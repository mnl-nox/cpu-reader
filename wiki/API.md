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

O contexto implícito de uso e o relatório legado de erro são locais à thread em compiladores GCC/Clang. Contextos explícitos não devem ser compartilhados simultaneamente por threads sem sincronização externa. Prefira as variantes <code>*_ex</code> para associar o diagnóstico à chamada.

## Diagnóstico estruturado\n\nPara um snapshot que mantenha status, código de erro, mensagem e família de fonte para cada métrica, consulte [API de diagnóstico](Diagnostic-API.md) e o cabeçalho público `include/cpu_diagnostics.h`. A API nova é aditiva e não altera o layout de `cpu_info_t`.\n\n## Compilação

~~~sh
make core
~~~

Para vincular a biblioteca estática, inclua <code>include/</code> e aponte o linker para <code>libcpu.a</code>. Consulte os exemplos existentes e o Makefile para as flags utilizadas.
