
/*
// Projeto SO - exercise 1, version 1
// Sistemas Operativos, DEI/IST/ULisboa 2018-19
*/

#include "CircuitRouter-AdvShell.h"
#include "lib/commandlinereader.h"
#include "lib/vector.h"
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

vector_t *children;
int runningChildren = 0;

void waitForChild() {
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
				perror("Unexpected error while waiting for child.\n");
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

void printChildren() {
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
	
	struct sigaction xpto;
	xpto.sa_handler = &sigchldTreatment; // function to be executed on signal
	xpto.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	sigemptyset(&xpto.sa_mask);
	sigaddset(&xpto.sa_mask, SIGPIPE); // FIXME signals to be blocked on sigaction
	sigaction(SIGCHLD, &xpto, NULL);

	sigset_t *sigset = (sigset_t*) malloc(sizeof(sigset_t));
	sigemptyset(sigset);
	sigaddset(sigset, SIGPIPE);
	sigaddset(sigset, SIGCHLD);

	char* programName = (char*) malloc(sizeof(char) * (strlen(argv[0]) + 15));
	// tirou ./ e acrescentar /0 no fim
	for (int i = 0; i <= strlen(argv[0]); i++) {
		programName[i] = argv[0][i];
	}

	strcat(programName, ".pipe");

	fd_set readfds;


	if (mkfifo(programName, 0777)) {
		perror("Unable to create pipe.\n");
	}

	FILE *shellPipe = fopen(programName, "r+");

	if(argv[1] != NULL){
		MAXCHILDREN = atoi(argv[1]);
	}

	children = vector_alloc(MAXCHILDREN); 
	FD_ZERO(&readfds);
	FD_SET(0, &readfds);
	FD_SET(fileno(shellPipe), &readfds);

	printf("Welcome to CircuitRouter-AdvancedShell\n\n");

	while (1) {
		int numArgs;
		bool_t isPipe = FALSE;
		

		if (select(2, &readfds, NULL, NULL, NULL) < 0 && errno != EINTR)
			perror("Inputs not read.\n");
		
		if (FD_ISSET(0, &readfds)){
			numArgs = readLineArguments(stdin, args, MAXARGS + 1, buffer, BUFFER_SIZE);
			if (numArgs < 0 || (numArgs > 0 && (strcmp(args[0], COMMAND_EXIT) == 0))) {

				sigprocmask(SIG_BLOCK, sigset, NULL);
				// EOF (end of file) do stdin ou comando "sair"
				printf("CircuitRouter-AdvShell will exit.\n--\n");

				/* Espera pela terminacao de cada filho */
				while (runningChildren > 0) {
					waitForChild();
					runningChildren--;
				}


				printChildren();

				printf("--\nCircuitRouter-AdvShell ended.\n");
				break;
			}
		}
		if (FD_ISSET(fileno(shellPipe), &readfds)) {
			numArgs =  readLineArguments(shellPipe, args, MAXARGS+1, buffer, BUFFER_SIZE);
			isPipe = TRUE;
			if (numArgs > 0 && (strcmp(args[0], COMMAND_RUN) != 0)) {
				FILE *clientPipe = fopen(args[MAXARGS-1],"a");
				fprintf(clientPipe, "Command not supported\n"); //FIXME nao sei se é para abrir com "a"
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
				sigprocmask(SIG_BLOCK, sigset, NULL);
				waitForChild();
				runningChildren--;
				sigprocmask(SIG_UNBLOCK, sigset, NULL);
			}
			child_t *child = (child_t*) malloc(sizeof(child_t));
			if (child == NULL) {
				perror("Error on alocating memory for child.\n");
			}
			sigprocmask(SIG_BLOCK, sigset, NULL);
			
			//marcação do tempo inicial
			TIMER_READ(child->startTime);

			pid = fork();
			if (pid < 0) {
				free(child);
				perror("Failed to create new process.\n");
				exit(EXIT_FAILURE);
			}

			if (pid > 0) {
				child->pid = pid;
				vector_pushBack(children, child);
				runningChildren++;
				sigprocmask(SIG_UNBLOCK, sigset, NULL);
				printf("%s: background child started with PID %d.\n\n", COMMAND_RUN, pid);
				continue;
			} else {
				char seqsolver[] = "../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver";
				char *newArgs[3] = {seqsolver, args[1], NULL};
				if (isPipe){
					FILE *clientPipe = fopen(args[MAXARGS-1],"a");
					close(1);
					dup(fileno(clientPipe));
				}
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
	unlink(programName);

	return 0;
}

void sigchldTreatment() {
	int pid, status;
	pid = waitpid(-1, &status, WNOHANG);
	for (int i = 0; i < vector_getSize(children); i++) {
		child_t *child = vector_at(children, i);
		if (pid == child->pid) {
			TIMER_READ(child->stopTime);
			child->status = status;
			runningChildren--;
		}
	}
}