C_SRC=$(shell find -type f | grep 'part' | grep '\.c')
PROGS=$(subst .c,,$(C_SRC))



build: $(PROGS) 



% : %.c 
	gcc -Wall -Wpedantic -Wextra $< -o $@ 

