#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

#define size_array 1000000


typedef struct {
    int numero;
} incdec;

int main(){

    int valor = 100;
    int numOps = 1000000;
    incdec *a;
    int fd, tamanho=sizeof(incdec);

    if((fd = shm_open("/shm1191421", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
        perror("Failed to create shared memory");

    if(ftruncate(fd, tamanho)<0)
        perror("Failed to adjust memory size");

    a =(incdec *)mmap(NULL, tamanho,PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    a->numero = valor;

    pid_t pid = fork();


    for (int i = 0; i < numOps; i++) {
        a->numero += 1;
        a->numero -= 1;
    }

    if (pid > 0) {
        printf("Pai : %d\n", a->numero);

        wait(NULL);
    }else{
        printf("Filho : %d\n", a->numero);
        exit(1);
    }

    if(munmap(a, tamanho)<0){ 
        perror("No munmap()");
        exit(0);
    }

    //Fecha o descritor 
    if(close(fd)<0){ 
        perror("No close()");
        exit(0);
    }

    //Apaga a memoria partilhada do sistema 
    if(shm_unlink("/shm1191421")<0){ 
        perror("No unlink()");
        exit(1);
    }
    

    return 0;
}
