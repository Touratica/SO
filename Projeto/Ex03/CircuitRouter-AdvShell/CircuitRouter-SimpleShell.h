#ifndef CIRCUITROUTER_SHELL_H
#define CIRCUITROUTER_SHELL_H

#include "lib/vector.h"
#include <sys/types.h>
#include <time.h>

typedef struct {
    pid_t pid;
    int status;
    clock_t start,end;
} child_t;

void waitForChild(vector_t *children);
void printChildren(vector_t *children);

#endif /* CIRCUITROUTER_SHELL_H */
