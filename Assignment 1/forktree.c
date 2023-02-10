#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Taking inputs for arguments*/
int main(int argc, char *argv[]) 
{
  if (argc < 1) {
    perror("program requires two arguments");
    exit(EXIT_FAILURE);
  }

  int level = atoi(argv[1]);
  int nodes = atoi(argv[2]);

  int p_status = -1;
  int cpid = getpid();
  int ppid = getppid();


  if (level <= 0 || nodes <= 0) { 
    printf("level 0 reached. Parent is %d\n", ppid);
    exit(EXIT_SUCCESS);
  }

  printf("--------------------------------------------------------\n");
  printf("(%d): Process starting\n", cpid);
  printf("(%d): Parent's id = (%d)\n", cpid, ppid);
  printf("(%d): Level in the tree = %d\n", cpid, level);
  printf("(%d): Creating %d children at Level = %d\n", cpid, nodes, level);
  printf("--------------------------------------------------------\n");

  if (level > 0) {
    for (size_t i = 0; i < nodes; i++) {

      pid_t mypid = fork();

      if (mypid == -1) {
        printf("process creation failed");
        exit(EXIT_FAILURE);
      }

      /* We're in the child process */
      if (mypid == 0) {
        char lvl[3] = {0};
        char nd[3] = {0};

        sprintf(lvl, "%d", level - 1);
        sprintf(nd, "%d", nodes);
        sleep(5);
        execl(argv[0], argv[0], lvl, nd, NULL);  /* Using exec as recursion to create new child processes*/
        exit(EXIT_SUCCESS);
      }
      /* wait in parent process */
      else {
        while ((wait(&p_status)) > 0);
      }
    }
  }
  printf("(%d): Termination at Level (%d)\n", cpid, level);
}

