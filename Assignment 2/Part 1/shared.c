#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#define SHM_SIZE 1024 /*make it a 1K shared memory segment */

struct __share
{
    int no1;
    int no2;
    pid_t winner;
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

    if (argc != 3) {
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

    if ((shmid = shmget(key, sizeof(struct __share), 0644 | IPC_CREAT | IPC_EXCL )) < 0)
    {
        perror("shmget");
        exit(1);
    }

    shmptr = (struct __share *) shmat(shmid, NULL, 0);
    shmptr->no1 = 0;
    shmptr->no2 = 2;
    shmptr->winner = 0;


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

                if((shmptr->no1 >= M) || (shmptr->no2 >= M)) {
                   break;
                }

                sum = shmptr->no1 + shmptr->no2;

                if (shmptr->no1 < shmptr->no2)
                    shmptr->no1 = sum;
                else
                    shmptr->no2 = sum;

                if(sum > M) {
                    shmptr->winner = getpid();
                    break;
                }

            }

            exit(EXIT_SUCCESS);
        }
    }
    while((wait(&p_status)) > 0);

    int max = shmptr->no1 > shmptr->no2 ? shmptr->no1 : shmptr->no2;
    shmdt(&shmptr);
    shmctl(shmid, IPC_RMID, NULL);
    printf("max value %d, winner pid: %d\n", max, shmptr->winner);
    return 0;
}
