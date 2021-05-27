#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#define SIZE 10

typedef struct{
	int num;
	int array[SIZE];
} Struct;

int ex07_writer(){

	Struct *s;
	int fd, tamanho = sizeof(Struct);

	// Cria e verifica se a memória partilhada foi devidamente aberta 
	// permissões de read, write, execute/search para user, grupo e owner 
	if((fd = shm_open("/shm1", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");
		
	// Ajusta o tamanho da memória partilhada
	if(ftruncate(fd, tamanho)<0)
        perror("Failed to adjust memory size");

	// Mapea a memória partilhada 
	s = (Struct *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	// Verifica se a memória partilhada foi devidamente mapeada 
	srand ((unsigned) time (NULL));
	printf("SOU O WRITER\n");
  	for(int i = 0; i<SIZE; i++){
	  	s->array[i] = rand () % 20 + 1;
  		printf("INDEX %d com o valor %d\n", i, s->array[i]);
  	}

  	printf("\n");

	 if (s == NULL) {
        perror("No mmap()");
        exit(0);
    }
	return 0;
}

int ex07_reader(){
	Struct *s;
	int fd, tamanho = sizeof(Struct);
	//Cria e verifica se a memória partilhada foi devidamente aberta 
	//permissões de read, write, execute/search para user, grupo e owner 
	if((fd = shm_open("/shm1", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");

	//Ajusta o tamanho da memória partilhada
	if(ftruncate(fd, tamanho)<0)
        perror("Failed to adjust memory size");
        
	//Mapea a memória partilhada 
	s = (Struct *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	//Verifica se a memória partilhada foi devidamente mapeada 
    if (s == NULL) {
        perror("No mmap()");
        exit(0);
    }

    int sum = 0;
    printf("CHEGUEI AO READER\n");
	for(int i = 0; i<SIZE; i++){
		sum = sum + s->array[i];
  		printf("INDEX %d com o valor %d\n", i, s->array[i]);
  	}

  	printf("\nMédia %d\n", sum/SIZE);

	//Desfaz o mapeamento 
	if(munmap(s, tamanho)<0){ 
        perror("No munmap()");
        exit(0);
    }
    
    	//Fecha o descritor 
	if(close(fd)<0){ 
        perror("No close()");
        exit(0);
    }

	//Apaga a memoria partilhada do sistema 
    if(shm_unlink("/shm1")<0){ 
        perror("No unlink()");
        exit(1);
    }

    return 0;
}

int main(){
	ex07_writer();
	ex07_reader();
}
