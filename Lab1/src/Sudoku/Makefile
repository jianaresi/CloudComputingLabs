CXXFLAGS+=-O2 -ggdb -DDEBUG
CXXFLAGS+=-Wall -Wextra

all: sudoku sudoku_slove

sudoku: main.cc neighbor.cc sudoku_basic.cc sudoku_min_arity.cc sudoku_min_arity_cache.cc sudoku_dancing_links.cc
	g++ -O2 -o $@ $^
	
sudoku_slove: main_multhreads.cc  sudokumul_dancing_links.cc
	g++ -O2 -pthread -o $@ $^
clean:	
	-rm sudoku sudoku_slove 
