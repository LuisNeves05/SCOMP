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

#define ARRAY_SIZE 10

struct st
{
    int numbers[ARRAY_SIZE];
    int sem;
};

int main(){

    int i, j, fd, tamanho = sizeof(struct st);
    pid_t pid;
    struct st *a;

    fd = shm_open("/shm1191421", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(fd, tamanho);
    a = (struct st *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    a->sem = 1;

    pid = fork();

    if (pid == 0) /* Producer filho*/
    {
        for (i = 0; i < 3; i++)
        {

            while (!a->sem)
                ;

            for (j = 0; j < ARRAY_SIZE; j++)
            {
                a->numbers[j] = (i * 10) + j + 1;
            }

            a->sem = 0;
        }
    }
    else /* Consumer pai */
    {
        for (i = 0; i < 3; i++)
        {

            while (a->sem)
                ;

            for (j = 0; j < ARRAY_SIZE; j++)
            {
                printf("Consumer says %d\n", a->numbers[j]);
            }

            a->sem = 1;
        }

        if (shm_unlink("/shm1191421") < 0)
        {
            printf("Error at shm_unlink()!\n");
            exit(EXIT_FAILURE);
        }
        // Disconnect map
        if (munmap(a, tamanho) < 0)
        {
            printf("Error  munmap()!\n");
            exit(EXIT_FAILURE);
        }
        // Close fd
        if (close(fd) < 0)
        {
            printf("Error  close()!\n");
            exit(EXIT_FAILURE);
        }

    }

    wait(NULL);

    return 0;
}
