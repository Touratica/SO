#include <stdio.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "CircuitRouter-SimpleShell.h"
#include "lib/queue.h"

struct process {
	int status;
	__pid_t pid;
}; // typedef *process_t

int main(int argc, char** argv) {
	char buffer[MAX_BUFFER], pathbuffer[MAX_BUFFER], inputbuffer[MAX_BUFFER];
	char c;
	int i;
	int status;
	long long maxchildren = 0;
	long long n_child = 0, n;
	__pid_t pid;
	queue_t* childQueue;
	int changedLine = FALSE;

	// checks for arguments passed with the executable
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
	childQueue = queue_alloc(-1); // creates queue to save the child pids and exit status
	while (TRUE){
		fprintf(stdout, "Circuit Router$ ");
		for (i = 0; (c = getchar()) != ' ' && c != '\n'; i++) {
			buffer[i]=c; // gets command
		}
		buffer[i] = '\0';

		if (!strcmp(buffer, "run")) {
			if (c == ' ') {
				changedLine = FALSE;
				for (i = 0; (c = getchar()) != '\n'; i++) {
					buffer[i] = c; // gets inputfile
				}
				buffer[i] = '\0';
			}
			else {
				changedLine = TRUE;
			}
			/* gets executable directory to get the inputfile and the SeqSolver
			executable directories*/
			if (getcwd(pathbuffer, sizeof(pathbuffer)) != NULL) {
				if (!changedLine){ 
					strcpy(inputbuffer, pathbuffer);
					strcat(inputbuffer, "/");
					strcat(inputbuffer, buffer);
				}
				strcat(pathbuffer,
				"/../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver");
				if (n_child == maxchildren && maxchildren > 0) {
					/* if more processes than the user allowed are already
					running, the parent waits until one ends*/
					pid = wait(&status);
					process_t process = (process_t) malloc(sizeof(process));
					process->pid = pid;
					process->status = status; 
					queue_push(childQueue, process);
					// pid and status are pushed to queue
					n_child--;
				}
				pid = fork();
				n_child++;
				if (pid == -1) { // if fork fails
					fprintf(stderr, "run: execution failed");
					n_child--;
					exit(EXIT_FAILURE);
				}
				else if (pid == 0){
					if (changedLine){// if the user passes no arguments with run
						execl(pathbuffer, "./CircuitRouter-SeqSolver", NULL);
						exit(EXIT_FAILURE);
					}
					else {
						execl(pathbuffer, "./CircuitRouter-SeqSolver",
						 inputbuffer, NULL) == -1 ? exit(EXIT_FAILURE) :
						 exit(EXIT_SUCCESS);
					}
				}
			}
			else { // if the path doesn't fit the buffer
				fprintf(stderr, "run: inputfile path too long\n");
			}
		}
		
		else if (!strcmp(buffer, "exit") && c == '\n')	{
			if (c == '\n') {
				for (n = n_child; n > 0; n--) {
					// waits until all run executions end
					pid = wait(&status);
					process_t process = (process_t) malloc(sizeof(process));
					process->pid = pid;
					process->status = status; 
					queue_push(childQueue, process);
					// pid and status are pushed to queue
				}
				while (!queue_isEmpty(childQueue)) {
					// gets values from queue and prints children information for every run
					process_t process = (process_t) queue_pop(childQueue);
					pid = process->pid;
					status = process->status;
					fprintf(stdout, "CHILD EXITED (PID=%d; return ", pid);
					if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
						fprintf(stdout, "OK)\n");
					}
					else {
						fprintf(stdout, "NOK)\n");
					}
					free(process);
				}
				queue_free(childQueue);
				fprintf(stdout, "END.\n");
				break;
			}
			else { // if exi is passed with arguments
				fprintf(stderr, "exit: too many arguments\n");
			}
		}
		else { // if the user inputs an unknown command
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
