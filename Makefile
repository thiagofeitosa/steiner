############################# Makefile ##########################
main: main.c steiner.h
	gcc -o main main.c -lglpk -lm -W -Wall
clean:
	rm -rf *.o
mrproper: clean
	rm -rf main
