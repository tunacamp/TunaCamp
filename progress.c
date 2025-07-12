#include "tunacamp.h"

extern struct record** gSessions;
extern struct record** gBooks;
extern int gSessionsCount;
extern int gBooksCount;

void printactivity(void) {
	int i;

	for (i = 0; i < gSessionsCount; ++i) {
		printf("%c %ld %ld %s", gSessions[i]->sessionType, gSessions[i]->startTime,
			gSessions[i]->stopTime, gSessions[i]->id);
				
		if (gSessions[i]->sessionType == 'r')
			printf(" %d %d", gSessions[i]->startPage, gSessions[i]->stopPage);

		printf("\n");
	}
}
