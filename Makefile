
.PHONY: all clean debug test test.valgrind

all:
	g++ -std=c++11 -Isrc src/maxmm/rope/rope.cpp tests/main.cpp -o rope.tsk

debug:
	g++ -std=c++11 -Isrc -DMAXMM_ROPE_DEBUG -g src/maxmm/rope/rope.cpp tests/main.cpp -o rope.tsk

test: debug
	./rope.tsk

test.valgrind: debug
	valgrind --leak-check=full ./rope.tsk

clean:
	rm -f *.o
	rm -f *.tsk
