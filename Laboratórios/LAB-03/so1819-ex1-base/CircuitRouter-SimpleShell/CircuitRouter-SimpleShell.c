#include <stdio.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "CircuitRouter-SimpleShell.h"
#include "../lib/queue.h"

#define MAX_BUFFER 256
#define TRUE 1

int main(int argc, char** argv) {
	char buffer[MAX_BUFFER], pathbuffer[MAX_BUFFER], inputbuffer[MAX_BUFFER];
	char c;
	int i;
	int status;
	long long maxchildren = 0;
	long long n_child = 0, n;
	__pid_t pid;
	queue_t* childQueue;

	if (argc > 2) {
		fprintf(stderr, "%s: too many arguments\n", argv[0]);
		displayUsage(argv[0]);
	}
	else if (argc==2){
		for (i=0; (c = argv[1][i]) != '\0'; i++){
			if (c < '0' || c > '9') {
				fprintf(stderr, "%s: invalid argument\n", argv[0]);
				displayUsage(argv[0]);
			}
		}
		maxchildren = atoll(argv[1]); //string->long long
		if (maxchildren < 0) { // if maxchildren > LLONG_MAX
			errx(1, "%s: not a valid MAXCHILDREN number", argv[0]);
		}
	}
	fprintf(stdout, "******************************************************************************\n\n");
	fprintf(stdout, "                        CIRCUIT ROUTER - SIMPLE SHELL\n\n");
	fprintf(stdout, "******************************************************************************\n");
	childQueue = queue_alloc(10);
	while (TRUE){
		fprintf(stdout, "Circuit Router$ ");
		for (i = 0; (c = getchar()) != ' ' && c != '\n'; i++) {
			buffer[i]=c; // gets command
		}
		buffer[i] = '\0';

		if (!strcmp(buffer, "run")) {
			if (c == ' ') {
				for (i = 0; (c = getchar()) != '\n'; i++) {
					buffer[i] = c; // gets inputfile
				}
				buffer[i] = '\0';
				if (getcwd(pathbuffer, sizeof(pathbuffer)) != NULL) {
					strcpy(inputbuffer, pathbuffer);
					strcat(inputbuffer, "/");
					strcat(inputbuffer, buffer);
					if (access(inputbuffer, R_OK)) {
						fprintf(stderr, "run: input file is unreadable\n");
					}
					else {
						strcat(pathbuffer, "/../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver");
						if (++n_child > maxchildren && maxchildren > 0) {
							pid = wait(&status);
							queue_push(childQueue, &pid);
							queue_push(childQueue, &status);
							n_child--;
						}
						pid = fork();
						n_child++;
						if (pid == 0) {
							if (execl(pathbuffer, "./CircuitRouter-SeqSolver", inputbuffer, NULL) == -1) {
								exit(EXIT_FAILURE);
							}
							else {
								exit(EXIT_SUCCESS);
							}
						}
					}
				}
				else {
					fprintf(stderr, "run: inputfile path too long\n");
				}
			}
			else {
				fprintf(stderr, "run: missing file operand\n");
			}
		}
		
		else if (!strcmp(buffer, "exit") && c == '\n')	{
			if (c == '\n') {
			for (n = n_child; n > 0; n--) {
				pid = wait(&status);
				queue_push(childQueue, &pid);
				queue_push(childQueue, &status);
			}
			while (!queue_isEmpty(childQueue)) {
				int* status = (int*) queue_pop(childQueue);
				__pid_t* pid = (__pid_t*) queue_pop(childQueue);
				fprintf(stdout, "CHILD EXITED (PID=%d; return ", *pid);
				if (*status == EXIT_SUCCESS) {
					fprintf(stdout, "OK)\n");
				}
				else {
					fprintf(stdout, "NOK)\n");
				}
			}
			fprintf(stdout, "END.\n");
			break;
			}
			else {
				fprintf(stderr, "exit: too many arguments\n");
			}
		}
		else {
			fprintf(stderr, "%s: command not found\n", buffer);
			while (c != '\n' && getchar() != '\n');
		}
	}
	return 0;
}

static void displayUsage (const char* appName){
	printf("Usage: %s [MAXCHILDREN]\n", appName);
	printf("\tMAXCHILDREN defines the limit of child processes allowed.\n");
	printf("\tIf MAXCHILDREN equals 0 or isn't defined, the number is unlimited.\n");
	exit(1);
}
