/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * This code is an adaptation of the Lee algorithm's implementation originally included in the STAMP Benchmark
 * by Stanford University.
 *
 * The original copyright notice is included below.
 *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) Stanford University, 2006.  All Rights Reserved.
 * Author: Chi Cao Minh
 *
 * =============================================================================
 *
 * Unless otherwise noted, the following license applies to STAMP files:
 *
 * Copyright (c) 2007, Stanford University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of Stanford University nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY STANFORD UNIVERSITY ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL STANFORD UNIVERSITY BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * =============================================================================
 *
 * CircuitRouter-SeqSolver.c
 *
 * =============================================================================
 */


#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "lib/list.h"
#include "maze.h"
#include "router.h"
#include "lib/timer.h"
#include "lib/types.h"
#include <pthread.h>

enum param_types {
	PARAM_BENDCOST	= (unsigned char)'b',
	PARAM_THREADNUM = (unsigned char)'t',
	PARAM_XCOST		= (unsigned char)'x',
	PARAM_YCOST		= (unsigned char)'y',
	PARAM_ZCOST		= (unsigned char)'z',
};

enum param_defaults {
	PARAM_DEFAULT_BENDCOST	= 1,
	PARAM_DEFAULT_THREADNUM	= 0,
	PARAM_DEFAULT_XCOST		= 1,
	PARAM_DEFAULT_YCOST		= 1,
	PARAM_DEFAULT_ZCOST		= 2,
};

bool_t global_doPrint = TRUE;
char* global_inputFile = NULL;
long global_params[256]; /* 256 = ascii limit */
char* global_string;


/* =============================================================================
 * displayUsage
 * =============================================================================
 */
static void displayUsage (const char* appName){
	printf("Usage: %s [options] input_filename\n", appName);
	puts("\nOptions:                            (defaults)\n");
	printf("    b <INT>    [b]end cost          (%i)\n", PARAM_DEFAULT_BENDCOST);
	printf("    t <INT>    [t]hread number      (%i)\n", PARAM_DEFAULT_THREADNUM);
	printf("    x <UINT>   [x] movement cost    (%i)\n", PARAM_DEFAULT_XCOST);
	printf("    y <UINT>   [y] movement cost    (%i)\n", PARAM_DEFAULT_YCOST);
	printf("    z <UINT>   [z] movement cost    (%i)\n", PARAM_DEFAULT_ZCOST);
	printf("    h          [h]elp message       (false)\n");
	exit(1);
}


/* =============================================================================
 * setDefaultParams
 * =============================================================================
 */
static void setDefaultParams (){
	global_params[PARAM_BENDCOST] = PARAM_DEFAULT_BENDCOST;
	global_params[PARAM_THREADNUM] = PARAM_DEFAULT_THREADNUM;
	global_params[PARAM_XCOST]    = PARAM_DEFAULT_XCOST;
	global_params[PARAM_YCOST]    = PARAM_DEFAULT_YCOST;
	global_params[PARAM_ZCOST]    = PARAM_DEFAULT_ZCOST;
	
}


/* =============================================================================
 * parseArgs
 * =============================================================================
 */
static void parseArgs (long argc, char* const argv[]){
	long opt;

	opterr = 0;

	setDefaultParams();

	while ((opt = getopt(argc, argv, "hb:t:w:x:y:z:")) != -1) {
		switch (opt) {
			case 'b':
			case 't':
				global_params[(unsigned char)opt] = atol(optarg);
				break;
			case 'w':
				global_string = optarg;
				break;
			case 'x':
			case 'y':
			case 'z':
				global_params[(unsigned char)opt] = atol(optarg);
				break;
			case '?':
			case 'h':
				displayUsage(argv[0]);
			default:
				break;
		}
	}

		if (global_params[PARAM_THREADNUM] <= 0) {
		fprintf(stderr, "Missing thread number\n");
		displayUsage(argv[0]);
	}

	if (optind >= argc) {
		fprintf(stderr, "Missing input file\n");
		displayUsage(argv[0]);
	}

	global_inputFile = argv[optind];
}

/* =============================================================================
 * outputFile
 * =============================================================================
 */
FILE * outputFile() {
	FILE *fp;

	char result_outputFile[strlen(global_inputFile) + strlen(".res") + 1];
	sprintf(result_outputFile, "%s.res", global_inputFile);

	if (access(result_outputFile, F_OK) == 0) {
		char old_outputFile[strlen(global_inputFile) + strlen(".res.old") + 1];
		sprintf(old_outputFile, "%s.res.old", global_inputFile);
		if (rename(result_outputFile, old_outputFile) == -1) {
			perror("Error renaming output file");
			exit(EXIT_FAILURE);;
		}
	}
	fp = fopen(result_outputFile, "wt");
	if (fp == NULL) {
		perror("Error opening output file");
		exit(EXIT_FAILURE);
	}
	return fp;
}


/* =============================================================================
 * main
 * =============================================================================
 */
