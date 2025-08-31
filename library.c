/*
 * Functions for library module
 * Jairo R. K. Kipnusu
 * Thursday August 28, 2025
 */

#include "common.h"
#include "library.h"


// prototype for static functions
static int get_author_list(char*, struct document*);
static int get_subject_list(char*, struct document*);
static int get_isbn_list(char*, struct document*);


/* external functions */

/*
 * get_libary_catalogue()
 * Load document catalogue from a file into memory
 */
int load_document_table(FILE* file, struct document_table *table) {
	struct token_table token_table = {0, NULL};
	struct document* doc;
	int ret, i;

	// reset table
	table->count = 0;
	table->items = NULL;

	// get document token table from file
	ret = get_token_table(file, &token_table);
	if (ret == memory_error)
		return memory_error;

	for (i = 0; i < token_table.count; i++) {
		// check each record for few fields
		if (token_table.items[i]->count != FIELDS_IN_DOCINFO) {
			free_token_table(&token_table);
			free_document_table(table);
			return decode_error;
		}

		// memory for each new document record
		doc = malloc(sizeof(struct document));
		if (doc == NULL) {
			free_token_table(&token_table);
			free_document_table(table);
			return memory_error;
		}

		// record processing
		doc->id = atoi(token_table.items[i]->items[0]);
		doc->type = atoi(token_table.items[i]->items[1]);
		doc->title = strdup(token_table.items[i]->items[2]);
		if (strcmp(token_table.items[i]->items[3], "none"))
			doc->edition = atoi(token_table.items[i]->items[3]);
		else
			doc->edition = EMPTY;
		if (strcmp(token_table.items[i]->items[6], "none"))
			doc->publisher = atoi(token_table.items[i]->items[6]);
		else
			doc->publisher = EMPTY;
		if (strcmp(token_table.items[i]->items[7], "none"))
			doc->year = atoi(token_table.items[i]->items[7]);
		else
			doc->year = EMPTY;
		get_author_list(token_table.items[i]->items[4], doc);
		get_isbn_list(token_table.items[i]->items[5], doc);
		get_subject_list(token_table.items[i]->items[8], doc);

		/*
		 * expand memory for document table
		 * cleanup before return any error
		 */
		table->count++;
		table->items = realloc(table->items,
			sizeof(struct document*) * table->count);
		if (table->items == NULL) {
			table->count--;
			free_document_table(table);
			free_token_table(&token_table);
			return memory_error;
		}

		table->items[i] = doc;
	}

	// cleanup before return
	free_token_table(&token_table);
	return success;
}


// Free table of documents
int free_document_table(struct document_table *table) {
	int i;

	for (i = 0; i < table->count; i++) {
		free(table->items[i]->title);
		free(table->items[i]->authors.items);
		table->items[i]->authors.count = 0;
		table->items[i]->authors.items = NULL;
		free(table->items[i]->isbns.items);
		table->items[i]->isbns.count = 0;
		table->items[i]->isbns.items = NULL;
		free(table->items[i]->subjects.items);
		table->items[i]->subjects.count = 0;
		table->items[i]->subjects.items = NULL;
		free(table->items[i]);
	}
	free(table->items);
	table->count = 0;
	table->items = NULL;
	return success;
}


// Read list of authors from file into memory
int load_author_table(FILE *file, struct author_table *table) {
	struct token_table token_table = {0, NULL};
	struct author *a;
	int ret, i;

	// get file contents
	ret = get_token_table(file, &token_table);

	// reset author table
	table->count = 0;
	table->items = NULL;

	if (ret == memory_error)
		return memory_error;

	// process file contents
	for(i = 0; i < token_table.count; i++) {
		// see if we have fewer than expected fields
		if (token_table.items[i]->count < 4) {
			fprintf(stderr,
				"DecodeError - load_author_table(): line %d of file:"
				"require %d fields "
				"but got %d\n", i+1, 4, token_table.items[i]->count);
			free_token_table(&token_table);
			free_author_table(table);
			return decode_error;
		}

		// memory for another author info
		a = malloc(sizeof(struct author));
		if (a == NULL) {
			free_author_table(table);
			free_token_table(&token_table);
			return memory_error;
		}

		// actual processing
		a->id = atoi(token_table.items[i]->items[0]);
		a->first = strdup(token_table.items[i]->items[1]);
		a->middle = strdup(token_table.items[i]->items[2]);
		a->last = strdup(token_table.items[i]->items[3]);


		// table growth and appending
		table->count++;
		table->items = realloc(table->items,
			sizeof(struct author) * table->count);
		if (table->items == NULL) {
			table->count--;
			free_token_table(&token_table);
			free_author_table(table);
			return memory_error;
		}
		table->items[i] = a;
	}

	free_token_table(&token_table);
	return success;
}


// free author table from memory
int free_author_table(struct author_table *author_table) {
	int i;
	for (i = 0; i < author_table->count; i++) {
		free(author_table->items[i]->first);
		free(author_table->items[i]->middle);
		free(author_table->items[i]->last);
		free(author_table->items[i]);
	}
	free(author_table->items);
	author_table->count = 0;
	author_table->items = NULL;

	return success;
}


