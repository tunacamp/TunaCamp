#include "tunacamp.h"

static char** tokenize(char*);

static void stripNewline(char* buffer) {
	for (int i = 0; i < strlen(buffer); ++i)
		if (buffer[i] == '\n')
			buffer[i] = '\0';
}


static char*** readFile(FILE* file, int* count) {
	char*** list;
	char buffer[BUFF_SIZE];
	char** tokens;

	list = malloc(sizeof(char**));
	if (list == NULL)
		return NULL;

	while (fgets(buffer, sizeof(buffer), file) != NULL) {
		stripNewline(buffer);
		tokens = tokenize(buffer);
		list[*count] = tokens;
		*count = *count + 1;

		list = realloc(list, sizeof(char**) * (*count+1));
		if (list == NULL)
			return NULL;
	}
	
	return list;
}


static char** tokenize(char* buffer) {
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


struct session** readSessionList(FILE* file, int* count) {
	struct session** sessions;
	struct session* session;
	int i;

	char*** list = readFile(file, count);

	sessions = calloc(*count, sizeof(struct session*));
	if (sessions == NULL)
		return NULL;

	for (i = 0; i < *count; ++i) {
		session = malloc(sizeof(struct session));
		if (session == NULL)
			return NULL;

		session->type = atoi(list[i][0]);
		session->startTime = atol(list[i][1]);
		session->stopTime = atol(list[i][2]);
		session->id = list[i][3];

		if (session->type == 0) {
			session->startPage = atoi(list[i][4]);
			session->stopPage = atoi(list[i][5]);
		}

		sessions[i] = session;

		// free strings that were converted to numeric: we won't need them
		free(list[i][0]);
		free(list[i][1]);
		free(list[i][2]);
		if(session->type == 0) {
			free(list[i][4]);
			free(list[i][5]);
		}
	}

	return sessions;
}


struct project** readProjectList(FILE* file, int* count) {
	struct project** projects;
	struct project* project;
	int i;

	char*** records = readFile(file, count);
	projects = calloc(*count, sizeof(struct project*));
	if (projects == NULL)
		return NULL;

	for (i = 0; i < *count; ++i) {
		project = malloc(sizeof(struct project));
		if (project == NULL)
			return NULL;

		project->name = records[i][0];
		project->description = records[i][1];
		projects[i] = project;
	}

	return projects;
}

struct book** readBookList(FILE* file, int* count) {
	struct book** books;
	struct book* book;
	int i;

	char*** records = readFile(file, count);

	books = calloc(*count, sizeof(struct book*));
	if (books == NULL)
		return NULL;

	for (i = 0; i < *count; ++i) {
		book = malloc(sizeof(struct book));
		if (book == NULL)
			return NULL;

		book->id = records[i][0];
		book->title = records[i][1];
		book->minPage = atoi(records[i][2]);
		book->maxPage = atoi(records[i][3]);
		book->priority = atoi(records[i][4]);
		books[i] = book;
	}

	return books;
}

int writeSessions(FILE* file, struct session** list, const int count) {
	file = freopen(NULL, "w", file);
	if (file == NULL)
		return 1;

	for (int i = 0; i < count; ++i) {
		fprintf(file, "%d:%ld:%ld:%s",
			list[i]->type,
			list[i]->startTime,
			list[i]->stopTime,
			list[i]->id);

		if (list[i]->type == study)
			fprintf(file, ":%d:%d",
				list[i]->startPage,
				list[i]->stopPage);
		fprintf(file, "\n");
	}

	return 0;
}


int findOpen(struct session** list, const int count) {
	for (int i = 0; i < count; ++i)
		if (list[i]->stopTime == 0)
			return i;
	return -1;
}


int findBook(char* bookid, struct book** books, const int count) {
	for (int i = 0; i < count; ++i)
		if (strcmp(books[i]->id, bookid) == 0)
			return i;
	return -1;
}


/*char* genDocId(void) {
	int i, c;
	char docId[DOCID_SIZE];
	bool found = false;

	srand((unsigned) time(NULL));

	while (!found) {
		i = 0;
		while (i < DOCID_SIZE) {
			c = rand() % 123;

			if (islower(c) || isdigit(c))
				docId[i++] = c;
		}

		docId[i] = '\0';	// null terminate the string;

		// make sure the id is unique
		for (i = 0; i < resourceCount; ++i)
			if (strcmp(docId, resourceList[i]->id) != 0)
				found = true;
	}

	return strdup(docId);
}*/


void printBooks(struct book** books, const int count) {
	int i;

	for (i = 0; i < count; ++i)
		printf("%2d %s %s\n", i, books[i]->id,
			books[i]->title);
}


void printProjects(struct project** projects, const int count) {
	int i;
	for (i = 0; i < count; ++i)
		printf("%2d %s %s\n", i, projects[i]->name,
			projects[i]->description);
}


/*int addProject(void) {
	char name[15], description[81];
	int i;

	printf("Project name: ");
	scanf("%s", name);
	printf("Description\n: ");
	scanf("%s", description);

	for (i = 0; i < projectCount; ++i)
		if (strcmp(projectList[i]->name, name) == 0)
			return 1;	// project with this name exista

	++projectCount;

	projectList = realloc(projectList, sizeof(struct project*) * projectCount);
	if (projectList == NULL)
		return 2;

	projectList[projectCount-1]->name = strdup(name);
	projectList[projectCount-1]->description = strdup(description);

	return 0;
}*/


/*int editProject(void) {
	int jn;	// jet number
	char name[15], description[81];

	printProjects();
	printf("Enter jet number: ");
	scanf("%d", &jn);

	if (jn < 0 || jn >= projectCount)
		return 1;

	printf("Old name: %s\n", projectList[jn]->name);
	printf("New name ('o' to use old): ");
	scanf("%s", name);
	printf("Old description: %s\n", projectList[jn]->description);
	printf("New description ('o' to use old)\n: ");
	scanf("%s", description);

	if (strcmp(name, "o") != 0 || strcmp(name, "O") != 0)
		projectList[jn]->name = strdup(name);

	if(strcmp(description, "o") != 0 || strcmp(description, "O") != 0)
		projectList[jn]->description = strdup(description);

	return 0;
}*/


int main(int argc, char** argv) {
	char* sessionsp = "test/sessions";
	char* projectsp = "test/jets";
	char* booksp = "test/resources";
	FILE* sessionsf = fopen(sessionsp, "r");
	if (sessionsf == NULL) {
		perror(sessionsp);
		return 1;
	}
	FILE* projectsf = fopen(projectsp, "r");
	if (projectsf == NULL) {
		perror(projectsp);
		return 1;
	}
	FILE* booksf = fopen(booksp, "r");
	if (booksf == NULL) {
		perror(booksp);
		return 1;
	}

	struct session** sessions;
	struct project** projects;
	struct book** books;
	int sessionCount, projectCount, bookCount;
	sessions = readSessionList(sessionsf, &sessionCount);
	projects = readProjectList(projectsf, &projectCount);
	books = readBookList(booksf, &bookCount);

	if (argc < 2) {
		fprintf(stderr, "usage: [session, project, library] [options]\n");
		return 1;
	}

	bool sessionListModified = false;
	// session
	if (strcmp(argv[1], "session") == 0) {
		if (argc < 3) {
			fprintf(stderr, "usage: session [start, stop]\n");
			return 1;
		}

		// session start
		int sn, bn, pn, sp;	// session number, book number, project number
		if (strcmp(argv[2], "start") == 0) {
			// we can't start a session when another was already started
			if ((sn = findOpen(sessions, sessionCount)) != -1) {
				fprintf(stderr, "session with id \'%s\' was already started\n",
					sessions[sn]->id);
				return 1;
			}

			struct session* session = malloc(sizeof(struct session));
			if (session == NULL) {
				perror("malloc(new session)");
				return 1;
			}

			session->startTime = time(NULL);
			session->stopTime = 0;	// we don't know the stop time yet

			printf("Enter session type(0:study, 1:project): ");
			scanf("%d", &session->type);

			switch (session->type) {
				case study:
					printBooks(books, bookCount);
					printf("Select book number(%d-%d): ",
						0, bookCount-1);
					scanf("%d", &bn);

					if (bn < 0 || bn > bookCount-1) {
						fprintf(stderr, "book number out of range\n");
						return 1;
					}
					
					session->id = books[bn]->id;

					printf("Enter start page(%d-%d): ",
						books[bn]->minPage, books[bn]->maxPage);
					scanf("%d", &session->startPage);
					// validate start page
					if (session->startPage < books[bn]->minPage ||
						session->startPage > books[bn]->maxPage) {
						fprintf(stderr, "page out of range\n");
						return 1;
					}
					session->stopPage = 0;	// we don't know the stop page yet
					break;
				case project:
					printProjects(projects, projectCount);
					printf("Enter project number(%d-%d): ",
						0, projectCount-1);
					scanf("%d", &pn);

					if (pn < 0 || pn > projectCount-1) {
						fprintf(stderr, "project number out of range\n");
						return 1;
					}
					session->id = projects[pn]->name;
					break;
				default:
					fprintf(stderr, "unknown session type: %d\n",
						session->type);
					return 1;
			}

			++sessionCount;	
			sessions = realloc(sessions, sizeof(struct session*) * sessionCount);
			if (sessions == NULL)
				return 1;

			sessions[sessionCount-1] = session;
			sessionListModified = true;
		}
		else if (strcmp(argv[2], "stop") == 0) {
			sn = findOpen(sessions, sessionCount);
			if (sn == -1) {
				fprintf(stderr, "no session was started\n");
				return 1;
			}

			bn = findBook(sessions[sn]->id, books, bookCount);

			if (sessions[sn]->type == study) {
				printf("Enter stop page(%d-%d): ",
					sessions[sn]->startPage, books[bn]->maxPage);
				scanf("%d", &sp);

				if (sp < sessions[sn]->startPage || sp > books[bn]->maxPage) {
					fprintf(stderr, "page number out of range\n");
					return 1;
				}

				sessions[sn]->stopPage = sp;
			}

			sessions[sn]->stopTime = time(NULL);
			sessionListModified = true;
		}
		else {
			fprintf(stderr, "unknown argument \'%s\'\n", argv[2]);
			return 1;
		}
	}
	else {
		fprintf(stderr, "unknown argument \'%s\'\n", argv[1]);
		return 1;
	}


	if (sessionListModified)
		writeSessions(sessionsf, sessions, sessionCount);
	return 0;
}

