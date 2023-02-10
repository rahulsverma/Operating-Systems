#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "process.h"

#define U_TASK_RUNNING                  0x0000
#define U_TASK_INTERRUPTIBLE            0x0001
#define U_TASK_UNINTERRUPTIBLE          0x0002
#define U_TASK_STOPPED                  0x0004
#define U_TASK_TRACED                   0x0008

#define U_TASK_PARKED                   0x0040
#define U_TASK_DEAD                     0x0080
#define U_TASK_WAKEKILL                 0x0100
#define U_TASK_WAKING                   0x0200
#define U_TASK_NOLOAD                   0x0400
#define U_TASK_NEW                      0x0800

#define U_TASK_KILLABLE                 (U_TASK_WAKEKILL | U_TASK_UNINTERRUPTIBLE)
#define UU_TASK_STOPPED                 (U_TASK_WAKEKILL | U_TASK_STOPPED)
#define UU_TASK_TRACED                  (U_TASK_WAKEKILL | U_TASK_TRACED)

#define UU_TASK_IDLE                    (U_TASK_UNINTERRUPTIBLE | U_TASK_NOLOAD)

#define UU_TASK_NORMAL                  (U_TASK_INTERRUPTIBLE | U_TASK_UNINTERRUPTIBLE)


char *get_process_state(long state)
{
        switch(state){
        case U_TASK_RUNNING:
                return "TASK_RUNNING";
        break;

        case U_TASK_INTERRUPTIBLE:
                return "TASK_INTERRUPTIBLE";
        break;

        case U_TASK_UNINTERRUPTIBLE:
                return "TASK_UNINTERRUPTIBLE";
        break;

        case U_TASK_STOPPED:
                return "TASK_STOPPED";
        break;

        case U_TASK_TRACED:
                return "TASK_TRACED";
        break;

        case U_TASK_PARKED:
                return "TASK_PARKED";
        break;

        case U_TASK_DEAD:
                return "TASK_DEAD";
        break;

        case U_TASK_WAKEKILL:
                return "TASK_WAKEKILL";
        break;

        case U_TASK_WAKING:
                return "TASK_WAKING";
        break;

        case U_TASK_NOLOAD:
                return "TASK_NOLOAD";

        break;
        case U_TASK_NEW:
                return "TASK_NEW";
        break;

        case UU_TASK_IDLE:
                return "TASK_IDLE";
        break;
        case UU_TASK_NORMAL:
                return "TASK_NORMAL";
        break;


        }

}

int main(char argc, char **argv)
{

   int fp;
   struct process_info uinfo[MAX_BUFFER];
   int pcount = 0;
   int i = 0;

    fp = open("/dev/process_list", O_RDONLY);
    if(fp) {

                pcount = read(fp, uinfo, sizeof(uinfo));
           if ( pcount < 0 ) {

                  printf("failed to read from device.\n");
                        exit(EXIT_FAILURE);
                }
                while(i < pcount ){
                        printf( "PID= %ld  PPID= %ld  CPU= %ld   STATE= %s\n",uinfo[i].pid , uinfo[i].ppid ,uinfo[i].cpu ,get_process_state(uinfo[i].state));
                        i++;
                }

    }
    else{
      printf("failed to open character device\n");
          exit(EXIT_FAILURE);
        }


return 0;
}
