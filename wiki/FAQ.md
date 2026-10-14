# Perguntas frequentes

## O CPU Reader funciona no Windows ou macOS?

Não. O projeto usa interfaces do Linux, principalmente <code>/proc</code> e <code>/sys</code>.

## Por que temperatura não aparece?

O kernel precisa expor um sensor compatível. Algumas máquinas não oferecem essa leitura ao usuário ou usam drivers específicos.

## Por que a primeira leitura de uso é zero?

O cálculo compara duas amostras. A primeira estabelece o baseline; as próximas calculam a variação.

## O .deb funciona em qualquer distribuição?

O pacote foi pensado para distribuições Debian e derivadas compatíveis com suas dependências. Ele é gerado para a arquitetura local e não é um pacote universal para todas as distribuições.

## Existe release estável?

Consulte a página de releases e o histórico do projeto. Um artefato temporário de CI não é uma release oficial.

## Onde encontro a referência da API?

Em [API C](API.md) e no cabeçalho público <code>include/cpu.h</code>.
