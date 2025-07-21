#include "sessions.h"
#include "books.h"
#include "projects.h"

#define BUFFER_SIZE 100

static int read_session_list(FILE*, struct session_list* sl);
static void free_session_list(struct session_list sl);
static int read_book_list(FILE*, struct book_list* bl);
static void free_book_list(struct book_list bl);
static int read_project_list(FILE*, struct project_list* bl);
static void free_project_list(struct project_list pl);
static char** tokenize_buffer(char* buffer);
static void strip_newline(char* str);

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


static int read_session_list(FILE* file, struct session_list* sl) {
	struct session* session;
	int i;

	char*** records = read_raw_records(file, &sl->count);

	sl->items = calloc(sl->count, sizeof(struct session*));
	if (sl->items == NULL)
		return -1;

	for (i = 0; i < sl->count; ++i) {
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

		sl->items[i] = session;

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


static void free_session_list(struct session_list sl) {
	for (int i = 0; i < sl.count; ++i)
		free(sl.items[i]);
	sl.items = NULL;
}


static int read_project_list(FILE* file, struct project_list* pl) {
	struct project* project;
	int i;

	char*** records = read_raw_records(file, &pl->count);

	pl->items = calloc(pl->count, sizeof(struct project*));
	if (pl->items == NULL)
		return -1;

	for (i = 0; i < pl->count; ++i) {
		project = malloc(sizeof(struct project));
		if (project == NULL)
			return i;

		project->name = records[i][0];
		project->description = records[i][1];
		pl->items[i] = project;
	}

	return 0;
}


static void free_project_list(struct project_list pl) {
	for (int i = 0; i < pl.count; ++i) {
		free(pl.items[i]->name);
		free(pl.items[i]->description);
		free(pl.items[i]);
	}

	pl.items = NULL;
}


static int read_book_list(FILE* file, struct book_list* bl) {
	struct book* book;
	int i;

	char*** records = read_raw_records(file, &bl->count);

	bl->items = calloc(bl->count, sizeof(struct book*));
	if (bl->items == NULL)
		return -1;

	for (i = 0; i < bl->count; ++i) {
		book = malloc(sizeof(struct book));
		if (book == NULL)
			return i;

		book->tag = atoi(records[i][0]);
		book->id = records[i][1];
		book->title = records[i][2];
		book->min_page = atoi(records[i][3]);
		book->max_page = atoi(records[i][4]);
		bl->items[i] = book;

		free(records[i][0]);
		free(records[i][3]);
		free(records[i][4]);
	}

	return 0;
}


static void free_book_list(struct book_list bl) {
	for (int i = 0; i < bl.count; ++i) {
		free(bl.items[i]->id);
		free(bl.items[i]->title);
		free(bl.items[i]);
	}

	bl.items = NULL;
}


int main(int argc, char** argv) {
#ifdef debugging
	char* session_logs_path = "test-data/sessions";
	char* project_logs_path = "test-data/jets";
	char* book_logs_path = "test-data/books";
#else
	char* session_logs_path = getenv("session_logs");
	char* project_logs_path = getenv("project_logs");
	char* book_logs_path = getenv("book_logs");
#endif

	FILE* session_logs_file = fopen(session_logs_path, "r");
	if (session_logs_file == NULL) {
		perror(session_logs_path);
		return 1;
	}

	FILE* project_logs_file = fopen(project_logs_path, "r");
	if (project_logs_file == NULL) {
		perror(project_logs_path);
		return 1;
	}

	FILE* book_logs_file = fopen(book_logs_path, "r");
	if (book_logs_file == NULL) {
		perror(book_logs_path);
		return 1;
	}

	struct session_list sl = {NULL, 0};
	struct project_list pl = {NULL, 0};
	struct book_list bl = {NULL, 0};

   	read_session_list(session_logs_file, &sl);
	read_project_list(project_logs_file, &pl);
	read_book_list(book_logs_file, &bl);

	if (argc < 2) {
		fprintf(stderr, "usage [sessionman, projectman, libraryman] [options]\n");
		return 1;
	}

	int return_value, session_number;
	if (strcmp(argv[1], "sessionman") == 0) {
		if (argc < 3) {
			char* usage = "usage: sessionman  [start, stop]\n";
			fprintf(stderr, usage);
			return  1;
		}

		if (argc > 3) {
			fprintf(stderr, "unknown argument \'%s\'\n", argv[3]);
			return 1;
		}

		if (strcmp(argv[2], "start") == 0) {
			return_value = start_session(&sl, bl, pl);
			if (return_value == 1) {
				session_number = find_open_session(sl);
				fprintf(stderr, "another session with id \'%s\' is open\n",
					sl.items[session_number]->id);
			}
			else if (return_value == 2)
				perror("");
			else if (return_value == 3)
				fprintf(stderr, "number out of range\n");
			else if (return_value == 4)
				fprintf(stderr, "unknown session type\n");
			else
				write_session_list(session_logs_file, sl);
		}
		else if (strcmp(argv[2], "stop") == 0) {
			return_value = stop_session(&sl, bl);
			if (return_value == 1)
				fprintf(stderr, "no session was started\n");
			else if (return_value == 2)
				fprintf(stderr, "number out of range\n");
			else
				write_session_list(session_logs_file, sl);
		}
		else {
			fprintf(stderr, "unknown argument \'%s\'\n", argv[2]);
			return_value =  1;
		}
	}
	else {
		fprintf(stderr, "unknown argument \'%s\'\n", argv[1]);
		return_value = 1;
	}

	fclose(session_logs_file);
	fclose(book_logs_file);
	fclose(project_logs_file);

	free_session_list(sl);
	free_book_list(bl);
	free_project_list(pl);

	return return_value;
}
