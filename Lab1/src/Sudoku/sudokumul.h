#ifndef SUDOKUMUL_H
#define SUDOKUMUL_H

const bool DEBUG_MODE = true;
enum { ROW=9, COL=9, N = 81, NEIGHBOR = 20 };
const int NUM = 9;


void input(const char in[N],int *board);
bool solve_sudoku_dancing_links(int* board,int unused);
#endif
