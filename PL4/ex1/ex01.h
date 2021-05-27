#ifndef PL4_H
#define PL4_H

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

// Number of processes
#define NUMBER_PROCESS 8
// Number of numbers to be used in the files
#define NUMEROS 200
// Name of file to read
#define INPUT_FILE "Numbers.txt"
// Name of file to write
#define OUTPUT_FILE "Output.txt"
// Name of semaphore
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

#endif
