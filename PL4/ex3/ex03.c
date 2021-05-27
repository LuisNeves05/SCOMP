#include "ex03.h"
#include "../funcoes.h"

int main(void)
{

    int i = 0, j, num;
    int id;
    sem_t *semFilho, *semPai;

    // Create semaphore to close and clean
    if ((semFilho = sem_open(SEMAPHORE, O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
    {
        perror("Error in sem_open()");
        exit(1);
    }

    if ((semPai = sem_open("semPai", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    {
        perror("Error in sem_open()");
        exit(1);
    }

    id = babyMaker(1);

    if(id > 0){     //filho

        sem_wait(semFilho);          //Este semaforo, como esta inicializado a 1, vai correr primeiro
        printf("Sou o filho!\n");

        sem_post(semPai);

        exit(0);

    }else{

        sem_wait(semPai);           //Este semaforo vai estar sempre a espera que o filho de Up para poder avançar
        printf("Sou o pai!\n");

        sem_post(semFilho);
    }

    sem_close(semPai);
    sem_close(semFilho);
    sem_unlink(SEMAPHORE);
    sem_unlink("semPai");

    return 0;
}