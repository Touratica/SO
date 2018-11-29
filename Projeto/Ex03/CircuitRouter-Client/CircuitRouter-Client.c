
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
#include "CircuitRouter-SimpleShell.h"

#define COMMAND_RUN "run"

#define MAXARGS 2
#define BUFFER_SIZE 100

int main (int argc, char** argv) {

	char *args[MAXARGS + 1];
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
		fprintf(stderr, "AdvShell pipe not specified.");
		exit(EXIT_FAILURE);
	}

	printf("Welcome to CircuitRouter-Client\n\n");
	
	// Usar mkstemp para criar ficheiro temporário e mkfifo para criar pipe para o client
	char *template = "/tmp/CircuitRouter-Client.XXXXXX";
	char *tmpname = mkstemp(template);
	if (!mkfifo(tmpname, 0777)) {
		perror("Error while creating pipe for the client.");
		exit(EXIT_FAILURE);
	}
	FILE * readPipe = fopen(tmpname, "r+");
	FD_SET(fileno(readPipe), &readfds);
	while (1) {
		if (fgets(buffer, BUFFER_SIZE, stdin)==NULL){
			break;
		}
		buffer[strlen(buffer)-1] = ' '; // replaces /n with a whitespace
		strcat(buffer, tmpname);

		// TODO verificar erros
		FILE *advShellPipe = fopen(advShellPipeName, "a");
		if (advShellPipe == NULL) {
			perror("Pipe doesn't exist."); //checks if a pipe exists (if fopen sets errno)
		}
		fprintf(advShellPipe, buffer); 
		fclose(advShellPipe); //verificar se -1
		select(2, &readfds, NULL, NULL, NULL);
		if(FD_ISSET(fileno(readPipe), &readfds)){
			fgets(received, BUFFER_SIZE,readPipe);
			fprintf(received,"%s");
		}
		//if (FD_ISSET(0,&readfds))

	}
	fclose(readPipe); 
	unlink(tmpname);
	exit(EXIT_SUCCESS);

}
