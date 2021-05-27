#include<stdio.h>
#include<time.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdbool.h>
#define MAX_SIZE 3
#include "../sem.h"


int main(void){
   
    int id;
    sem_t *sem, *sem2;
    
    // Create semaphore to close and clean
    if ((sem = sem_open("ex8_sem", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
    {
        perror("Error in sem_open()");
        exit(1);
    }

    if ((sem2 = sem_open("ex8_sem_2", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    {
        perror("Error in sem_open()");
        exit(1);
    }

     
     
    for(int i=0;i<3;i++){
        id = fork (); 
        if (id > 0){ //Pai
            sem_wait(sem);
            printf("S");
            sem_post(sem2);
        }else{ //Filho
            sem_wait(sem2);
            printf("C");
            sem_post(sem);
            exit(0);
        }
    }

    closeAndDelete(sem, "ex8_sem");
    closeAndDelete(sem2, "ex8_sem_2");

    return 0;
}