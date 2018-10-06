#include <stdio.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include "CircuitRouter-SimpleShell.h"


#define MAX_COMMAND_LENGTH 6

int main(int argc, char** argv) {
	char command[MAX_COMMAND_LENGTH];
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
		maxchildren = atoll(argv[1]);
		if (maxchildren < 0) {
			errx(1, "%s: not a valid MAXCHILDREN number", argv[0]);
		}
	}

	fprintf(stdout, "$ ");
	for (i = 0; (c=getchar())!=' ' || c!='\n'; i++) {
		command[i]=c;
	}
	return 0;
}

static void displayUsage (const char* appName){
	printf("Usage: %s [MAXCHILDREN]\n", appName);
	printf("\tMAXCHILDREN defines the limit of child processes allowed\n");
	exit(1);
}
