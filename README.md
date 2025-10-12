# ft-gc

Garbage collector that follows the 42 school network's strict C code rules.

# Running `tests.c`

Without `-fsanitize=address,undefined`, the test passes, but uses UB:
```sh
gcc -Wall -Wextra -Wpedantic -Werror -g gc/*.c tests.c -Igc -o tests && ./tests
```

With `-fsanitize=address,undefined` the test crashes:
```sh
gcc -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined gc/*.c tests.c -Igc -o tests && ./tests
```
