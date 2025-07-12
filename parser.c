#include "tunacamp.h"


char** tokenize(char*);

int parseList(FILE* file, const int type) {
	char buffer[BUFF_SIZE];
	char** tokens;

	session* s;
	document* d;
	project* j;

	// initialize the list
	if (type == sessions) {
			sessionCount = 0;
			sessionList = malloc(sizeof(session*));
			if (sessionList == NULL)
				return 1;
	}
	else if (type == documents) {
			documentCount = 0;
			documentList = malloc(sizeof(document*));
			if (documentList == NULL)
				return 1;
	}
	else if (type == projects) {
			projectCount = 0;
			projectList = malloc(sizeof(project*));
			if (projectList == NULL)
				return 1;
	}
	else
		return -1;

	while (fgets(buffer, sizeof(buffer), file) != NULL) {
			// strip newline
			for (int i = 0; i < strlen(buffer); ++i)
				if (buffer[i] == '\n')
						buffer[i] = '\0';

		tokens = tokenize(buffer);

		// appending to and growing the list dynamically
		switch(type) {
			case documents:
				d = malloc(sizeof(document));
				if (d == NULL)
					return 1;

				d->type = atoi(tokens[0]);
				d->id = tokens[1];
				d->title = tokens[2];
				d->startPage = atoi(tokens[3]);
				d->stopPage = atoi(tokens[4]);
				d->priority = atoi(tokens[5]);
				d->status = atoi(tokens[6]);

				documentList[documentCount] = d;
				++documentCount;

				documentList = realloc(documentList, sizeof(document*) * (documentCount+1));
				if (documentList == NULL)
					return 1;
				break;
			case sessions:
				s = malloc(sizeof(session));
				if (s == NULL)
					return 1;

				s->type = atoi(tokens[0]);
				s->startTime = atol(tokens[1]);
				s->stopTime = atol(tokens[2]);
				s->id = tokens[3];

				if (s->type == study) {
					s->startPage = atoi(tokens[4]);
					s->stopPage = atoi(tokens[5]);
				}

				sessionList[sessionCount] = s;
				++sessionCount;

				sessionList = realloc(sessionList, sizeof(session) * (sessionCount+1));
				if (sessionList == NULL)
					return 1;
				break;
			case projects:
				j = malloc(sizeof(project));
				if (j == NULL)
					return 1;

				j->name = tokens[1];
				j->description = tokens[2];
				j->status = atoi(tokens[0]);

				projectList[projectCount] = j;
				++projectCount;

				projectList = realloc(projectList, sizeof(project*) * (projectCount+1));
				if (projectList == NULL)
					return 1;
				break;
		}
	}

	return 0;
}


char** tokenize(char* buffer) {
		int i = 0;
		char* token;
		char** tokens;

		tokens = malloc(sizeof(char*));
		if (tokens == NULL)
			return NULL;

		token = strtok(buffer, ":");
		while (token != NULL) {
			tokens = realloc(tokens, sizeof(char*) * (i+1));
			if (tokens == NULL)
				return NULL;

			tokens[i++] = strdup(token);
			token = strtok(NULL, ":");
		}

		return tokens;
}


void freeList(const int type) {
	int i;

	switch (type) {
		case documents:
			for (i = 0; i < documentCount; ++i) {
				free(documentList[i]->id);
				free(documentList[i]->title);
				free(documentList[i]);
			}

			documentList = NULL;
			break;
		case sessions:
			for (i = 0; i < sessionCount; ++i) {;
				free(sessionList[i]->id);
				free(sessionList[i]);
			}

			sessionList = NULL;
			break;
		case projects:
			for (i = 0; i < projectCount; ++i) {
				free(projectList[i]->name);
				free(projectList[i]->description);
				free(projectList[i]);
			}

			projectList = NULL;
			break;
		default:
			;
			break;
	}
}


int writeList(const int type) {
	int i;
	FILE* outFile;

	switch (type) {
		case sessions:
			outFile = freopen(NULL, "w", sessionListFile);
			break;
		case projects:
			outFile = freopen(NULL, "w", projectListFile);
			break;
		case documents:
			outFile = freopen(NULL, "w", documentListFile);
			break;
		default:
			return 2;
	}

	if (outFile == NULL)
		return 1;

	switch (type) {
		case documents:
			for (i = 0; i < documentCount; ++i)
				fprintf(outFile, "%d:%s:%s:%d:%d:%d:%d\n",
					documentList[i]->type,
					documentList[i]->id,
					documentList[i]->title,
					documentList[i]->startPage,
					documentList[i]->stopPage,
					documentList[i]->priority,
					documentList[i]->status);
			break;
		case sessions:
			for (i = 0; i < sessionCount; ++i) {
				fprintf(outFile, "%d:%ld:%ld:%s",
					sessionList[i]->type,
					sessionList[i]->startTime,
					sessionList[i]->stopTime,
					sessionList[i]->id);

				if (sessionList[i]->type == study) {
					fprintf(outFile, ":%d:%d",
						sessionList[i]->startPage,
						sessionList[i]->stopPage);
				}

				fprintf(outFile, "\n");
			}
			break;
		case projects:
			for (i = 0; i < projectCount; ++i) {
				fprintf(outFile, "%d:%s:%s\n",
					projectList[i]->status,
					projectList[i]->name,
					projectList[i]->description);
			}
			break;
		default:
			return 2;
	}

	return 0;
}
