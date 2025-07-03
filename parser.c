#include "tunacamp.h"


/* int loadLogs(const FILE* logfile, char** logs, int* count)
 * Synopsis:
 * 		Read logs from a file stream.
 *		Logfile is read line by line, each line is stripped-off '\n'
 *		and copied to log[i] where i is the line number.
 * Arguments:
 *		const FILE* logfile - a pointer to a file descriptor to read from.
 *		char** logs - pointer to list of strings which will hold the logs.
 *		int* count - pointer to an int which is modified to reflect the number of
 * 					logs.
 * Return:
 * 	0 - success i.e logs holds *count entries.
 * 	1 - memory allocation for logs failed. *count is assigned 0.
 * 	2 - memory reallocation for logs failed in which case logs is cleaned
 * 	    and assigned NULL. Also *count is assigned 0.
 */
int loadLogs(FILE* logfile, char*** logs, int* logCount) {
	char log[MAX_LOG_WIDTH];
	int count = 0;

	logs = calloc(ALLOC_SIZE, sizeof(char*));
	if (logs == NULL) {
		*logCount = 0;
		return 1;
	}

	while (fgets(log, sizeof(log), logfile) != NULL) {
		log[strlen(log)-1] = '\0';	// strip newline
		logs[count] = strdup(log);

		if (count % ALLOC_SIZE == 0) {
			logs = realloc(logs, sizeof(char*) * (ALLOC_SIZE + count));
			if (logs == NULL) {
				freeLogs(logs, count);
				logs = NULL;
				*logCount = 0;
				return 2;
			}
		}
		count++;
	}
	*logCount = count;
	return 0;
}



/* int parseSLogs(const char** logs, struct SLog** slogs, const int count)
 * Synopsys:
 * 	Parse session logs from string logs.
 * 	Read each string log in logs list and extracting fields from it and
 * 		stroring them in a struct SLog which is linked to the slogs list.
 * 	A session log is expected to be in this format:
 * 		isbn:day:month:year:hour:min:hour:min:page:page (10 fields).
 * Arguments:
 * 	const char** logs - a list of string logs to chew.
 * 	struct SLog** slogs - list to link our struct SLog logs.
 * 	const int count - number of logs (assigned in loadLogs()).
 * Return:
 * 	0 - success i.e slogs holds count struct SLog entries.
 * 	1 - failed to allocate memory for a struct SLog during processing.
 */
int parseSLogs(const char** logs, struct SLog** slogs, const int count) {
	struct SLog* slog;
	int i;

	struct tm startTime, stopTime;

	for (i = 0; i < count; ++i) {
		slog = malloc(sizeof(struct SLog));
		if (slog == NULL)
			return 1;
		sscanf(logs[i], "%ld:%d:%d:%d:%d:%d:%d:%d:%d:%d",
				&slog->isbn,
				&startTime.tm_mday, &startTime.tm_mon, &startTime.tm_year,
				&startTime.tm_hour, &startTime.tm_min,
				&stopTime.tm_hour, &stopTime.tm_min,
				&slog->startPage, &slog->stopPage);
		stopTime.tm_mday = startTime.tm_mday;
		stopTime.tm_mon = startTime.tm_mon;
		stopTime.tm_year = startTime.tm_year;

		slog->startTime = mktime(&startTime);
		slog->stopTime = mktime(&stopTime);

		slogs[i] = slog;
	}
	return 0;
}

/*
 * int parsePLogs(const char** logs, stuct PLog** plogs, const int count)
 * Synopsis:
 * 		Convert each log in logs to a struct PLog and link it to plogs list.
 * Arguments:
 * 		const char** logs - list of logs to chew.
 * 		struct PLog** plogs - the list to link to.
 * 		const int count - numbers of logs we're working upon.
 * Return:
 * 		0 - success i.e plogs holds count parsed logs.
 * 		1 - failed to allocate memory for some plog.
 */
int parsePLogs(const char** logs, struct PLog** plogs, const int count) {
	struct PLog* plog;
	int i;

	for (i = 0; i < count; ++i) {
		plog = malloc(sizeof(struct PLog));
		if (plog == NULL)
			return 1;

		sscanf(logs[i], "%ld:%s", &plog->isbn, plog->ranges);
		plogs[i] = plog;
	}
	return 0;
}


void freeSLogs(struct SLog** slogs, const int logCount) {
	for (int i = 0; i < logCount; ++i)
		free(slogs[i]);
	free(slogs);
}


void freePLogs(struct PLog** plogs, const int logCount) {
	for (int i = 0; i < logCount; ++i)
		free(plogs[i]);
	free(plogs);
}


void freeLogs(char** logs, const int logcount) {
	for (int i = 0; i < logcount; ++i)
		free(logs[i]);
	free(logs);
}
