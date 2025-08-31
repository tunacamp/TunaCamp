/*
 * common.c
 * Function definitions for TunaCamp library
 * Jairo R. K. Kipnusu
 * Wed August 27, 2025
 */


#include "common.h"


/*
 * get_token_list()
 * Purpose: split a string by each delimiter inside delimiters and store
 *          the resulting tokens into list
 */
int get_token_list(char *string, char *delimiters, struct token_list *list) {
	char *string_copy = NULL; // copy of string since strtok() modifies
							  //  its client
	int len = strlen(string) + 1; // length of string_copy
	char *token;

	// reset list
	list->count = 0;
	list->items = NULL;

	string_copy = calloc(len, sizeof(char));
	if (string_copy == NULL)
		return memory_error;
	memcpy(string_copy, string, len);

	// the first token
	token = strtok(string_copy, delimiters);
	if (token == NULL) {
		free(string_copy);
		return success;
	}

	list->count++;
	list->items = realloc(list->items, sizeof(char*));
	if (list->items == NULL) {
		list->count = 0;
		return memory_error;
	}
	list->items[list->count-1] = strdup(token);

	// the rest of tokens
	while ((token = strtok(NULL, delimiters)) != NULL) {
		list->count++;
		list->items = realloc(list->items, sizeof(char*) * list->count);
		if (list->items == NULL) {
			// free everything we've allocated so far before returning
			list->count--;
			free_token_list(list);
			free(string_copy);
			return memory_error;
		}
		list->items[list->count-1] = strdup(token);
	}
	free(string_copy);

	return success;
}


/* remove list of strings from memory */
int free_token_list(struct token_list *list) {
	for (int i = 0; i < list->count; i++)
		free(list->items[i]);
	free(list->items);
	list->count = 0;
	list->items = NULL;
	return success;
}


/*
 * read_records()
 * Purpose: strings from file into a table in memory
 */
int get_token_table(FILE *file, struct token_table *table) {
	char *line_buffer = NULL; // store consecutive line from file for processing
	size_t buffer_size = 0; // initial size of line_buffer
	ssize_t buffer_len; // number of characters read into line_buffer
	struct token_list *list = NULL; // store tokens from splitting each line
	int ret; // return value from split_string()
	
	// reset logs
	table->count = 0;
	table->items = NULL;

	// get and tokenize each line from file
	while ((buffer_len = getline(&line_buffer, &buffer_size, file)) != -1) {
		// skip empty line
		if (!strcmp(line_buffer, "\n"))
			continue;

		list = malloc(sizeof(struct token_list));
		if (list == NULL) {
			free_token_table(table);
			return memory_error;
		}

		// get tokens and check for errors
		ret = get_token_list(line_buffer, ":\n", list);
		if (ret == memory_error) {
			free_token_table(table);
			return memory_error;
		}

		// grow table and append new token_list
		table->count++;
		table->items = realloc(table->items,
			sizeof(struct token_table*) * table->count);
		if (table->items == NULL) {
			table->count--;
			free_token_table(table);
			return memory_error;
		}
		table->items[table->count-1] = list;
	}

	free(line_buffer);
	return success;
}


/* remove table of strings from memory */
int free_token_table(struct token_table *table) {
	int i;
	for (i = 0; i < table->count; i++) {
		free_token_list(table->items[i]);
		free(table->items[i]);
	}
	free(table->items);
	table->items = NULL;
	table->count = 0;

	return success;
}
