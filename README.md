# Testerka do librstack.so
![podgląd](./preview.png)

## Instalacja:
Sklonuj to repozytorium w podkatalogu w korzeniu swojego projektu:
```
git clone https://github.com/ignacypekala/rstack_tests ./rstack_tests
```

# Użycie
Aby uruchomić wszystkie testy:
```
./test-all.sh
```

Aby uruchomić wszystkie testy z packi:
```
./test-batch.sh NAZWAPACZKI
```

Aby uruchomić pojedyńczy test:
```
./test.sh NAZWAPACZKI NAZWATESTU (NAZWAPRZYPADKU)
```
> Żeby skorzystać z domyślnego przypadku należy nie podawać trzeciego argumentu.

Na początku skryptów `test.sh` i `test-batch.sh` znajdują się proste ustawienia
które można nadpisać.


## Struktura testów
### Test
Każdy test jest reprezentowany przez osobny program w C w pliku:
```
tests_NAZWAPACZKI/NAZWATESTU.c.
```

Jeden test może mieć dowolną liczbę przypadków (test cases) w folderze:
```
tests_NAZWAPACZKI/NAZWATESTU/
```
albo jeden domyślny przypadek w tym samym folderze co kod źródłowy. W takiej
sytuacji nazwą przypadku jest nazwa testu.

### Przypadek
Każdy przypadek może mieć po jednym z plików
- `NAZWAPRZYPADKU.args` - argumenty wywoływania programu
- `NAZWAPRZYPADKU.in` - tekst do wpisania na wejście standardowe
- `NAZWAPRZYPADKU.stdout` - tekst do porównania wyjścia standardowego
- `NAZWAPRZYPADKU.fout` - tekst do porównania wypisanego pliku `test.fout`
(makro OUTPUT_FILE)

Ponadto przypadek może także generować dowolną liczbę nazwanych plików wynikowych:
- `NAZWAPRZYPADKU_NAZWAPLIKU.fout` - plik porównywany z
`test_NAZWAPLIKU.fout` (makro TEST_FILE(nazwa))

# Autorzy
Repozytorium zawiera testy napisane przeze mnie, ale także zewnętrzne testy
innych autorów zaadaptowane do formatu wymaganego przez testerkę. Lista
zewnętrznych paczek testów:

- tests_michal_bak - Michał Bąk
- tests_example - testy dołączone do treści zadania

Ponadto pliki `memory_tests.c`, `memory_tests.h` oraz `rstack.h` pochodzą z treści
zadania. Natomiast plik `macros.h` zawiera makra bazowane na makrach z pliku
`rstack_example.c` z treści zadania (nie jest on tutaj załączony).

# Contributing
Zachęcam do dzielenia się swoimi testami oraz zgłaszania wszelkich usterek.
W obu przypadkach można otwierać issues lub pisać do mnie prywatnie.
