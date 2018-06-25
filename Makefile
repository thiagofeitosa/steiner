############################# Makefile ##########################
main: main.cpp steiner.h vetorja.h #load.h
	g++ -o main main.cpp -lglpk -lm -W -Wall
clean:
	rm -rf *.o
mrproper: clean
	rm -rf main
