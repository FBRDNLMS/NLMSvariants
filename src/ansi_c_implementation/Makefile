#build lms binary
all: NLMSvariants.c
	gcc -o lms NLMSvariants.c -lm

win: NLMSvariants.c
	gcc -o lms NLMSvariants.c

gdb: NLMSvariants.c
	gcc -Wall -Wextra -pedantic -ggdb NLMSvariants.c

clean:
	$(RM) lms
