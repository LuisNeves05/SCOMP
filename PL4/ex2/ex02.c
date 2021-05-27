#include "ex02.h"

int main(void)
{
    int i, j, num;
    char semName[100];
    FILE *inputFile, *outputFile;
    pid_t pid;
    sem_t *sem[NUMBER_PROCESS];

    /* Primeiro semaforo incializado a 1 */
    sprintf(semName, "%s%d", SEMAPHORE, 1);
    if ((sem[0] = sem_open(semName, O_CREAT, 0644, 1)) == SEM_FAILED)
    {
        perror("Error in sem_open()");
        exit(1);
    }
    /* Outros semaforos sao inicializados a 0 */
    for (i = 1; i < NUMBER_PROCESS; i++)
    {
        sprintf(semName, "%s%d", SEMAPHORE, i + 1);
        if ((sem[i] = sem_open(semName, O_CREAT, 0644, 0)) == SEM_FAILED)
        {
            perror("Error in sem_open()");
            exit(1);
        }
    }

    // Cria output
    outputFile = fopen(OUTPUT_FILE, "w");
    fclose(outputFile);

    // 8 Processos
    for (i = 0; i < NUMBER_PROCESS; i++)
    {
        pid = fork();
        if (pid == 0) /* Filho */
        {
            sem_wait(sem[i]);

            // Abre input
            inputFile = fopen(INPUT_FILE, "r");
            if (inputFile != NULL)
            {
                // Abre output
                outputFile = fopen(OUTPUT_FILE, "a");

                for (j = 0; j < i * NUMBER_NUMBERS; j++)
                {
                    fscanf(inputFile, "%d", &num);
                }

                for (; j < (i + 1) * NUMBER_NUMBERS; j++)
                {
                    fscanf(inputFile, "%d", &num);
                    fprintf(outputFile, "%d\n", num);
                }

                // Fecha output
                fclose(outputFile);

                // Fecha input
                fclose(inputFile);
            }
            // Up do semaforo seguinte
            if (i < NUMBER_PROCESS - 1)
            {
                sem_post(sem[i + 1]);
            }

            // Fecha semaforo
            sem_close(sem[i]);
            // Unlink
            sprintf(semName, "%s%d", SEMAPHORE, i + 1);
            sem_unlink(semName);

            exit(0);
        }
        else /* Pai */
        {
            wait(NULL);
        }
    }
    // Abre output
    outputFile = fopen(OUTPUT_FILE, "r");
    if (outputFile == NULL)
    {
        printf("Could not open file");
    }
    else
    {
        while (fscanf(outputFile, "%d", &num) != EOF)
        {
            printf("%d\n", num);
        }
    }
    fclose(outputFile);

    return EXIT_SUCCESS;
}
