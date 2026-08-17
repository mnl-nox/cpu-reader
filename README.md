# CPU Reader

Biblioteca leve e eficiente em C para leitura e monitoramento de dados da CPU em tempo real.

<div align="center">

[![C Language](https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&logo=c&logoColor=white)](<https://en.wikipedia.org/wiki/C_(programming_language)>)
[![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)](https://www.linux.org/)
[![Git](https://img.shields.io/badge/Git-F05033?style=for-the-badge&logo=git&logoColor=white)](https://git-scm.com/)
[![GitHub](https://img.shields.io/badge/GitHub-181717?style=for-the-badge&logo=github&logoColor=white)](https://github.com/)
[![Make](https://img.shields.io/badge/Make-004B87?style=for-the-badge&logo=gnu&logoColor=white)](https://www.gnu.org/software/make/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

</div>

---

CPU Reader fornece interface C simples para acessar informações da CPU:

- Leitura de núcleos, threads, modelo e frequência
- Monitoramento em tempo real de uso de CPU
- Leitura de temperatura (quando disponível)
- Zero dependências externas
- Compatível com x86, ARM e RISC-V

## Requirements

- Linux kernel 4.4+
- GCC 5.0+ ou Clang 3.5+
- GNU Make

## Build

```bash
make              # Compilar biblioteca
make install      # Instalar
make clean        # Limpar
```

Installation:

```bash
make install PREFIX=/usr/local
---
```

```
cpu-reader/
├── Makefile
├── README.md
├── LICENSE
├── doc/
│   ├── requerimentos.md
│   ├── decisao-design.md
│   ├── prioridades-sdlc.md
│   ├── arquitetura.md
│   ├── casosdeuso.md
│   ├── criterios.md
│   └── diagramas.md
├── src/
│   ├── cpu.c
│   └── cpu.h
├── examples/
│   └── main.c
└── include/
    └── cpu.h
```
---

## Documentation

- [Requerimentos](doc/requerimentos.md) - Functional and non-functional requirements
- [Decisao Design](doc/decisao-design.md) - Architectural decisions
- [Prioridades SDLC](doc/prioridades-sdlc.md) - Development roadmap
- [Arquitetura](doc/arquitetura.md) - Internal design
- [Casos de Uso](doc/casosdeuso.md) - Use cases
- [Criterios](doc/criterios.md) - Acceptance criteria
- [Diagramas](doc/diagramas.md) - Architecture diagrams

## Contributing

1. Fork the project
2. Create a feature branch (`git checkout -b feature/Feature`)
3. Commit changes (`git commit -am 'Add Feature'`)
4. Push to branch (`git push origin feature/Feature`)
5. Open a Pull Request

Guidelines:

- C99+ compliance
- Test coverage >= 80%
- Update documentation
- No external dependencies

## License

MIT License - See [LICENSE](LICENSE) file

You are free to use, modify, and distribute this software.
Please include the license file when redistributing.
