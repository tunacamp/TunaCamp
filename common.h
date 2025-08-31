/*
 * TunaCamp Library
 * common.h
 * Common functionality used TunaCamp modules
 * Jairo R. K. Kipnusu
 * Wed August 27, 2025
 */

#ifndef COMMON_H
#define COMMON_H


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include <ncurses.h>


/*
 * Return values for int functions
 * All my functions return int
 */
enum returns {success, memory_error, file_error, getline_error, decode_error};


/* structure for holding array of strings */
struct token_list {
	int count;
	char **items;
};

/* structure for holdind table (arrays of arrays) of strings */
struct token_table {
	int count;
	struct token_list **items;
};


/* prototypes for functions defined in common.c */
int get_token_list(char*, char*, struct token_list*);
int free_token_list(struct token_list*);
int get_token_table(FILE*, struct token_table*);
int free_token_table(struct token_table*);

#endif /* common.h */
