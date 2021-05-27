#include "../funcoes.h"


int main(void) {
	sem_t *sem2;
	
	if ((sem2 = sem_open("/sem_ex5", O_CREAT | O_EXCL, 0644, 0))
			== SEM_FAILED) {
		perror("Error in sem_open()");
		exit(1);
	}

	pid_t pid = fork();

	if (pid ==0){
		printf("Eu sou o filho\n");
		sem_post(sem2);
	}
	if(pid > 0){
		sem_wait(sem2);
		printf("Eu sou o pai\n");
	}

	sem_close(sem2);
	sem_unlink("/sem_ex5");

	return EXIT_SUCCESS;
}