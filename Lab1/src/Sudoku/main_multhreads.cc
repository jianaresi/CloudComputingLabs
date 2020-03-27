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

#include "sudokumul.h"
char (*fileContent)[N];
int (*result)[N];
int jobNum=0;
int nextJobToBeDone=0;

pthread_mutex_t jobQueueMutex=PTHREAD_MUTEX_INITIALIZER;

int64_t now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}
typedef struct {
  int myboard[N];
} ThreadParas;
void input(const char in[N],int *board){
  for (int cell = 0; cell < N; ++cell) {
    board[cell] = in[cell] - '0';
    assert(0 <= board[cell] && board[cell] <= NUM);
  }
}
void readFile(char *path){
	char puzzle[128];
	FILE *fp=fopen(path,"r");
	while (fgets(puzzle, sizeof puzzle, fp) != NULL) {
		jobNum++;
	}
	if(DEBUG_MODE)printf("%d\n",jobNum);
	fileContent=(char (*)[N])malloc(jobNum*N*sizeof(int));
	result=(int (*)[N])malloc(jobNum*N*sizeof(int));
	rewind(fp); 
	int j=0;
	while (fgets(puzzle, sizeof puzzle, fp) != NULL) {
		strcpy(fileContent[j],puzzle);
		j++;
	}
	fclose(fp);
}
int recvAJob(){
	int currentJobID=0;
  pthread_mutex_lock(&jobQueueMutex);
  if(nextJobToBeDone>=jobNum) 
  {
    pthread_mutex_unlock(&jobQueueMutex);
    return -1;
  }
  currentJobID=nextJobToBeDone;
  nextJobToBeDone++;
  pthread_mutex_unlock(&jobQueueMutex);
  return currentJobID;
}
void* sudokuSlove(void *args){
	ThreadParas* para = (ThreadParas*) args; 
	int currentJobID=0;
	while(1){
		currentJobID=recvAJob();  
		if(currentJobID==-1)//All job done!
      break;    
  	if(DEBUG_MODE)printf("thread:[%ld],job:[%d]", pthread_self(),currentJobID);
  	input(fileContent[currentJobID],para->myboard);
  	solve_sudoku_dancing_links(para->myboard,0);
  	//if(DEBUG_MODE)printf("The result:");
  	for(int i=0;i<N;i++){
  		result[currentJobID][i]=para->myboard[i];
  		//if(DEBUG_MODE)printf("%d",result[currentJobID][i]);
  	}
  	if(DEBUG_MODE)printf("\n");
  }
}

int main(int argc, char* argv[])
{
	int threadsNum=1;
	if(argc==3){
		readFile(argv[1]);
		threadsNum=atoi(argv[2]);
	}
	else{
		printf("Please enter the correct number of parameters\n");
		exit(1);
	}
	pthread_t th[threadsNum];
	ThreadParas thPara[threadsNum];
	for(int i=0;i<threadsNum;i++)
  {
    if(pthread_create(&th[i], NULL, sudokuSlove,&thPara[i])!=0)
    {
      perror("pthread_create failed");
      exit(1);
    }
  }
	for(int i=0;i<threadsNum;i++)
    pthread_join(th[i], NULL);
 	
  free(fileContent);
  free(result);

  return 0;
}

