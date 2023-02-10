#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <sched.h>

#define SHM_SIZE 1024 /*make it a 1K shared memory segment */

struct __share
{
    int no1;
    int no2;
    pid_t winner;
    sem_t sem;
};


/*Taking inputs for arguments*/
int main(int argc, char *argv[])
{
    key_t key;
    int shmid;
    char *data;
    int mode;

    int p_status = -1;
    struct __share * shmptr;

    if (argc != 3)
    {
        perror("program requires two arguments");
        exit(EXIT_FAILURE);
    }

    system("touch ./shared_mem");

    int P = atoi(argv[1]);
    int M = atoi(argv[2]);

    if ((key = ftok("./shared_mem", 'R')) < 0)
    {
        perror("ftok");
        exit(1);
    }

    if ((shmid = shmget(key, sizeof(struct __share), 0644 | IPC_CREAT)) < 0)
    {
        perror("shmget");
        exit(1);
    }

    shmptr = (struct __share *) shmat(shmid, NULL, 0);
    shmptr->no1 = 0;
    shmptr->no2 = 1;
    shmptr->winner = 0;
    sem_init(&shmptr->sem, 1, 1);

    for (size_t i = 0; i < P; i++)
    {
        pid_t mypid = fork();

        if (mypid == -1)
        {
            printf("process creation failed");
            exit(EXIT_FAILURE);
        }

        /*We're in the child process */
        if (mypid == 0)
        {
            int sum = 0;
            while (1)
            {
                if (sem_wait(&shmptr->sem) == 0)
                {
                    printf("acquired semaphore pid:%d\n", getpid());
                    if ((shmptr->no1 >= M) || (shmptr->no2 >= M))
                    {
                        sem_post(&shmptr->sem);
                        printf("release semaphore pid: %d\r\n", getpid());
                        break;
                    }
                    else
                    {
                        sum = shmptr->no1 + shmptr->no2;

                        if (shmptr->no1 < shmptr->no2)
                            shmptr->no1 = sum;
                        else
                           shmptr->no2 = sum;

                        if (sum >= M)
                        {
                            shmptr->winner = getpid();
                            printf("release semaphore pid: %d\r\n", getpid());
                            sem_post(&shmptr->sem);
                            break;
                         }
                        sem_post(&shmptr->sem);
                        printf("release semaphore pid: %d\r\n", getpid());
                     }
                }
                else
                    perror("sem_try_wait failed:");

             //sleep(5);
            }

            exit(EXIT_SUCCESS);
        }
    }

    while ((wait(&p_status)) > 0);

    int max = shmptr->no1 > shmptr->no2 ? shmptr->no1 : shmptr->no2;
    shmdt(&shmptr);
    shmctl(shmid, IPC_RMID, NULL);
    printf("max value %d, winner pid: %d\n", max, shmptr->winner);
	system("rm ./shared_mem");
    return 0;
}
