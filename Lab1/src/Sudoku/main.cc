#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "sudoku.h"

int64_t now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main(int argc, char* argv[])
{
  char fileName[128];
  while(scanf("%s",fileName)!=EOF){
  	init_neighbors();
  	FILE* fp = fopen(fileName, "r");
  	char puzzle[128];
  	int total_solved = 0;
  	int total = 0;
  	bool (*solve)(int) = solve = solve_sudoku_dancing_links;
  	int64_t start;
  	if(DEBUG_MODE1) start= now();
  	while (fgets(puzzle, sizeof puzzle, fp) != NULL) {
    	if (strlen(puzzle) >= N) {
      	++total;
      	input(puzzle);
      	init_cache();
      	if (solve(0)) {
        	++total_solved;
        	if (!solved())
          	assert(0);
          	for(int i=0;i<81;i++){
          		fprintf(stdout,"%d",board[i]);
          	}
    		}
      	fprintf(stdout,"\n");   
   		}
   		else {
      	printf("No: %s", puzzle);
   		}
 		}
 		if(DEBUG_MODE1) {
 			int64_t end=now();
 			double sec = (end-start)/1000000.0;
  		fprintf(stderr,"use %f sec\n",sec);
  	}
  	//int i=fflush(NULL);
  }
  return 0;
}


