C_SRC=$(shell find -type f | grep 'part' | grep '\.c')
PROGS=$(subst .c,,$(C_SRC))



build: $(PROGS) 



% : %.c data_structs.h
	gcc -lm -ggdb -Wall -Wpedantic -Wextra $< -o $@ 

