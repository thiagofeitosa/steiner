############################# Makefile ##########################
abcbee: abcbee.cpp steiner.h vetorja.h 
	g++ -o abcbee abcbee.cpp -lglpk -lm -W -Wall -L.
clean:
	rm -rf *.o
mrproper: clean
	rm -rf abcbee
