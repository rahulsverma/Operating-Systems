
#ifndef PROCESS_H
#define PROCESS_H

#define MAX_BUFFER 10000

struct process_info {
    unsigned long pid;
    long ppid;
    long state;
    long cpu;
};

#endif //PROCESS_H
