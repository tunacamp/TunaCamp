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
