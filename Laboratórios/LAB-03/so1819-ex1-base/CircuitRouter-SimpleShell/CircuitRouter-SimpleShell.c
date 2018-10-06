#include <stdio.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "CircuitRouter-SimpleShell.h"

#define MAX_BUFFER 256
#define TRUE 1

int main(int argc, char** argv) {
	char buffer[MAX_BUFFER], pathbuffer[MAX_BUFFER], inputbuffer[MAX_BUFFER];
	char c;
	int i;
	int status;
	long long maxchildren = 0;
	long long n_child = 0;
	__pid_t* pids; 
	__pid_t pid;

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
	if (maxchildren != 0) {
		pids = (__pid_t*) malloc(sizeof(__pid_t) * maxchildren);
	}
	fprintf(stdout, "******************************************************************************\n\n");
	fprintf(stdout, "                        CIRCUIT ROUTER - SIMPLE SHELL\n\n");
	fprintf(stdout, "******************************************************************************\n");

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
						if (++n_child > maxchildren) {
							pid = wait(&status);
							n_child--;
						}
						i = 0;
						while (pids[i++] == 0);
						pids[i] = fork();
						if (pids[i] == 0) {
							execl(pathbuffer, "./CircuitRouter-SeqSolver", inputbuffer, NULL);
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
			// TODO waits until all children end, outputs result and exits app
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
