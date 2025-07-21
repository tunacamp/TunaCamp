#ifndef BOOKS_H
#define BOOKS_H
#define BOOKID_SIZE 13

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

enum book_tag {theoretical, practical, life};

struct book {
	int tag;
	char* id;
	char* title;
	int min_page;	// The absolute pages
	int max_page;	// in evince including only the core info.
};

struct book_list {
	struct book** items;
	int count;
};

void print_book_list(const struct book_list bl);

int find_book(const char*, const struct book_list bl);

char* gen_bookid(struct book_list bl);
#endif /* books.h */
