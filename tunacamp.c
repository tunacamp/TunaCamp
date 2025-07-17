#include <string.h>
#include <stdlib.h>
#include "tunacamp.h"

#define BUFFER_SIZE 100

static char** tokenize_buffer(char*);

static void strip_newline(char* buffer) {
	for (int i = 0; i < strlen(buffer); ++i)
		if (buffer[i] == '\n')
			buffer[i] = '\0';
}


static char*** read_raw_records(FILE* file, int* count) {
	char*** record_list;
	char buffer[BUFFER_SIZE];
	char** token_list;

	record_list = malloc(sizeof(char**));
	if (record_list == NULL)
		return NULL;

	while (fgets(buffer, sizeof(buffer), file) != NULL) {
		strip_newline(buffer);
		token_list = tokenize_buffer(buffer);
		record_list[*count] = token_list;
		*count = *count + 1;

		record_list = realloc(record_list, sizeof(char**) * (*count+1));
		if (record_list == NULL)
			return NULL;
	}

	return record_list;
}


static char** tokenize_buffer(char* buffer) {
	int i = 0;
	char* token;
	char** token_list;

	token_list = malloc(sizeof(char*));
	if (token_list == NULL)
		return NULL;

	token = strtok(buffer, ":");
	while (token != NULL) {
		token_list = realloc(token_list, sizeof(char*) * (i+1));
		if (token_list == NULL)
			return NULL;

		token_list[i++] = strdup(token);
		token = strtok(NULL, ":");
	}

	return token_list;
}


int read_session_list(FILE* file, struct session_list* session_list) {
	struct session* session;
	int i;

	char*** records = read_raw_records(file, &session_list->count);

	session_list->items = calloc(session_list->count, sizeof(struct session*));
	if (session_list->items == NULL)
		return -1;

	for (i = 0; i < session_list->count; ++i) {
		session = malloc(sizeof(struct session));
		if (session == NULL)
			return i;

		session->type = atoi(records[i][0]);
		session->start_time = atol(records[i][1]);
		session->stop_time = atol(records[i][2]);
		session->id = records[i][3];

		if (session->type == 0) {
			session->start_page = atoi(records[i][4]);
			session->stop_page = atoi(records[i][5]);
		}

		session_list->items[i] = session;

		// free strings that were converted to numeric: we won't need them
		free(records[i][0]);
		free(records[i][1]);
		free(records[i][2]);
		if(session->type == 0) {
			free(records[i][4]);
			free(records[i][5]);
		}
	}

	return 0;
}


void free_session_list(struct session_list session_list) {
	for (int i = 0; i < session_list.count; ++i) {
		free(session_list.items[i]->id);
		free(session_list.items[i]);
	}
	session_list.items = NULL;
}


int read_project_list(FILE* file, struct project_list* project_list) {
	struct project* project;
	int i;

	char*** records = read_raw_records(file, &project_list->count);

	project_list->items = calloc(project_list->count, sizeof(struct project*));
	if (project_list->items == NULL)
		return -1;

	for (i = 0; i < project_list->count; ++i) {
		project = malloc(sizeof(struct project));
		if (project == NULL)
			return i;

		project->name = records[i][0];
		project->description = records[i][1];
		project_list->items[i] = project;
	}

	return 0;
}


void free_project_list(struct project_list project_list) {
	for (int i = 0; i < project_list.count; ++i) {
		free(project_list.items[i]->name);
		free(project_list.items[i]->description);
		free(project_list.items[i]);
	}

	project_list.items = NULL;
}


int read_book_list(FILE* file, struct book_list* book_list) {
	struct book* book;
	int i;

	char*** records = read_raw_records(file, &book_list->count);

	book_list->items = calloc(book_list->count, sizeof(struct book*));
	if (book_list->items == NULL)
		return -1;

	for (i = 0; i < book_list->count; ++i) {
		book = malloc(sizeof(struct book));
		if (book == NULL)
			return i;

		book->tag = atoi(records[i][0]);
		book->id = records[i][1];
		book->title = records[i][2];
		book->min_page = atoi(records[i][3]);
		book->max_page = atoi(records[i][4]);
		book_list->items[i] = book;

		free(records[i][0]);
		free(records[i][3]);
		free(records[i][4]);
	}

	return 0;
}


void free_book_list(struct book_list book_list) {
	for (int i = 0; i < book_list.count; ++i) {
		free(book_list.items[i]->id);
		free(book_list.items[i]->title);
		free(book_list.items[i]);
	}

	book_list.items = NULL;
}


int session_status(struct session* session) {
	if (session->stop_time == 0)
		return open;
	else
		return closed;
}


int find_open_session(struct session_list session_list) {
	for (int i = 0; i < session_list.count; ++i)
		if (session_status(session_list.items[i]) == open)
			return i;
	return -1;
}


int find_book(char* bookid, struct book_list book_list) {
	for (int i = 0; i < book_list.count; ++i)
		if (strcmp(book_list.items[i]->id, bookid) == 0)
			return i;
	return -1;
}


void print_book_list(struct book_list book_list) {
	int i;

	for (i = 0; i < book_list.count; ++i)
		printf("%2d %s %s\n", i, book_list.items[i]->id,
			book_list.items[i]->title);
}


void print_project_list(struct project_list project_list) {
	int i;
	for (i = 0; i < project_list.count; ++i)
		printf("%2d %s %s\n", i, project_list.items[i]->name,
			project_list.items[i]->description);
}
