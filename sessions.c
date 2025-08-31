/* utility funtions for ttracker
 * Jairo R. K. Kiptoo August 27, 2025
 */

#include "common.h"
#include "sessions.h"


/* read_study_logs(FILE*, struct study_log_list*)
 * Purpose: Reads study logs from file pointed to by FILE* into list in
 *          memory pointed to by study_log_list.
 */
int get_study_logs(FILE *file, struct study_log_list *list) {
	struct study_log *log; // store each new log during processing
	struct token_table table = {0, NULL}; // hold table of tokens from file for
							   // processing

	// reset the list
	list->count = 0;
	list->items = NULL;

	int ret = get_token_table(file, &table);
	if (ret == memory_error)
		return memory_error;

	if (table.count == 0)
		return success; // empty file
	
	list->count = table.count;
	list->items = calloc(list->count, sizeof(struct study_log*));
	if (list->items == NULL) {
		free_token_table(&table);
		return memory_error;
	}

	// process table entries
	for (int i = 0; i < table.count; i++) {
		// check if all fields are present
		if (table.items[i]->count != 5) {
			printf("less\n");
			free_token_table(&table);
			free_study_logs(list);
			return decode_error;
		}

		// allocate memory for new log
		log = malloc(sizeof(struct study_log));
		if (log == NULL) {
			free_token_table(&table);
			free_study_logs(list);
			return memory_error;
		}

		// assign log
		log->book_id = atoi(table.items[i]->items[0]);
		log->start_time = atol(table.items[i]->items[1]);
		log->stop_time = atol(table.items[i]->items[2]);
		log->start_page = atoi(table.items[i]->items[3]);
		log->stop_page = atoi(table.items[i]->items[4]);
		list->items[i] = log;
	}

	free_token_table(&table);
	return success;
}


/* remove list of study logs from memory */
int free_study_logs(struct study_log_list *list) {
	for (int i = 0; i < list->count; i++)
		free(list->items[i]);
	free(list->items);
	list->count = 0;
	list->items = NULL;

	return success;
}
