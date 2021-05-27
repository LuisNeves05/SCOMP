#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#define SIZE 50
#define ARRAYSIZE 100000

struct St{
	char str[SIZE];
	int num;
};

struct St sts[ARRAYSIZE];

int ex03_writer(){
	struct St *st;

	clock_t start, end;

	start = clock();
	
	int i;
	for(i = 0; i < 100; i++){
		strcpy(sts[i].str, "ISEP - SCOMP 2020");
		sts[i].num = 10;
	}

	end = clock();

	double time = (double)(end - start) / CLOCKS_PER_SEC;

	printf("\n");
	printf("\n%lf\n", time);
	
	int fd, tamanho = sizeof(struct St);

	// Cria e verifica se a memória partilhada foi devidamente aberta 
	// permissões de read, write, execute/search para user, grupo e owner 
	if((fd = shm_open("/shm", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");
		
	// Ajusta o tamanho da memória partilhada
	if(ftruncate(fd, tamanho)<0)
        perror("Failed to adjust memory size");

	// Mapea a memória partilhada 
	st = (struct St *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	// Verifica se a memória partilhada foi devidamente mapeada 
	
	 if (st == NULL) {
        perror("No mmap()");
        exit(0);
    }


	//Desfaz o mapeamento 
	if(munmap(st, tamanho)<0){ 
        perror("No munmap()");
        exit(0);
    }

	//Fecha o descritor 
	if(close(fd)<0){ 
        perror("No close()");
        exit(0);
    }

	return 0;
}


int ex03_reader(){
	struct St *st;
	
	int fd, tamanho = sizeof(struct St);

	// Cria e verifica se a memória partilhada foi devidamente aberta 
	// permissões de read, write, execute/search para user, grupo e owner 
	if((fd = shm_open("/shm", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");
		
	// Ajusta o tamanho da memória partilhada
	if(ftruncate(fd, tamanho)<0)
        perror("Failed to adjust memory size");

	// Mapea a memória partilhada 
	st = (struct St *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	// Verifica se a memória partilhada foi devidamente mapeada 
	
	 if (st == NULL) {
        perror("No mmap()");
        exit(0);
    }
    
	int i;
	for(i = 0; i < 100; i++){
		printf("%s", sts[i].str);
		printf("%d", sts[i].num);
	}
	printf("\n");
 

	//Desfaz o mapeamento 
	if(munmap(st, tamanho)<0){ 
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

	return 0;
}

int main(){
	ex03_writer();
	ex03_reader();
	return 0;
}
