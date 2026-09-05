# BUILD.md - Instruções de Build do CPU Reader

**Versão:** 2.0 
**Data:** 2026-09-04 
**Status:** ATUALIZADO

---

## Pré-requisitos

### Sistema Operacional

- **Linux** (kernel 3.10+)
 - Debian/Ubuntu (14.04+)
 - RHEL/CentOS (7.0+)
 - Alpine Linux (3.10+)

### Compiladores

- **GCC** 4.8+ com suporte a C99
 - OU
- **Clang** 3.5+ com suporte a C99

### Ferramentas de Build

- **GNU Make** 3.81+
- **GNU Binutils** (ar, gcc)

### Dependências Opcionais

- **ncurses-dev** (libncurses5-dev, libncursesw5-dev) - para compilar o monitor
 - Debian/Ubuntu: `apt-get install libncurses-dev`
 - RHEL/CentOS: `yum install ncurses-devel`
 - Alpine: `apk add ncurses-dev`

### Dependências para Teste (Planejadas - Semana 2)

- **Valgrind** - verificação de vazamento de memória
- **AddressSanitizer** - já incluído em gcc/clang modernos
- **gcov** - para cobertura de testes

---

## Build Padrão

### Compilação Completa (Biblioteca + Monitor + Testes)

```bash
make # equivalente a: make all
```

**Saída esperada:**

```
cc -std=c99 -Wall -Wextra -Wpedantic -Iinclude -c -o build/cpu.o src/cpu.c
cc -std=c99 -Wall -Wextra -Wpedantic -Iinclude -c -o build/cpu_info.o src/cpu_info.c
cc -std=c99 -Wall -Wextra -Wpedantic -Iinclude -c -o build/cpu_usage.o src/cpu_usage.c
ar rcs libcpu.a build/cpu.o build/cpu_info.o build/cpu_usage.o
cc -std=c99 -Wall -Wextra -Wpedantic -Iinclude -o build/cpu-monitor examples/monitor.c -L. -lcpu -lncurses
cc -std=c99 -Wall -Wextra -Wpedantic -Iinclude -o build/test_cpu tests/test_cpu.c -L. -lcpu
```

**Sem warnings ou erros** (RNF-001)

### Compilação da Biblioteca Apenas

```bash
make libcpu.a
```

**Saída:**

- `libcpu.a` - arquivo da biblioteca estática

### Compilação do Monitor

```bash
make monitor
```

**Saída:**

- `build/cpu-monitor` - executável do monitor ncurses

### Compilação dos Testes

```bash
make build/test_cpu
```

**Saída:**

- `build/test_cpu` - executável dos testes

---

## Testes

### Executar Testes

```bash
make test
```

**O que valida:**

- RF-001: Inicialização (`cpu_init()`)
- RF-002: Limpeza (`cpu_cleanup()`)
- RF-003: Informações estáticas (`cpu_get_info()`)
- RF-004: Liberação de memória (`cpu_free_info()`)
- RF-005: Uso agregado (`cpu_get_usage()`)
- RF-006: Contextos independentes (`cpu_usage_context_*()`)
- RF-007: Temperatura (`cpu_get_temperature()`)
- RF-008: Clock speed (`cpu_get_clock_speed()`)
- RF-009: Processos ativos (`cpu_get_active_processes()`)
- RF-010: Tratamento de erros (`cpu_get_last_error_*()`)

**Esperado:** Execução sem crashes ou erros de segmentação

---

## Limpeza

```bash
make clean
```

**Remove:**

- `build/` - diretório com objetos compilados e executáveis
- `libcpu.a` - biblioteca estática

---

## Execução

### Monitor de Terminal

```bash
./build/cpu-monitor
```

**Funcionalidades:**

- Exibe cores, threads, modelo, frequência
- Mostra uso agregado (%)
- Mostra temperatura (se disponível)
- Mostra clock speed (se disponível)
- Atualiza a cada ~1 segundo
- Pressione `q` para sair

### Testes Unitários

```bash
./build/test_cpu
```

**Saída esperada:**

- Testes passam sem erros
- Sem vazamento de memória (com Valgrind)

---

## Build com Flags Adicionais

### Compilação com Sanitizers (Semana 2)

```bash
# AddressSanitizer (detecção de erros de memória)
make CFLAGS="-std=c99 -Wall -Wextra -Wpedantic -fsanitize=address -g"

# UndefinedBehaviorSanitizer
make CFLAGS="-std=c99 -Wall -Wextra -Wpedantic -fsanitize=undefined -g"

# Ambos
make CFLAGS="-std=c99 -Wall -Wextra -Wpedantic -fsanitize=address,undefined -g"
```

### Compilação com Coverage (Semana 2)

```bash
make CFLAGS="-std=c99 -Wall -Wextra -Wpedantic --coverage -g"
```

Depois:

```bash
./build/test_cpu
gcov src/cpu.c src/cpu_info.c src/cpu_usage.c
```

### Debug Build

```bash
make CFLAGS="-std=c99 -Wall -Wextra -Wpedantic -g -O0"
```

---

## Validação de Build

### Verificar Compilação sem Warnings

```bash
make clean && make 2>&1 | grep -i warning
```

**Esperado:** Sem saída (nenhum warning)

### Verificar Tamanho da Biblioteca

```bash
ls -lh libcpu.a
```

**Esperado:** ~50-100 KB (binário pequeno)

### Verificar Símbolos da Biblioteca

```bash
nm libcpu.a | grep " T " | grep cpu_
```

**Esperado:** 13 funções públicas começando com `cpu_`

---

## Troubleshooting

### Erro: `make: gcc: comando não encontrado`

**Solução:**

```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# RHEL/CentOS
sudo yum install gcc make

# Alpine
apk add gcc make
```

### Erro: `fatal error: ncurses.h: Arquivo ou diretório não encontrado`

**Solução:**

```bash
# Ubuntu/Debian
sudo apt-get install libncurses-dev

# RHEL/CentOS
sudo yum install ncurses-devel

# Alpine
apk add ncurses-dev
```

### Erro: `error: undefined reference to 'endwin'`

**Causa:** ncurses não foi encontrado durante a linkagem 
**Solução:** Instalar libncurses-dev conforme acima

### Aviso: `warning: implicit declaration of function`

**Causa:** Função não declarada no header 
**Solução:** Verificar se `#include` está correto em `include/cpu.h`

---

## Checklist de Build

- [ ] Sistema Linux verificado
- [ ] Compilador (gcc/clang) instalado
- [ ] GNU Make instalado
- [ ] ncurses-dev instalado (opcional, para monitor)
- [ ] `make clean` executado
- [ ] `make` sem warnings
- [ ] `make test` sem erros
- [ ] `./build/cpu-monitor` funciona
- [ ] `make clean` remove artefatos

---

## Referências

- [README.md](../README.md) - Visão geral do projeto
- [doc/requisitos-nao-funcionais.md](requisitos-nao-funcionais.md) - RNF-001, RNF-002, RNF-003
- [Makefile](../Makefile) - Configuração completa de build
- [Linux C99 Build Best Practices](https://en.wikipedia.org/wiki/C99)

---

**Data:** 2026-09-04 
**Versão:** 2.0 
**Status:** ATUALIZADO
