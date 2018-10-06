#include <stdio.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 6

int main(int argc, char** argv) {
	char command[MAX_COMMAND_LENGTH];
	fprintf(stdout, "$ ");
	fgets(command, sizeof(command), stdin);
	if (!strcmp(command, "run\n")) {
		fprintf(stdout, "Tada!!!!\n");
		return 0;
	}
	return 1;
}