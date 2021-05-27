#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>

void printMessage(char *message) {
    printf("%s", message);
    fflush(stdout);
}


void down(sem_t *sem) {
    if (sem_wait(sem) == -1) {
        perror("Error at sem_wait().");
        exit(0);
    }
}

void up(sem_t *sem) {
    if (sem_post(sem) == -1) {
        perror("Error at sem_post().");
        exit(0);
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

void closeSem(sem_t *sem){
    if (sem_close(sem) == -1) {
		perror("Error at sem_close().\n");
		exit(0);
	}
}


void deleteSem(char semName[]){
    if (sem_unlink(semName) == -1) {
		perror("Error at sem_unlink().\n");
		printf("Error: %s\n", strerror(errno));
		exit(0);
	}
}

void closeAndDelete(sem_t *sem, char semName[]){
	closeSem(sem);
	deleteSem(semName);
}