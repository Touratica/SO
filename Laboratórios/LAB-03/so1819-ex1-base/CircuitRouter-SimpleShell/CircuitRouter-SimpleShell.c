#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "CircuitRouter-SimpleShell.h"


#define MAX_COMMAND_LENGTH 6

int main(int argc, char** argv) {
	char command[MAX_COMMAND_LENGTH];
	char c;
	int i;
	const long long maxchildren;
	const char* errstr;

	if (argc==2){
		for (i=0; (c = argv[1][i]) != '\0'; i++){
			if (c < '0' || c > '9') {
				fprintf(stderr, "%s: invalid argument\n", argv[0]);
				displayUsage(argv[0]);
			}
		}
		maxchildren = strtonum(argv[1], 0, LLONG_MAX, &errstr);
		if (errstr) {
			errx(1, "%s: not a valid MAXCHILDREN number", argv[0]);
		}
	}

	fprintf(stdout, "$ ");
	//fgets(command, sizeof(command), stdin);
	for ((i = 0; c=getchar())!=' ' || c='\n'; i++) {
		command[i]=c;
	}
	return 0;
}

static void displayUsage (const char* appName){
	printf("Usage: %s [MAXCHILDREN]\n", argv[0]);
	printf("\tMAXCHILDREN defines the limit of child processes allowed\n");
	exit(1);
}
