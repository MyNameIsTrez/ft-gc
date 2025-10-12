# ft-gc

Garbage collector that follows the 42 school network's strict C code rules.

# Running `test_small.c`

```sh
gcc -DGC_DEBUG -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined gc/*.c test_small.c -Igc -o test_small && ./test_small
```

# Running `test_large.c`

```sh
gcc -DGC_DEBUG -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined gc/*.c test_large.c -Igc -o test_large && ./test_large
```
