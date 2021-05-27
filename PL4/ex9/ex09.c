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
#include "../funcoes.h"
#define N_SEMAFOROS 2
#define SEMAFORO_01 0
#define SEMAFORO_02 1


void buy_chips(){
	printf("buy_chips!\n");
}
void buy_beer(){
	printf("buy beer!\n");
}
void eat_and_drink(){
	printf("eat and drink!\n");
}

int main(void) {


	sem_t *sem, *sem2;

	
		if ((sem = sem_open("ex09", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED) {
			perror("Error at sem_open()!\n");
			exit(EXIT_FAILURE);
		}

		if ((sem2 = sem_open("ex09_2", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {
			perror("Error at sem_open()!\n");
			exit(EXIT_FAILURE);
		}

	int id = babyMaker(2);

	if(id == 1){
		sem_wait(sem);			//sem ta a 1 e da down, fica a 0
		buy_chips();		
		sem_post(sem2);			//sem2 fica a 0 e desbloqueia no outro filho
		sem_wait(sem);			//sem fica bloqueado a espera que o outro filho o desbloqueie
		eat_and_drink();
		exit(1);
	}

	if(id == 2){
		sem_wait(sem2);			//espera que o primeiro filho o desbloqueie
		buy_beer();
		sem_post(sem);			//desbloqueia o primeiro filho
		eat_and_drink();
		exit(1);
	}

	
	babyFuneral(2);


	sem_close(sem);
	sem_close(sem2);
	sem_unlink("ex09");
	sem_unlink("ex09_2");

	return EXIT_SUCCESS;
}
