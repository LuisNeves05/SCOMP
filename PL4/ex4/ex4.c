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

#define NBABIES 50

int babyMaker(int n){
	int p, i;
	for(i=0; i<n; i++){
		p = fork();
		if(p<0) exit(-1); //ERRO
		if(p==0) return i+1; //FILHO i+1
	}
	return 0; //PAI
}

void babyFuneral(int n){
	int i, status, exitcode, p;
	for(i=0; i<n; i++){
		p = waitpid(-1,&status,0);
		if(WIFEXITED(status)){
			exitcode = WEXITSTATUS(status);
		}
	}
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

typedef struct{
    char str[50][80];
} Desc;

int main(void){

    sem_t *sem;

    /* Primeiro semaforo incializado a 1 */
    if ((sem = sem_open("SEMAFORO 3", O_CREAT, 0644, 1)) == SEM_FAILED){
        perror("Error in sem_open()");
        exit(1);
    }


	Desc *d;
	int fd, tamanho = sizeof(Desc);

	// Cria e verifica se a memória partilhada foi devidamente aberta 
	// permissões de read, write, execute/search para user, grupo e owner 
	if((fd = shm_open("/shm", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");
		
	// Ajusta o tamanho da memória partilhada
	if(ftruncate(fd, tamanho)<0)
        perror("Failed to adjust memory size");

	// Mapea a memória partilhada 
	d = (Desc *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	// Verifica se a memória partilhada foi devidamente mapeada 
	 if (d == NULL) {
        perror("No mmap()");
        exit(0);
    }
    
    int id;

    id = babyMaker(NBABIES);


    if(id == 0){
    	babyFuneral(NBABIES);
    }
    else{
    	down(sem);

    	char aux[80];

    	sprintf(aux, "I’m the Father - with PID %d\n", getpid());

    	strcpy(d->str[id], aux);

    	up(sem);

    	exit(0);
    }


    for(int i = 0; i < NBABIES; i++){
    	printf("%s\n", d->str[i]);
    }

    int op;
    int index = 50;

        do{

        printf("\n1 - Remover ultima string da shm\n0 - sair\n");
        scanf("%d", &op);
        switch (op)
        {
        case 1:
            printf("Remover ultima string...\n");

            // Sleep 12 segundos
            sleep(12);
            // remover str da ultima posição
            strcpy(d->str[index], "" /* empty string */);
            // diminuir index
            index--;
            // print das strings na shm
            for (int i = 0; i < index; i++){
                printf("%s\n", d->str[i]);
            }

            break;

        case 0: /* sair */
            printf("sair\n");
            break;

        default:
            printf("Selecione uma opcao valida\n");
        }

    } while (op != 0);


    //Desfaz o mapeamento 
    if(munmap(d, tamanho)<0){ 
        perror("No munmap()");
        exit(0);
    }
    
        //Fecha o descritor 
    if(close(fd)<0){ 
        perror("No close()");
        exit(0);
    }

    //Apaga a memoria partilhada do sistema 
    if(shm_unlink("/shm")<0){ 
        perror("No unlink()");
        exit(1);
    }

    //Fecha os semáforos
        if (sem_close(sem) == -1) {
            perror("Error at sem_close().\n");
            exit(0);
        }

    //Apaga os semáforos
        if (sem_unlink("SEMAFORO 3") == -1) {
            perror("Error at sem_unlink().\n");
            printf("Error: %s\n", strerror(errno));
            exit(0);
        }

    return EXIT_SUCCESS;
}