# Guia de QA, validação e relatórios

## 1. Objetivo

Este documento padroniza como validar alterações no CPU Reader e como comunicar resultados técnicos sem exagerar a evidência. O projeto está em beta. O comportamento implementado deve ser verificado no código, nos testes e na CI; recursos descritos no roadmap não devem aparecer em relatórios como funcionalidades disponíveis.

## 2. Camadas de validação

| Camada | Comando / evidência | O que demonstra | O que não demonstra |
|---|---|---|---|
| Build do núcleo | `make clean && make core` | Compilação da biblioteca no ambiente atual | Correção funcional completa ou suporte a todo hardware |
| Testes funcionais | `make test` | Passagem dos testes incluídos para CPU, plataforma, diagnóstico e concorrência | Cobertura de todos os kernels e equipamentos |
| Sanitizers | `make test-sanitize` | Ausência de problemas detectados por ASan/UBSan nos caminhos exercitados | Ausência absoluta de bugs |
| Hardening | `make test-security` | Build/testes com flags de proteção configuradas | Auditoria de segurança completa |
| Portabilidade | `make test-portable` e CI multi-arquitetura | Evidência nos toolchains/arquiteturas efetivamente executados | Teste em hardware nativo quando executado sob emulação |
| GTK4 | `make gtk-monitor`, `make test-gtk`, `make test-gtk-ui` | Compilação e smoke tests da interface | Usabilidade visual em todos os ambientes gráficos |
| Pacote Debian | `make package-deb`, `dpkg-deb --info`, instalação e smoke tests | Integridade básica do pacote local | Release assinada ou publicação oficial |
| Análise estática | comando cppcheck documentado em [VALIDATION.md](../VALIDATION.md) | Alertas detectados pela configuração usada | Prova formal de ausência de defeitos |

Os alvos que limpam e recompilam artefatos não devem ser executados simultaneamente no mesmo clone.

## 3. Fluxo recomendado para uma alteração

1. Relacionar a mudança a uma issue e registrar critérios de aceite.
2. Compilar o núcleo sem dependências gráficas.
3. Adicionar testes determinísticos para sucesso, entrada malformada, indisponibilidade, limites e falhas de I/O relevantes.
4. Executar `make test`.
5. Para parsing, memória, aritmética de contadores ou ownership, executar também `make test-sanitize` e `make test-security`.
6. Se houver alteração no GTK4 ou empacotamento, executar os alvos correspondentes e validar o pacote.
7. Inspecionar diff, mensagens de erro, documentação, compatibilidade da API e dados sensíveis.
8. Incluir no PR comandos realmente executados, resultado e limitações conhecidas.
9. Manter o PR em draft se a compilação ou os testes ainda não tiverem evidência suficiente.

## 4. Contrato mínimo para telemetria

Para métricas novas, documentar antes de integrar:

- Nome e significado exato da métrica.
- Unidade e intervalo válido.
- Fonte do sistema e escopo (host, namespace ou cgroup, quando aplicável).
- Estado explícito: valor válido, baseline, indisponível ou erro.
- Timestamp e base temporal usados para taxas.
- Ownership, ciclo de vida e contrato de concorrência.
- Política para overflow, regressão de contadores, reinicialização e remoção de dispositivos.
- Testes com fixtures que não dependam do hardware do desenvolvedor.

Não converter indisponibilidade em zero. Não chamar capacidade de filesystem de I/O de disco. Não chamar contagem de processos runnable de inventário total de processos.

## 5. Modelo de relatório de validação

Copie e preencha este modelo para um PR ou release candidate:

```text
CPU Reader — Relatório de validação
Data/hora:
Commit:
Branch / PR:
Sistema e versão:
Kernel:
Arquitetura:
Compilador e versão:

Escopo da alteração:
Critérios de aceite:

Comandos executados:
- [comando]: PASS / FAIL / NÃO EXECUTADO
  Evidência / observação:

Testes:
- Build do núcleo:
- Testes funcionais:
- Sanitizers:
- Hardening:
- Portabilidade:
- GTK4:
- Pacote:
- Análise estática:

Defeitos conhecidos:
Limitações de plataforma:
Dados omitidos/redigidos:
Conclusão:
```

Use **NÃO EXECUTADO** quando não houver evidência. Não marque um teste como aprovado com base apenas na expectativa de que a CI o execute.

## 6. Relatório de estado do projeto

Na data desta documentação, a situação técnica é:

- **Implementado:** biblioteca C para informações de CPU, uso agregado, frequência/temperatura opcionais e API de diagnóstico; ferramentas de terminal/GTK4 e empacotamento fazem parte do repositório.
- **Em evolução:** monitoramento de sistema completo, com novos coletores públicos e contratos consistentes.
- **Planejado por issues:** API de telemetria, CPU por processador lógico, memória/swap/PSI, carga e processos, rede/armazenamento e reforço de CI.
- **Não declarado como validado nesta mudança:** compilação, execução dos testes, instalação do pacote e inspeção visual.
- **Limite de confiança:** as métricas são observações do sistema operacional; não constituem atestação de hardware.

Consulte o [roadmap](system-telemetry-roadmap.md) para a sequência e os critérios por iteração. O estado das issues e da CI pode mudar depois da atualização deste documento.

## 7. Política de relatórios gerados

O alvo `make telemetry` depende do sucesso de `make test` e grava um JSON local em `build/telemetry.json`. Esse arquivo é um resumo de execução, não um relatório forense nem uma certificação. Confirme o commit, ambiente e logs associados antes de compartilhá-lo.

Não publique dados brutos de `/proc/cpuinfo`, variáveis de ambiente, nomes de host ou caminhos pessoais sem revisão. Prefira fixtures sintéticas e recortes mínimos.

## 8. Critérios de bloqueio

Não integrar uma alteração quando:

- o núcleo não compila com as flags padrão;
- testes relevantes falham ou não foram executados sem justificativa explícita;
- há comportamento indefinido, acesso fora dos limites ou vazamento confirmado;
- erros e indisponibilidade são silenciosamente convertidos em valores válidos;
- a documentação descreve como implementado um recurso apenas planejado;
- a API pública altera comportamento ou layout legado sem plano de compatibilidade;
- o PR não registra limitações ou resultados verificáveis.