int main(int argc, char** argv){
	/*
	 * Initialization
	 */
	parseArgs(argc, argv);
	FILE* resultFp = outputFile();
	maze_t* mazePtr = maze_alloc();
	assert(mazePtr);
	int p; // to assert mutex operations
	long numPathToRoute = maze_read(mazePtr, global_inputFile, resultFp);
	router_t* routerPtr = router_alloc(global_params[PARAM_XCOST],
									   global_params[PARAM_YCOST],
									   global_params[PARAM_ZCOST],
									   global_params[PARAM_BENDCOST]);
	assert(routerPtr);
	list_t* pathVectorListPtr = list_alloc(NULL);
	assert(pathVectorListPtr);



	// Creates as many threads as specified by user
	pthread_t tid[global_params[PARAM_THREADNUM]];
	locks_t * fine_locks=(locks_t*)malloc(sizeof(locks_t));

	//allocs memory to a fine_locks->grid_lock[x][y][z]
	assert(fine_locks);

	fine_locks->grid_lock = (pthread_mutex_t ***)malloc(mazePtr->gridPtr->width * sizeof(pthread_mutex_t**));
	assert(fine_locks->grid_lock);

	for (long x = 0; x < mazePtr->gridPtr->width; x++){ 
		fine_locks->grid_lock[x] = (pthread_mutex_t **)malloc(mazePtr->gridPtr->height * sizeof(pthread_mutex_t*));
		assert(fine_locks->grid_lock[x]);

		for (long y = 0; y < mazePtr->gridPtr->height; y++) {
			fine_locks->grid_lock[x][y] = (pthread_mutex_t *)malloc(mazePtr->gridPtr->depth * sizeof(pthread_mutex_t));
			assert(fine_locks->grid_lock[x][y]);
		}
	}
	
	// initializes grid mutexes
	for (long x = 0; x < mazePtr->gridPtr->width; x++)
		for (long y = 0; y < mazePtr->gridPtr->height; y++) 
			for (long z = 0; z < mazePtr->gridPtr->depth; z++) {
				p = pthread_mutex_init(&(fine_locks->grid_lock[x][y][z]),NULL);
				assert(p == 0);
			}

	//initializes queue of pair coordinates mutex
	p = pthread_mutex_init(&(fine_locks->queue_lock),NULL);
	assert(p == 0);

	//initializes vector path mutex
	p = pthread_mutex_init(&(fine_locks->pathVector_lock),NULL);
	assert(p == 0);

	router_solve_arg_t routerArg = {routerPtr, mazePtr, pathVectorListPtr, fine_locks, global_string};
	TIMER_T startTime;
	TIMER_READ(startTime);

	for (int i = 0; i < global_params[PARAM_THREADNUM]; i++) {
		// If not successful, gives error message and exits program
		if (pthread_create(&tid[i], NULL, (void *)router_solve, (void *)&routerArg)) {
			fprintf(stderr, "Unable to create thread.\n");
			exit(1);
		} 
	}

	for (int i = 0; i < global_params[PARAM_THREADNUM]; i++) {
		if (pthread_join(tid[i], NULL)) {
			fprintf(stderr, "Unable to join thread.\n");
			exit(1);
		}
	}

	TIMER_T stopTime;
	TIMER_READ(stopTime);

	long numPathRouted = 0;
	list_iter_t it;
	list_iter_reset(&it, pathVectorListPtr);
	while (list_iter_hasNext(&it, pathVectorListPtr)) {
		vector_t* pathVectorPtr = (vector_t*)list_iter_next(&it, pathVectorListPtr);
		numPathRouted += vector_getSize(pathVectorPtr);
	}
	fprintf(resultFp, "Paths routed    = %li\n", numPathRouted);
	fprintf(resultFp, "Elapsed time    = %f seconds\n", TIMER_DIFF_SECONDS(startTime, stopTime));

	// frees grid mutexes
	for (long x = 0; x < mazePtr->gridPtr->width; x++) 
		for (long y = 0; y < mazePtr->gridPtr->height; y++) 
			for (long z = 0; z < mazePtr->gridPtr->depth; z++) {
				p = pthread_mutex_destroy(&(fine_locks->grid_lock[x][y][z]));
				assert(p == 0);
			}

	//frees queue of pair coordinates mutex
	p = pthread_mutex_destroy(&(fine_locks->queue_lock));
	assert(p == 0);

	//frees vector path mutex
	p = pthread_mutex_destroy(&(fine_locks->pathVector_lock));
	assert(p == 0);

	//frees everything in mutexes' structure
	for (long x = 0; x < mazePtr->gridPtr->width; x++){ 
		for (long y = 0; y < mazePtr->gridPtr->height; y++) 
			free(fine_locks->grid_lock[x][y]);

		free(fine_locks->grid_lock[x]);
	}
	free(fine_locks->grid_lock);
	free(fine_locks);

	/*
	 * Check solution and clean up
	 */
	assert(numPathRouted <= numPathToRoute);
	bool_t status = maze_checkPaths(mazePtr, pathVectorListPtr, resultFp, global_doPrint);
	assert(status == TRUE);
	fputs("Verification passed.\n",resultFp);

	maze_free(mazePtr);
	router_free(routerPtr);

	list_iter_reset(&it, pathVectorListPtr);
	while (list_iter_hasNext(&it, pathVectorListPtr)) {
		vector_t* pathVectorPtr = (vector_t*)list_iter_next(&it, pathVectorListPtr);
		vector_t* v;
		while((v = vector_popBack(pathVectorPtr))) {
			// v stores pointers to longs stored elsewhere; no need to free them here
			vector_free(v);
		}
		vector_free(pathVectorPtr);
	}
	list_free(pathVectorListPtr);

	fclose(resultFp);
	// pthread_exit(NULL);
	exit(0);
}


/* =============================================================================
 *
 * End of CircuitRouter-SeqSolver.c
 *
 * =============================================================================
 */
