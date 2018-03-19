#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define SHMKEY 75

int find(int start, int end, int length, int search, int *num,int *ptr){
	int i, k;
	//printf("%d %d %d %d %d\n",getpid(),start, end, ptr[0], ptr[1]);
	if(ptr[1] == -1)
		for(i = start; i <= end && i < length; i++){
			(ptr[0])++;
			//printf("%d\n",num[i]);
			if(num[i] == search){
				ptr[1] = i;
				return i;
			}
		}
	else
		exit(1);
}
int main(int argc,char *argv[]){
	FILE *fp;
	int parentPid = getpid(), pid, d, n, start = 0, end=4, i, length = 1,count=0,*shm;
	//printf("Parent = %d \n",parentPid);
	int shmid = shmget(SHMKEY, 2*sizeof(int), 0666 | IPC_CREAT);
	shm = shmat(shmid, NULL, 0);
	shm[0]=0;
	shm[1]=-1;
	char c;
	fp = fopen(argv[1], "r");
	int *num; 
	for (c = getc(fp); c != EOF; c = getc(fp))
        	if (c == '\n') 
            		length++;
	fseek(fp,0,0);
	d = length;
	//printf("length = %d\n",length);
	int search = atoi(argv[2]);
	num = (int*) malloc(d * sizeof(int));
	for(i = 0; i < d; i++){
		fscanf(fp, "%d", &n);
		num[i] = n;
	}
	fclose(fp);
	i=0;
	while(length > 0){
		if((pid = fork()) == 0){	
			break;
		}
		else{
			start += 5;
			end += 5;
			length -= 5;
		}
	}
	
	pid = getpid();
	if(pid != parentPid)
		find(start, end, d, search, num, shm);
	if(pid == parentPid){
		
		while(shm[1] == -1 && shm[0] < d){
			//printf("WAITING\n");
		}
		
		if(shm[1] != -1){
			printf("Number found at %d\n",shm[1]);
			kill(-1*parentPid, SIGKILL);
		}
		
		if(shm[0] == d){
			printf("Number not found\n");
			exit(1);
		}
	}
}
