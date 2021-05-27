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
	int num1;
	int num2;
} Struct;

int ex05_writer(){
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
	
	 if (s == NULL) {
        perror("No mmap()");
        exit(0);
    }

	pid_t pid;
  	pid = fork();

  	s->num1 = 8000;
  	s->num2 = 200;

    //WriterBody
    if(pid != 0){ //PAI
    
    	for(int i = 0; i < 1000000; i++){
    		s->num1 = s->num1 + 1;
    		s->num2 = s->num2 - 1;
    		//printf("%d\n", i);
    	}
	}else{ //FILHO
		for(int i = 0; i < 1000000; i++){
			s->num1 = s->num1 - 1;
    		s->num2 = s->num2 + 1;
    		//printf("%d\n", i);
    	}
    	exit(1);
	}

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

	return 0;
}


int ex05_reader(){
	Struct *s;
	int fd, tamanho=sizeof(Struct);

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
    
    //Read struct
	printf("\nNUM1 = %d", s->num1);
	printf("\nNUM2 = %d", s->num2);
	printf("\n");

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
	ex05_writer();
	ex05_reader();
}
