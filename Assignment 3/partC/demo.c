
#include <stdio.h>
#include <stdlib.h>



int main(char argc, char **argv)
{

    FILE *fp = NULL;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char buffer[25] = {0};

    fp = fopen("/dev/sudeep_rahul", "r");
    if(fp) {
         while ((read = getline(&line, &len, fp)) != -1) {
            printf("%s", line);
       }
    }
    else
      printf("failed to open character device\n");



return 0;
}
