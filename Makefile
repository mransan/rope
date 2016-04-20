

.PHONY: all clean

all:
	g++ -std=c++11 rope.cpp -o rope.tsk

debug:
	g++ -std=c++11 -g rope.cpp -o rope.tsk

clean:
	rm -f *.o
	rm -f *.tsk
