############################# Makefile ##########################
main: main.c steiner.h vetorja.h
	g++ -o main main.c -lglpk -lm -W -Wall
clean:
	rm -rf *.o
mrproper: clean
	rm -rf main
