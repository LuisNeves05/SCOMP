#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <limits.h>

#define ARRAY_SIZE 1000

int babyMaker(int n)
{
    int p, i;
    for (i = 0; i < n; i++)
    {
        p = fork();
        if (p < 0)
            exit(-1); //ERRO
        if (p == 0)
            return i + 1; //FILHO i+1
    }
    return 0; //PAI
}

void babyFuneral(int n)
{
    int i, status, exitcode, p;
    for (i = 0; i < n; i++)
    {
        p = waitpid(-1, &status, 0);
        if (WIFEXITED(status))
        {
            exitcode = WEXITSTATUS(status);
            printf("Baby %d has gone with exit code %d\n", p, exitcode);
        }
    }
}

void pl1_ex01()
{
    int x = 1;
    pid_t p = fork(); /*pid_t: sys/types.h; fork(): unistd.h*/
    if (p == 0)
    { // Filho
        x = x + 1;
        printf("1. x = %d\n", x);
    }
    else
    { // Pai
        x = x - 1;
        printf("2. x = %d\n", x);
    }
    printf("3. %d; x = %d\n", p, x);

    // A - Depending of the scheduling chosing the child or the father to go first, if it is the father
    // 2 will be printed, and next the 3 with the PID of 1234, then the prints of the child.

    // B - The father will output first, but it depends of the scheduling processes in the CPU.
}

void pl1_ex02()
{
    printf("I'm..\n");
    if (fork() != 0)
    {                        // Pai
        wait(NULL);          // Pai espera que o 1º filho termine
        printf("..the..\n"); // O pai esperou que o 1º filho terminasse para escrever
        if (fork() != 0)
        {                        // Pai
            wait(NULL);          // Pai espera que o 2º filho termine
            printf("father!\n"); // O pai esperou que o 2º filho terminasse para escrever
        }
        else
        { // 2º Filho criado
            printf("I'll never join you!\n");
            exit(0); // 2º Filho termina
        }
    }
    else
    { // 1º Filho criado
        printf("I'll never join you!\n");
        exit(0); // 1º Filho termina
    }
    printf("I'll never join you!\n"); // Com os dois filhos terminados, apenas o pai escreve
}

void pl1_ex03()
{
    fork();
    fork();
    fork();
    printf("SCOMP!\n");

    // A - 8 process are created, because when using fork() without exit(), we can use the following
    // rule, 2 ^ N, meaning 2 ^ 3 = 8

    // B
    // |---+
    // |---|---+
    // |---|---|---+
    // |---|---|---|

    // C - since 8 process are create, 8 prints will be in the output
}

void pl1_ex04()
{
    int a = 0, b, c, d;
    b = (int)fork();
    c = (int)getpid(); /* getpid(), getppid(): unistd.h*/
    d = (int)getppid();
    a = a + 5;
    //if(b>0) wait(NULL);
    printf("\na=%d, b=%d, c=%d, d=%d\n", a, b, c, d);

    // A
    // Only the variable A will be the same, because for b, the fork will be different if it is a child or a parent
    // the c, will represent each individual PID that can't be shared by process, and for C, the child will have the
    // parent PID, and the parent will be someones child

    // B
    // |---+
    // |---|
    // |---|
}

void pl1_ex05()
{
    int pidVec[3] = {};
    int i, status1, status2 = 0;

    int id = babyMaker(2);

    if (id == 0)
    {
        babyFuneral(2);
    }
    else
    {
        sleep(id);
        exit(id);
    }
}

void pl1_ex06()
{
    for (int i = 0; i < 4; i++)
    {
        pid_t p = fork();

        if (p == 0)
        {
            sleep(1);
            exit(i);
        }
    }

    babyFuneral(4);

    // A
    // |---+
    // |---|---+
    // |---|---|---+
    // |---|---|---|---+
    // |---|---|---|---|
    // It will be create 16 process, 2 ^ 4 = 16

    // B - just use a exit() in the child proces

    // C & D - Done in the code
}

