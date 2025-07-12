#include "tunacamp.h"

int bookmark(char*);
bool validPageRange(const char*, const int, const int);


int startSession(void) {
	int dn, jn;	// document number, jet number

	// we cant't start a sessions when another is open
	for (int i = 0; i < sessionCount; ++i)
		if (sessionList[i]->startTime != 0 && sessionList[i]->stopTime == 0) {
			fprintf(stderr, "Error: another session is open: %s\n",
				sessionList[i]->id);
			return 2;
		}

	session* s = malloc(sizeof(session));
	if (s == NULL)
		return 1;

	time_t now = time(NULL);
	s->startTime = now;
	s->stopTime = 0;	// we don't know the stop time yet

	printf("Enter session type: ");
	scanf("%d", &s->type);

	switch (s->type) {
		case study:
			printLibrary();
			printf("Select doc number: ");
			scanf("%d", &dn);

			if (dn >= 0 && dn < documentCount)
				s->id = documentList[dn]->id;
			else {
				fprintf(stderr, "Unknown doc number: %d\n", dn);
				return 2;
			}

			printf("Enter start page: ");
			scanf("%d", &s->startPage);
			// validate start page
			if (!validPageRange( s->id, s->startPage, s->stopPage)) {
				fprintf(stderr, "Invalid page range for book: %s(%d, %d)\n",
					documentList[dn]->id,
					documentList[dn]->startPage, documentList[dn]->stopPage);
					return 2;
			}

			s->stopPage = 0;	// we don't know the stop page yet

			break;
		case programming:
			printProjects();
			printf("Enter project number: ");
			scanf("%d", &jn);

			if (jn >= 0 && jn < projectCount) 
				s->id = projectList[jn]->name;
			else {
				fprintf(stderr, "Unknown jet number: %d\n", jn);
				return 2;
			}
			break;
		default:
			fprintf(stderr, "Unknown session type: %d\n", s->type);
			return 2;
	}

	sessionCount++;

	sessionList = realloc(sessionList, sizeof(session*) * sessionCount);
	if (sessionList == NULL)
		return 1;

	sessionList[sessionCount-1] = s;

	return 0;
}


// two adjascent sessions with same session type and ID means
// the session was paused.
int pauseSession(void) {
	int i;
	time_t now = time(NULL);
	session* s;

	for (i = 0; i < sessionCount; ++i) {
		if (sessionList[i]->startTime != 0 && sessionList[i]->stopTime == 0) {

			// mark the session as stopped before cloning
			sessionList[i]->stopTime = now;

			s = malloc(sizeof(session));
			if (s == NULL)
				return 1;

			// clone session type and session ID
			s->type = sessionList[i]->type;
			s->id = sessionList[i]->id;

			// paused session has c:0:0 so that we can test during resume
			s->startTime = s->stopTime = 0;

			// more work for reading sessions
			if (s->type == study) {
				printf("Enter stop page: ");
				scanf("%d", &sessionList[i]->stopPage);
				s->startPage = sessionList[i]->stopPage;
				s->stopPage = 0;
			}

			sessionCount++;
			sessionList = realloc(sessionList, sizeof(session*) * sessionCount);
			if (sessionList == NULL)
					return 1;

			sessionList[sessionCount-1] = s;

			return 0;
		}
	}

	return 2;
}


void resumeSession(void) {
		int i;
		time_t now = time(NULL);

		for (i = 0; i < sessionCount; ++i)
			if (sessionList[i]->startTime == 0 && sessionList[i]->stopTime == 0)
				sessionList[i]->startTime = now;
}


void stopSession(void) {
	int i;
	time_t now = time(NULL);

	for (i = 0; i < sessionCount; ++i) {
		if (sessionList[i]->stopTime == 0) {
			sessionList[i]->stopTime = now;

			if (sessionList[i]->type == 'r') {
				printf("Enter stop page: ");
				scanf("%d", &sessionList[i]->stopPage);

				if (!validPageRange(sessionList[i]->id, sessionList[i]->startPage, sessionList[i]->stopPage)) {
					fprintf(stderr, "Invalid page stop for book: %s\n", sessionList[i]->id);

					// revert changes
					sessionList[i]->stopTime = sessionList[i]->stopPage = 0;
				}
			}
		}
	}
}


int bookmark(char* bookid) {
	int i, sppage = 0;

	for (i = 0; i < sessionCount; ++i)
		if (strcmp(sessionList[i]->id, bookid) == 0)
			if (sessionList[i]->startTime != 0 && sessionList[i]->stopTime != 0)
				sppage = sessionList[i]->stopPage;

	return sppage;
}


bool validPageRange(const char* bookid, const int start, const int stop) {
		int i;

		if (start > stop && stop != 0)
			return false;

		for (i = 0; i < documentCount; ++i) {
			if (strcmp(documentList[i]->id, bookid) == 0) {
				if (start < documentList[i]->startPage)
					return false;

				if (stop > documentList[i]->stopPage)
					return false;
			}
		}

		return true;
}
