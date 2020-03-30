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
#include<unistd.h>
#include "sudokumul.h"
//Store the contents of the file
char (*fileContent)[N];
//Store the corresponding results
int (*result)[N];
//Total number of tasks
int jobNum=0;
int nextJobToBeDone=0;

pthread_mutex_t jobQueueMutex=PTHREAD_MUTEX_INITIALIZER;

int64_t now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}
//Stores the results of each thread
typedef struct {
  int myboard[N];
} ThreadParas;
//Converts the Input Char to an int
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
//	if(DEBUG_MODE)printf("%d\n",jobNum);
	fileContent=(char (*)[N])malloc(jobNum*N*sizeof(int));
	result=(int (*)[N])malloc(jobNum*N*sizeof(int));
	rewind(fp); //The file pointer redirects to the file header
	int j=0;
	while (fgets(puzzle, sizeof puzzle, fp) != NULL) {
		strcpy(fileContent[j],puzzle);
		j++;
	}
	fclose(fp);
}
//Get the task number that the thread needs to do
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
//the sudoku_slove function
void* sudokuSlove(void *args){
	ThreadParas* para = (ThreadParas*) args; 
	int currentJobID=0;
	while(1){
		currentJobID=recvAJob();  
		if(currentJobID==-1)//All job done!
      break;    
//  	if(DEBUG_MODE)printf("thread:[%ld],job:[%d]", pthread_self(),currentJobID);
  	input(fileContent[currentJobID],para->myboard);
  	solve_sudoku_dancing_links(para->myboard,0);
  	for(int i=0;i<N;i++){
  		result[currentJobID][i]=para->myboard[i];//Store the results in the appropriate location
  	}
  }
}

int main(int argc, char* argv[])
{
	int threadsNum=1;
  char fileName[128];
  
  threadsNum=sysconf(_SC_NPROCESSORS_ONLN);
	pthread_t th[threadsNum];
	ThreadParas thPara[threadsNum];
	int64_t start;
	while(scanf("%s",fileName)!=EOF){
		if(DEBUG_MODE) start = now();
		jobNum=0;
		nextJobToBeDone=0;
  	readFile(fileName);
		for(int i=0;i<threadsNum;i++)
  	{
    	if(pthread_create(&th[i], NULL, sudokuSlove,&thPara[i])!=0)
    	{
      	perror("pthread_create failed");
      	exit(1);
    	}
  	}
  	//wait all threads end
		for(int i=0;i<threadsNum;i++)
    	pthread_join(th[i], NULL);
 		//the output
		for(int i=0;i<jobNum;i++)
		{
			for(int j=0;j<N;j++){
				fprintf(stdout,"%d",result[i][j]);
			}
  		fprintf(stdout,"\n");
		fflush(stdout);
		}
		// Free Memory	
  	free(fileContent);
  	free(result);
  	if(DEBUG_MODE) {
 			int64_t end=now();
 			double sec = (end-start)/1000000.0;
  		fprintf(stderr,"use %f sec\n",sec);
//		fflush(stdout);
  	}
  }
  return 0;
}


