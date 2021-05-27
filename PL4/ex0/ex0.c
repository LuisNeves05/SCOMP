/*  Exemplo de semaforo binario.
 *
 *  Utilizacao de semaforo binario para controlar acessos a memoria partilhada
 */
 
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define NBABIES 1

int babyMaker(int n){
	int p, i;
	for(i=0; i<n; i++){
		p = fork();
		if(p<0) exit(-1); //ERRO
		if(p==0) return i+1; //FILHO i+1
	}
	return 0; //PAI
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

int main(int argc, char *argv[]) {
    int *shm=NULL;;
	int fd, i, id;
        
    //Cria e verifica se a memória partilhada foi devidamente aberta 
	//permissões de read, write, execute/search para owner, grupo e other 
	if((fd = shm_open("/shmHSP", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");

	//Ajusta o tamanho da memória partilhada
	if(ftruncate(fd, sizeof(int))<0)
        perror("Failed to adjust memory size");

	//Mapea a memória partilhada 
	shm = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	//Verifica se a memória partilhada foi devidamente mapeada 
    if (shm == NULL) {
        perror("No mmap()");
        exit(0);
    }
    
    sem_t *sem;
	//Cria/abre o semáforo
	//permissões 0644 (owner: read and write; group: read; other: read) 
	if ((sem = sem_open("semaforo", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED) {
		perror("Error at sem_open().\n");
		exit(0);
	}
    
    *shm = 0;
                       
    id = babyMaker(NBABIES);
	
	if(id == 0){ //PAI
        down(sem);
        for (i=0; i<500000; i++) (*shm)++;
        up(sem);

        wait(NULL); /* esperar que o filho termine */
        printf("COUNT: %d.\n", *shm);
    }
    else{
        down(sem);
        for (i=0; i<500000; i++) (*shm)++;
        up(sem);

        exit(0);
    }

    //Desfaz o mapeamento 
	if(munmap(shm, sizeof(int))<0){ 
        perror("No munmap()");
        exit(0);
    }

	//Fecha o descritor 
	if(close(fd)<0){ 
        perror("No close()");
        exit(0);
    }

	//Apaga a memoria partilhada do sistema 
    if(shm_unlink("/shmHSP")<0){ 
        perror("No unlink()");
        exit(1);
    }
    
    //Fecha o semáforo
	if (sem_close(sem) == -1) {
		perror("Error at sem_close().\n");
		exit(0);
	}

	//Apaga o semáforo
	if (sem_unlink("semaforo") == -1) {
		perror("Error at sem_unlink().\n");
		printf("Error: %s\n", strerror(errno));
		exit(0);
	}
    
    return 0;
}