#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>

#include "sudoku.h"

pthread_mutex_t jobQueueMutex=PTHREAD_MUTEX_INITIALIZER;
char *fileText=NULL;
long currentPos=0;

int64_t now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}
typedef struct {
  char chrBuff[128];
} ThreadParas;
char* textFileRead(char* filename)
{
	char* text;
	FILE *pf = fopen(filename,"r");
	fseek(pf,0,SEEK_END);
	long lSize = ftell(pf);
	// 用完后需要将内存free掉
	text=(char*)malloc(lSize+1);
	rewind(pf); 
	int i=fread(text,sizeof(char),lSize,pf);
	text[lSize] = '\0';
	return text;
}
int recvAJob(ThreadParas *my){
	char chrBuff[128];
	pthread_mutex_lock(&jobQueueMutex);
	int i=currentPos,n=0;
	while(1){
		if(fileText[i]=='\0'){
			chrBuff[n]='\0';
			pthread_mutex_unlock(&jobQueueMutex);
			return -1;
		}
		if(fileText[i]=='\n'){
			chrBuff[n]='\0';
			if(strlen(chrBuff)==0) printf("!!!!!!!!");
			break;
		}
		chrBuff[n]=fileText[i];
		n++;i++;
		
	}
	currentPos+=n+1;
	strcpy(my->chrBuff,chrBuff);
  pthread_mutex_unlock(&jobQueueMutex);
  return 0;
}
void processAJob(ThreadParas *my){
	bool (*solve)(int) = solve_sudoku_basic;
  solve=solve_sudoku_dancing_links;
  if (strlen(my->chrBuff) >= N) {
     input(my->chrBuff);
     init_cache();
     if (solve(0)) {
     	 printf("Right: %s\n",my->chrBuff);
     if (!solved())
       assert(0);
     }
     else {
       printf("Wrong: %s\n", my->chrBuff);
     }
  }
}	
void* sudokuSlove(void *args){
	while(1){
		ThreadParas my;
		int nflag=recvAJob(&my);
		if(nflag==-1) break;
		processAJob(&my);
	}
}

int main(int argc, char* argv[])
{
	int threadsNum=1;
	if(argc==3){
		fileText=textFileRead(argv[1]);
		threadsNum=atoi(argv[2]);
	}
	else{
		printf("Please enter the correct number of parameters\n");
		exit(1);
	}
	pthread_t th[threadsNum];
	for(int i=0;i<threadsNum;i++)
  {
    if(pthread_create(&th[i], NULL, sudokuSlove, NULL)!=0)
    {
      perror("pthread_create failed");
      exit(1);
    }
  }
	for(int i=0;i<threadsNum;i++)
    pthread_join(th[i], NULL);
  free(fileText);
	/*char fileName[128];//storage one file's name 
	while(scanf("%s",fileName)!=EOF){
  	init_neighbors();
		FILE* fp =fopen(fileName,"r");
  	char puzzle[128];
  	int total_solved = 0;
  	int total = 0;
  	bool (*solve)(int) = solve_sudoku_basic;
  	solve=solve_sudoku_dancing_links;
  	int64_t start = now();
  	while (fgets(puzzle, sizeof puzzle, fp) != NULL) {
    	if (strlen(puzzle) >= N) {
      	++total;
      	input(puzzle);
      	init_cache();
      	if (solve(0)) {
        	++total_solved;
        	if (!solved())
          	assert(0);
      	}
      	else {
        	printf("No: %s", puzzle);
      	}
    	}
  	}
  	int64_t end = now();
  	double sec = (end-start)/1000000.0;
  	printf("%f sec %f ms each %d\n", sec, 1000*sec/total, total_solved);
	}
	*/
  return 0;
}