void pl1_ex07()
{
    time_t t;
    srand((unsigned)time(&t));

    int numbers[ARRAY_SIZE];
    int n, x;
    int pidVec[2] = {};

    for (x = 0; x < ARRAY_SIZE; x++)
    {
        numbers[x] = rand() % 1000;
    }

    for (int a = 0; a < ARRAY_SIZE; a++)
    { //DEBUG
        //printf("%d ", numbers[a]);
    }

    n = rand() % 1000;
    int status1 = 0;

    pid_t p = fork();
    if (p == 0)
    { // Filho
        int repeatedNum = 0;
        for (int i = 0; i < ARRAY_SIZE / 2; i++)
        {
            if (numbers[i] == n)
            {
                repeatedNum++;
            }
        }

        pidVec[0] = (int)getpid();
        exit(repeatedNum);
    }

    int repeatedNum = 0;
    for (int i = ARRAY_SIZE - (ARRAY_SIZE / 2); i < ARRAY_SIZE; i++)
    {
        if (numbers[i] == n)
        {
            repeatedNum++;
        }
    }

    int p1 = waitpid(pidVec[0], &status1, 0);
    printf("Total repeated numbers = %d\n", repeatedNum + WEXITSTATUS(status1));
}

void pl1_ex08()
{
    pid_t p;

    if (fork() == 0)
    {
        printf("PID = %d\n", getpid());
        exit(0);
    }

    if ((p = fork()) == 0)
    {
        printf("PID = %d\n", getpid());
    }

    printf("Parent PID = %d\n", getpid());

    printf("Waiting... (for PID=%d)\n", p);
    waitpid(p, NULL, 0);

    printf("Enter Loop...\n");

    int i = 50;
    while (1)
    {
        if (i == 0)
        {
            break;
        }
        printf("Teste");
        i--;
    }

    // A - Parent and child PID will be shown, however one will not because it will
    // be waiting for the process to put in the background.

    // B - The child is waiting for the SIGNAL, to continue

    // C - kill getppid()
}

void pl1_ex09()
{
    int id = babyMaker(10);

    if (id == 0)
    {
        babyFuneral(10);
    }
    else
    {
        int finish = id * 100;
        for (int i = (id - 1) * 100; i < finish; i++)
        {
            printf("%d\n", i);
        }
    }

    // A
    // Theoretically they are not guarented, however every time i compile and run it shows me that
    // they are sorted, but all depends of the scheduling
}

void pl1_ex10()
{
    int numbers[ARRAY_SIZE * 2];
    int n, x;

    time_t t;
    srand((unsigned)time(&t));
    n = rand() % 1000;

    for (x = 0; x < ARRAY_SIZE * 2; x++)
    {
        numbers[x] = rand() % 1000;
    }

    int id = babyMaker(10);

    if (id == 0)
    {
        babyFuneral(10);
    }
    else
    {
        int finish = id * 200;
        for (int i = (id - 1) * 200; i < finish; i++)
        {
            if (numbers[i] == n)
            {
                exit(i);
            }
        }
        exit(255);
    }
}

void pl1_ex11()
{
    time_t t;
    srand((unsigned)time(&t));

    int numbers[ARRAY_SIZE];
    for (int x = 0; x < ARRAY_SIZE; x++)
    {
        numbers[x] = rand() % 255;
    }

    int id = babyMaker(5);

    if (id == 0)
    {
        int max = INT_MIN;
        int i, status, exitcode, p;
        for (i = 0; i < 5; i++)
        {
            p = waitpid(-1, &status, 0);
            if (WIFEXITED(status))
            {
                exitcode = WEXITSTATUS(status);
                if (exitcode > max)
                {
                    max = exitcode;
                }
            }
        }

        int newChilde = babyMaker(1);

        if (newChilde != 0)
        {
            int result[500];
            for (int i = 0; i < ARRAY_SIZE / 2; i++)
            {
                result[i] = (int)(numbers[i] * 100) / max;
                printf(" %d ", result[i]);
            }
            exit(newChilde);
        }

        babyFuneral(1);
        int result[500];
        for (int i = ARRAY_SIZE / 2; i < ARRAY_SIZE; i++)
        {
            result[i] = (int)(numbers[i] * 100) / max;
            printf(" %d ", result[i]);
        }
        printf("\n");
    }
    else
    {
        int finish = id * 200;
        int max = INT_MIN;
        for (int i = (id - 1) * 200; i < finish; i++)
        {
            if (numbers[i] > max)
            {
                max = numbers[i];
            }
        }
        exit(max);
    }

    // A & B & C & D all present in the code above
}

// A 
int spwan_childs(int n){
    babyMaker(n);
}

void pl1_ex12(){
    // B
    int id = spwan_childs(6);

    if(id == 0){
        babyFuneral(6);
    }else{
        exit(id * id);
    }
}


int main()
{
    //pl1_ex01();
    //pl1_ex02();
    //pl1_ex03();
    //pl1_ex04();
    //pl1_ex05();
    //pl1_ex06();
    //pl1_ex07();
    //pl1_ex08();
    //pl1_ex09();
    //pl1_ex10();
    //pl1_ex11();
    //pl1_ex12();
}