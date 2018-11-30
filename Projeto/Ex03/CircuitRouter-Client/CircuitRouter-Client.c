
/*
// Projeto SO - exercise 3, version 1
// Sistemas Operativos, DEI/IST/ULisboa 2018-19
*/

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "lib/commandlinereader.h"
#include "lib/vector.h"
#include "CircuitRouter-Client.h"

#define COMMAND_RUN "run"

#define BUFFER_SIZE 100

int main (int argc, char** argv) {

	char buffer[BUFFER_SIZE];
	char received[BUFFER_SIZE];
	char *advShellPipeName;
	fd_set readfds;
	FD_ZERO(&readfds);

	FD_SET(0, &readfds);

	if (argv[1] != NULL) {
		advShellPipeName = strdup(argv[1]);
	}
	else {
		fprintf(stderr, "AdvShell pipe not specified.\n");
		exit(EXIT_FAILURE);
	}

	printf("Welcome to CircuitRouter-Client\n\n");
	
	// Usar mkstemp para criar ficheiro temporário e mkfifo para criar pipe para o client
	char template[] = "/tmp/CircuitRouter-Client.XXXXXX";
	int fdtemp = mkstemp(template);
	FILE * pipeClient = fdopen(fdtemp, "r+");
	if (!mkfifo(template, 0777)) {
		perror("Error while creating pipe for the client.");
		exit(EXIT_FAILURE);
	}
	FD_SET(fdtemp, &readfds);

	while (1) {
		if (fgets(buffer, BUFFER_SIZE, stdin) == NULL){
			break;
		}

		buffer[strlen(buffer)-1] = ' '; // replaces /n with a whitespace
		strcat(buffer, template);


		FILE *advShellPipe = fopen(advShellPipeName, "a");
		
		if (advShellPipe == NULL) {
			perror("Pipe doesn't exist."); // checks if a pipe exists (if fopen sets errno)
		}
		fprintf(advShellPipe, "%s", buffer); 
		fclose(advShellPipe);
		select(2, &readfds, NULL, NULL, NULL);
		if(FD_ISSET(fdtemp, &readfds)){
			fgets(received, BUFFER_SIZE, pipeClient);
			fprintf(stdout, "%s", received);
		}
	}
	unlink(template);
	fclose(pipeClient); 
	exit(EXIT_SUCCESS);

}
