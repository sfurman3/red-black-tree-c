UNAME_S := $(shell uname -s)
cc :=
ifeq ($(UNAME_S),Linux)
    cc = gcc
endif
ifeq ($(UNAME_S),Darwin)
    cc = gcc-6
endif

DEBUG_FLAGS := -D ALLOC_TRACK -D REP_OK -O0 -g
#DEBUG_FLAGS := -D ALLOC_TRACK -O0 -g

# Red-Black Trees
rbt.o: rbt.c rbt.h
	$(cc) -c $+

tests: rbt.o rbt_test.c
	$(cc) $+ -o rbt_test

run: clean tests
	./rbt_test

# Compile and run (with debugging symbols).
# Print the number of nodes allocated and freed during execution.
rbt.o_debug: rbt.c rbt.h
	$(cc) -c $(DEBUG_FLAGS) $+

rbt_test: rbt.o_debug rbt_test.c
	$(cc) rbt.o rbt_test.c $(DEBUG_FLAGS) -o $@

test: rbt_test
	./rbt_test

# Compile and run (with debugging symbols) using valgrind's memcheck tool.
# NOTE: --leak-check=full generates a lot of false errors.
#    valgrind -q --leak-check=full ./rbt_test
valgrind: rbt_test
ifeq ($(UNAME_S),Linux)
	valgrind -q --leak-check=full ./rbt_test
endif
ifeq ($(UNAME_S),Darwin)
	valgrind -q ./rbt_test
endif

clean:
	rm -rf *.o *.dSYM *.gch rbt_test 
