# Testes e validação

## Testes funcionais

~~~sh
make test
~~~

## Sanitizers e hardening

~~~sh
make test-sanitize
make test-security
~~~

Os alvos podem limpar e recompilar os binários; execute-os em sequência, não em paralelo no mesmo clone.

## Análise estática

~~~sh
cppcheck --enable=warning,style,performance,portability --error-exitcode=1 --inline-suppr --std=c99 -Iinclude src tests examples bench
~~~

## Frontend e pacote

~~~sh
make test-gtk
make test-gtk-ui
make package-deb
desktop-file-validate packaging/debian/cpu-reader.desktop
dpkg-deb --info build/cpu-reader_*.deb
~~~

## CI

O workflow do GitHub Actions executa a matriz automatizada definida no repositório. Leia os logs do job correspondente antes de concluir que um teste passou. Artefatos gerados pelo workflow têm retenção limitada e não devem ser tratados como releases.
