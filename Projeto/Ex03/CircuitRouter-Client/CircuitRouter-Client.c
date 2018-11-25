
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
	char *advShellPipeName;

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
	mkfifo(tmpname, 0777);
	perror("Error while creating pipe for the client"); // Não deveria chegar aqui
	exit(EXIT_FAILURE);

	while (1) {
		int numArgs;

		numArgs = readLineArguments(args, MAXARGS+1, buffer, BUFFER_SIZE);

		/* EOF (end of file) do stdin ou comando "sair" */
		if (numArgs < 0) {
			printf("CircuitRouter-Client will exit.\n--\n");
			break;
		}
		else if (numArgs > 0 && strcmp(args[0], COMMAND_RUN) == 0) {
			if (numArgs < 2) {
				printf("%s: invalid syntax. Try again.\n", COMMAND_RUN);
				continue;
			}

			char *newArgs[3] = {args[1], tmpname, NULL}; // passar também o pipe deste cliente

			// Substituir por passar argumentos à AdvShell
			FILE *advShellPipe = fopen(advShellPipeName, "a");

			perror("Error while executing SeqSolver"); // Nao deveria chegar aqui
			exit(EXIT_FAILURE);
			
		}

		else if (numArgs == 0){
			/* Nenhum argumento; ignora e volta a pedir */
			continue;
		}
		else
			printf("Command not supported.\n");
	}
}
