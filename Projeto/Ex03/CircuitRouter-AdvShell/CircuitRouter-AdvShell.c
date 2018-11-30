
/*
// Projeto SO - exercise 1, version 1
// Sistemas Operativos, DEI/IST/ULisboa 2018-19
*/

#include "lib/commandlinereader.h"
#include "lib/vector.h"
#include "CircuitRouter-AdvShell.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

#define COMMAND_EXIT "exit"
#define COMMAND_RUN "run"

#define MAXARGS 3    // run inputfile pipe
#define BUFFER_SIZE 100

void waitForChild(vector_t *children) {
	while (1) {
		int pid, status;
		pid = wait(&status);
		if (pid < 0) {
			if (errno == EINTR) {
				/* Este codigo de erro significa que chegou signal que interrompeu a espera
                   pela terminacao de filho; logo voltamos a esperar */
				continue;
			}
			else {
				perror("Unexpected error while waiting for child.");
				exit(EXIT_FAILURE);
			}
		}
		for (int i = 0; i < vector_getSize(children); i++) {
			child_t *child = vector_at(children, i);
			if (pid == child->pid) {
				child->status = status;
			}
		}
		return;
	}
}

void printChildren(vector_t *children) {
	for (int i = 0; i < vector_getSize(children); ++i) {
		child_t *child = vector_at(children, i);
		int status = child->status;
		pid_t pid = child->pid;
		if (pid != -1) {
			const char *ret = "NOK";
			if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
				ret = "OK";
			}
			printf("CHILD EXITED: (PID=%d; return %s; %d s)\n", pid, ret, (int) TIMER_DIFF_SECONDS(child->startTime, child->stopTime));
		}
	}
	puts("END.");
}

int main (int argc, char** argv) {

	char *args[MAXARGS + 1];
	char buffer[BUFFER_SIZE];
	int MAXCHILDREN = -1;
	vector_t *children;
	int runningChildren = 0;
	
	struct sigaction xpto;
	xpto.sa_handler = &sigchldTreatment; // function to be executed on signal
	xpto.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	sigemptyset(&xpto.sa_mask);
	sigaddset(&xpto.sa_mask, SIGPIPE); // FIXME signals to be blocked on sigaction
	sigaction(SIGCHLD, &xpto, NULL);

	char *advShellPath = get_current_dir_name();
	if (advShellPath == NULL) {
			perror("Unable to set pipe's path.");
	}

	char* programName = (char*) malloc(sizeof(char) * strlen(argv[0]-1));
	// tirou ./ e acrescentar /0 no fim

	for (int i = 2; i <= strlen(argv[0]); i++){
		programName[i-2]= argv[0][i];
	}

	char *advShellPipe = (char*) malloc(sizeof(char) * (strlen(advShellPath) + strlen(programName) + strlen( ".pipe") + 1)); // 1 = \0

	strcat(advShellPipe, advShellPath);
	strcat(advShellPipe, programName);
	strcat(advShellPipe, ".pipe");

	fd_set readfds;

	if (!mkfifo(advShellPipe, 0777)) {
		perror("Unable to create pipe.");
	}

	FD_ZERO(&readfds);

	FD_SET(0, &readfds);
	FILE *shellPipe = fopen(advShellPipe,"r+");
	FD_SET(fileno(shellPipe), &readfds);

	if(argv[1] != NULL){
		MAXCHILDREN = atoi(argv[1]);
	}

	children = vector_alloc(MAXCHILDREN); 

	printf("Welcome to CircuitRouter-AdvancedShell\n\n");

	while (1) {
		int numArgs;

		if (select(2, &readfds, NULL, NULL, NULL) < 0 && errno != EINTR)
			perror("Inputs not read.");

		if (FD_ISSET(0, &readfds)){
			numArgs =  readLineArguments(0, args, MAXARGS+1, buffer, BUFFER_SIZE);
			if (numArgs < 0 || (numArgs > 0 && (strcmp(args[0], COMMAND_EXIT) == 0))) {
				sigprocmask(SIG_BLOCK, &xpto.sa_mask, NULL);
				// EOF (end of file) do stdin ou comando "sair"
				printf("CircuitRouter-AdvShell will exit.\n--\n");

				/* Espera pela terminacao de cada filho */
				while (runningChildren > 0) {
					waitForChild(children);
					runningChildren --;
				}


				printChildren(children);
				printf("--\nCircuitRouter-AdvShell ended.\n");
				break;
			}
		}
		if (FD_ISSET(fileno(shellPipe), &readfds)) {
			numArgs =  readLineArguments(fileno(shellPipe), args, MAXARGS+1, buffer, BUFFER_SIZE);
			if (numArgs > 0 && (strcmp(args[0], COMMAND_RUN) != 0)) {
				FILE *clientPipe = fopen(argv[MAXARGS-1],"a");
				fprintf(clientPipe, "Command not supported"); //FIXME nao sei se é para abrir com "a"
				fclose(clientPipe);
				continue;
			}
		}

		if (numArgs > 0 && strcmp(args[0], COMMAND_RUN) == 0) {
			int pid;
			if (numArgs < 2) {
				printf("%s: invalid syntax. Try again.\n", COMMAND_RUN);
				continue;
			}
			if (MAXCHILDREN != -1 && runningChildren >= MAXCHILDREN) {
				sigprocmask(SIG_BLOCK, &xpto.sa_mask, NULL);
				waitForChild(children);
				runningChildren--;
				sigprocmask(SIG_UNBLOCK, &xpto.sa_mask, NULL);
			}
			child_t *child = (child_t*) malloc(sizeof(child_t));
			if (child == NULL) {
				perror("Error on alocating memory for child.");
			}
			sigprocmask(SIG_BLOCK, &xpto.sa_mask, NULL);
			//marcação do tempo inicial
		
			TIMER_READ(child->startTime);
			pid = fork();
			if (pid < 0) {
				free(child);
				perror("Failed to create new process.");
				exit(EXIT_FAILURE);
			}

			if (pid > 0) {
				child->pid = pid;
				vector_pushBack(children, child);
				runningChildren++;
				sigprocmask(SIG_UNBLOCK, &xpto.sa_mask, NULL);
				printf("%s: background child started with PID %d.\n\n", COMMAND_RUN, pid);
				continue;
			} else {
				char seqsolver[] = "../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver";
				char *newArgs[3] = {seqsolver, args[1], NULL};
				FILE *clientPipe = fopen(argv[MAXARGS-1],"a");
				close(1);
				dup(fileno(clientPipe));
				execv(seqsolver, newArgs);
				perror("Error while executing child process"); // Nao deveria chegar aqui
				exit(EXIT_FAILURE);
			}
		}
		else if (numArgs == 0) {
			/* Nenhum argumento; ignora e volta a pedir */
			continue;
		}
		else {
			printf("Unknown command. Try again.\n");
		}
	}

	for (int i = 0; i < vector_getSize(children); i++) {
		free(vector_at(children, i));
	}
	vector_free(children);

	return EXIT_SUCCESS;
}

void sigchldTreatment(vector_t *children) {
	// FIXME implement the rest
	int pid, status;
	while ((pid = waitpid(-1, &status, WNOHANG)) != 0) {
		for (int i = 0; i < vector_getSize(children); i++) {
			child_t *child = vector_at(children, i);
			if (pid == child->pid) {
				TIMER_READ(child->stopTime);
				child->status = status;
			}
		}
	}
}