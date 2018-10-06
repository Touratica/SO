#include <stdio.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include "CircuitRouter-SimpleShell.h"

#define MAX_BUFFER 256
#define TRUE 1

int main(int argc, char** argv) {
	char command[MAX_BUFFER];
	char c;
	int i;
	long long maxchildren;


	if (argc==2){
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

	while (TRUE){
		fprintf(stdout, "$ ");
		for (i = 0; (c = getchar()) != ' ' || c != '\n'; i++) {
			command[i]=c;
		}
		command[i+1] = '\0';
		if (!strcmp(command, "run")) {
			if (c == ' ') {	
				for (i = 0; (c = getchar()) != '\n'; i++) {
					command[i]=c;
				}
				command[i+1] = '\0';  
				if (access(command, R_OK)) {
					fprintf(stderr, "run: input file is unreadable");
				}
				else {
					// TODO execute CircuitRouter-SeqSolver
				}
			}
			else {
				fprintf(stderr, "run: missing file operand");
			}
		}
		
		else if (!strcmp(command, "exit") && c == '\n')	{
			if (c == '\n') {
			// TODO waits until all children end, outputs result and exits app
			fprintf(stdout, "END.");
			break;
			}
			else {
				fprintf(stderr, "exit: too many arguments");
			}
		}
		else {
			fprintf(stderr, "%s: command not found", command);
		}
	}
	return 0;
}

static void displayUsage (const char* appName){
	printf("Usage: %s [MAXCHILDREN]\n", appName);
	printf("\tMAXCHILDREN defines the limit of child processes allowed.\n");
	printf("\tIf MAXCHILDREN equals 0, the number is unlimited.");
	exit(1);
}
