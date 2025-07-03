#ifndef TUNACAMP_H
#define TUNACAMP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

struct SLog {
	time_t startTime;
	time_t stopTime;
	long int isbn;
	int startPage;
	int stopPage;
};


struct PLog {
	long int isbn;
	char* ranges;
};


#define ALLOC_SIZE 100
#define SLOG_FIELD_COUNT 10
#define SLOG_WIDTH 60
#define PLOG_WIDTH 25
#define PLOG_FIELD_COUNT 3
#define MAX_LOG_WIDTH 60

int loadLogs(FILE*, char***, int*);
int parseSLogs(const char**, struct SLog**, const int);
int parsePLogs(const char**, struct PLog**, const int);

void freeLogs(char**, const int);
void freeSLogs(struct SLog**, const int);
void freePLogs(struct PLog**, const int);

#endif /* tunacamp.h */
