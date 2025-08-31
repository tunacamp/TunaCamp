#ifndef UNIX_EXPLORER_H
#define UNIX_EXPLORER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define NUMBER_OF_FIELDS 6

// return values for funtions that return int
enum returns{success, mem_err, file_err, decode_err,
		getline_err, dup_err, noname_err};


// information about items in the Unix system
struct object {
	int index; // object index
	char *name;	// object name
	char *package; // package in which object belongs
	char *description; // short description of object
	// list of available manual sections for object
	struct {
		int count;
		int *items;
	} sections;
	// list of other manual sections mentioned in object's man page
	struct {
		int count;
		int **items;
	} pointers;
};

struct object_list {
	int count; // number or objects in list
	struct object **items; // list of objects
};


struct string_list {
	int count; // number of substrings
	char **items; // list of substrings
};

int parse_objects(FILE*, struct object_list*); // parser.c
int process_record(char*, struct object*); // parser.c
int process_section_list(char*, struct object*);
int process_pointer_list(char*, struct object*);
void free_objects(struct object_list); // utils.c
int split_string(char*, char*, struct string_list*); // utils.c
void print_object(struct object_list, struct object *object); // utils.c
int add_object(struct object_list*);
int write_objects(FILE*, struct object_list);
int add_pointer_list(char*, struct object*, struct object_list*);
#endif /* UNIX_EXPLORER_H */
