

.PHONY: all clean debug test 

all:
	g++ -std=c++11 rope.cpp main.cpp -o rope.tsk

debug:
	g++ -std=c++11 -DMRANSAN_ROPE_DEBUG -g rope.cpp main.cpp -o rope.tsk

test: debug
	./rope.tsk

clean:
	rm -f *.o
	rm -f *.tsk