// Read list of publishers from file into memory
int load_publisher_table(FILE *file, struct publisher_table *table) {
	struct token_table token_table = {0, NULL};
	struct publisher *p;
	int i, ret;

	// reset table
	table->count = 0;
	table->items = NULL;

	ret = get_token_table(file, &token_table);
	if (ret == memory_error)
		return memory_error;

	for (i = 0; i < token_table.count; i++) {
		p = malloc(sizeof(struct publisher));
		if (p == NULL) {
			free_publisher_table(table);
			free_token_table(&token_table);
			return memory_error;
		}

		p->id = atoi(token_table.items[1]->items[0]);
		p->name = strdup(token_table.items[i]->items[1]);

		table->count++;
		table->items = realloc(table->items,
			sizeof(struct publisher) * table->count);
		if (table->items == NULL) {
			table->count--;
			free_publisher_table(table);
			free_token_table(&token_table);
			return memory_error;
		}
		table->items[i] = p;
	}

	free_token_table(&token_table);
	return success;
}


// remove table of publishers from momory
int free_publisher_table(struct publisher_table *table) {
	for (int i = 0; i < table->count; i++) {
		free(table->items[i]->name);
		free(table->items[i]);
	}
	free(table->items);
	table->count = 0;
	table->items = NULL;
	return success;
}

/*
// Read list of subjects from file into memory
int read_subjects(FILE* file, struct subjects* list) {
	char* buffer;
	ssize_t buffer_len;
	size_t buffer_size = 0;
	char** tokens;
	struct subject* s;
	int count;

	list->count = 0;
	list->items = NULL;

	while ((buffer_len = getline(&buffer, &buffer_size, file)) != -1) {
		list->count++;
		list->items = realloc(list->items, sizeof(struct subject*) * list->count);
		if(list->items == NULL)
			return mem_error;
		s = malloc(sizeof(struct subject));
		if (s == NULL)
			return mem_error;

		tokens = split_string(buffer, ":", &count);
		if (count != 2)
			return decode_error;

		s->id = atoi(tokens[0]);
		free(tokens[0]);
		s->name = tokens[1];
		list->items[list->count-1] = s;
	}
	free(buffer);

	return success;
}


// Display list of documents in the library
void print_catalogue(struct catalogue* cat) {
	for (int i = 0; i < cat->count; i++)
		printf("%02d:%s\n", cat->items[i]->id, cat->items[i]->title);
}
*/

/* static functions */

/* get list of author ids from a comma separated list in a document record */
static int get_author_list(char *string, struct document *document) {
	struct token_list tokens;
	int ret, i;

	// empty field
	if (strcmp(string, "none")) {
		document->authors.count = 0;
		document->authors.items = NULL;
		return success;
	}

	document->authors.count = 0;
	document->authors.items = NULL;

	// get list of author ids as string
	ret = get_token_list(string, ",", &tokens);
	if (ret == memory_error)
		return memory_error;

	// allocate memory for list of author ids
	document->authors.count = tokens.count;
	document->authors.items = calloc(tokens.count, sizeof(int));
	if (document->authors.items == NULL) {
		free_token_list(&tokens);
		document->authors.count = 0;
	}

	for (i = 0; i < tokens.count; i++)
		document->authors.items[i] = atoi(tokens.items[i]);
	free_token_list(&tokens);

	return success;
}


/* get list of subject ids from raw document record */
static int get_subject_list(char *string, struct document *document) {
	struct token_list token_list;
	int ret, i;

	if (strcmp(string, "none")) {
		document->subjects.count = 0;
		document->subjects.items = NULL;
		return success;
	}

	document->subjects.count = 0;
	document->subjects.items = NULL;

	ret = get_token_list(string, ",", &token_list);
	if (ret == memory_error)
		return memory_error;

	document->subjects.count = token_list.count;
	document->subjects.items = calloc(token_list.count, sizeof(int));
	if (document->subjects.items == NULL) {
		free_token_list(&token_list);
		document->subjects.count = 0;
	}

	for (i = 0; i < token_list.count; i++)
		document->subjects.items[i] = atoi(token_list.items[i]);
	free_token_list(&token_list);

	return success;
}


/* get list of isbns */
static int get_isbn_list(char *string, struct document *document) {
	struct token_list token_list;
	int ret, i;

	if (strcmp(string, "none")) {
		document->isbns.count = 0;
		document->isbns.items = NULL;
		return success;
	}

	document->isbns.count = 0;
	document->isbns.items = NULL;

	ret = get_token_list(string, ",", &token_list);
	if (ret == memory_error)
		return memory_error;

	document->isbns.count = token_list.count;
	document->isbns.items = calloc(token_list.count, sizeof(long));
	if (document->isbns.items == NULL) {
		free_token_list(&token_list);
		document->isbns.count = 0;
	}

	for (i = 0; i < token_list.count; i++)
		document->isbns.items[i] = atol(token_list.items[i]);
	free_token_list(&token_list);

	return success;
}
