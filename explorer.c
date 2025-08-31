/* General function utilities
 * Jairo R. K. Kipnusu 08/25/2025
 */

#include "explorer.h"

/*
// static funtion prototypes


// split a string into substrings
// return list of substring
int split_string(char *string, char *delimiters, struct string_list *list) {
	int i;
	char *token = NULL;
	char copy[strlen(string)+1];

	for (i = 0; i < (int) strlen(string)+1; i++) {
		copy[i] = string[i];
		if (copy[i] == '\n')
			copy[i] = '\0';
	}

	list->count = 0;
	list->items = NULL;

	token = strtok(copy, delimiters);
	if (token == NULL)
		return success;

	list->count++;
	list->items = malloc(sizeof(char*));
	if (list->items == NULL)
		return mem_err;
	list->items[list->count-1] = strdup(token);

	while((token = strtok(NULL, delimiters)) != NULL) {
		list->count++;
		list->items = realloc(list->items, sizeof(char*) * list->count);
		list->items[list->count-1] = strdup(token);
	}

	return success;
}


// print object's info
void print_object(struct object_list list, struct object *object) {
	int i, j;
	printf("Name:\t\t%s\n", object->name);
	printf("Package:\t%s\n", object->package);
	printf("Description:\t%s\n", object->description);
	printf("Sections:\t");
	for (i = 0; i < object->sections.count; i++) {
		printf("%d", object->sections.items[i]);
		if (i != object->sections.count-1)
			printf(",");
	}
	printf("\nPointers:\t");
	for (i = 0; i < object->pointers.count; i++) {
		for (j = 0; j < list.count; j++) {
			if (list.items[j]->index == i)
				printf("%s(%d)", list.items[j]->name,
				object->pointers.items[i][1]);
		}
		if (i != object->pointers.count-1)
			printf(",");
	}
	printf("\n");
}


// free object list from memory
void free_objects(struct object_list list) {
	for (int i = 0; i < list.count; i++) {
		free(list.items[i]->name);
		free(list.items[i]->package);
		free(list.items[i]->description);
		free(list.items[i]->sections.items);

		for (int j = 0; j < list.items[i]->pointers.count; j++)
			free(list.items[i]->pointers.items[j]);
		free(list.items[i]->pointers.items);
		free(list.items[i]);
	}
	free(list.items);
	list.items = NULL;
}


// add an object to the list in memory
int add_object(struct object_list *list) {
	struct object *object;
	char *buffer;
	size_t size = 0;
	ssize_t len = 0;

	printf("Name: ");
	switch((len = getline(&buffer, &size, stdin))) {
		case -1: return getline_err;
		case 1: return noname_err;
		default:
				buffer[len-1] = '\0'; // replace '\n' with '\0'
				// check if object already exists in list
				for (int i = 0; i < list->count; i++)
					if (!strcmp(list->items[i]->name, buffer)) {
						fprintf(stderr, "record with name '%s' exists\n",
							buffer);
						return dup_err;
					}
				object = malloc(sizeof(struct object));
				object->name = strdup(buffer); break;
	}
	free(buffer);
	printf("Package: ");
	size = 0;
	switch(len = getline(&buffer, &size, stdin)) {
		case -1: return getline_err;
		case 1: object->package = NULL; break;
		default:
				buffer[len-1] = '\0';
				object->package = strdup(buffer);
	}
	free(buffer);

	printf("Description: ");
	size = 0;
	switch (len = getline(&buffer, &size, stdin)) {
		case -1: return getline_err;
		case 1: object->description = NULL; break;
		default:
				buffer[len-1] = '\0';
				object->description = strdup(buffer);
	}
	free(buffer);

	printf("Sections: ");
	size = 0;
	switch(len = getline(&buffer, &size, stdin)) {
		case -1: return getline_err;
		case 1:
				 object->sections.items = NULL;
				 object->sections.count = 0;
				 break;
		default:
				process_section_list(buffer, object);
	}
	free(buffer);

	printf("Pointers: ");
	size = 0;
	switch(len = getline(&buffer, &size, stdin)) {
		case -1: return getline_err;
		case 1:
				object->pointers.items = NULL;
				object->pointers.count = 0;
				break;
		default:
				add_pointer_list(buffer, object, list);
				printf("pointer list addition successful\n");
	}
	free(buffer);

	list->count++;
	list->items = realloc(list->items, sizeof(struct object) * list->count);
	if (list->items == NULL)
		return mem_err;
	object->index = list->count-1;
	list->items[list->count-1] = object;
	
	return success;
}


int write_objects(FILE* file, struct object_list list) {
	int i, j;
	for (i = 0; i < list.count; i++) {
		fprintf(file, "%d:%s:%s:%s:",
			list.items[i]->index,
			list.items[i]->name,
			list.items[i]->package,
			list.items[i]->description);

		for (j = 0; j < list.items[i]->sections.count; j++) {
			fprintf(file, "%d", list.items[i]->sections.items[j]);
			if (j != list.items[i]->sections.count-1)
				fprintf(file, ",");
			else
				fprintf(file, ":");
		}

		for (j = 0; j < list.items[i]->pointers.count; j++) {
			fprintf(file, "%d-%d",
				list.items[i]->pointers.items[j][0],
				list.items[i]->pointers.items[j][1]);
			if (j != list.items[i]->pointers.count-1)
				fprintf(file, ",");
			else
				fprintf(file, "\n");
		}
	}

	return success;
}


int add_pointer_list(char *buffer, struct object *object,
	struct object_list *list) {
	struct string_list strings, substrings;

	int i, j;

	split_string(buffer, ",", &strings);
	object->pointers.items = calloc(strings.count, sizeof(int*));
	object->pointers.count = strings.count;

	for (i = 0; i < strings.count; i++) {
		split_string(strings.items[i], "()", &substrings);
		object->pointers.items[i] = calloc(2, sizeof(int));

		// check if referenced object exists
		for (j = 0; j < list->count; j++) {
			if (!strcmp(list->items[j]->name, substrings.items[0])) {
				// just copy its index if it exists
				object->pointers.items[i][0] = list->items[j]->index;
				printf("%d\n", list->items[j]->index);
			}
			else {
				// add it to the list first
				list->count++;
				list->items = realloc(list->items,
					sizeof(struct object*) * list->count);
				struct object *o = malloc(sizeof(struct object));
				o->index = list->count-1;
				o->name = strdup(substrings.items[0]);
				list->items[list->count-1] = o;

				object->pointers.items[i][0] = list->count-1;
			}
			object->pointers.items[i][1] = atoi(substrings.items[1]);
		}
	}

	return success;
}*/
