# ft-gc

Garbage collector that follows the 42 school network's strict C code rules.

The flag `-DGC_DEBUG` can be passed to enable printing colored debug info.

# Running `test_small.c`

```sh
gcc -DGC_DEBUG -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined gc/*.c test_small.c -Igc -o test_small && ./test_small
```

# Running `test_large.c`

```sh
gcc -DGC_DEBUG -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined gc/*.c test_large.c -Igc -o test_large && ./test_large
```

# Running `test_eager.c`

```sh
gcc -DGC_DEBUG -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined gc/*.c test_eager.c -Igc -o test_eager && ./test_eager
```
