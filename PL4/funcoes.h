#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>


#define SEMAPHORE "/1191421_sem"

int babyMaker(int n){
    int p, i;
    for(i=0; i<n; i++){
        p = fork();
        if(p<0) exit(-1); //ERRO
        if(p==0) return i+1; //FILHO i+1
    }
    return 0; //PAI
}

void babyFuneral(int n)
{
    int i, status, exitcode, p;
    for (i = 0; i < n; i++)
    {
        p = waitpid(-1, &status, 0);
        if (WIFEXITED(status))
        {
            exitcode = WEXITSTATUS(status);
            //printf("Baby %d has gone with exit code %d\n", p, exitcode);
        }
    }
}

void * createSem(char semName[], int value){
    sem_t *sem;
    if ((sem = sem_open(semName, O_CREAT | O_EXCL, 0644, value)) == SEM_FAILED) {
        perror("Error at sem_open().\n");
        exit(0);
    }
    return sem;
}