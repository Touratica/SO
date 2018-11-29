#ifndef CIRCUITROUTER_SHELL_H
#define CIRCUITROUTER_SHELL_H

#include "lib/vector.h"
#include "lib/timer.h"
#include <sys/types.h>

typedef struct {
    pid_t pid;
    int status;
    TIMER_T stopTime, startTime;
} child_t;

void waitForChild(vector_t *children);
void printChildren(vector_t *children);
void sigchldTreatment();

#endif /* CIRCUITROUTER_SHELL_H */
