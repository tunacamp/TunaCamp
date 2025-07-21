#ifndef SESSIONS_H
#define SESSIONS_H

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "projects.h"
#include "books.h"

enum session_type {study, project};
enum session_status {open, closed};

struct session {
	int type;
	time_t start_time;
	time_t stop_time;
	char* id;
	int start_page;
	int stop_page;
};

struct session_list {
	struct session** items;
	int count;
};

int start_session(struct session_list* sl, const struct book_list bl,
	const struct project_list pl);

int find_open_session(const struct session_list sl);

int session_status(const struct session* s);

void print_reading_gaps(const struct session_list sl, const struct book_list bl,
	const char* bookid);

int start_session(struct session_list* sl, const struct book_list bl,
	const struct project_list pl);

int stop_session(struct session_list* sl, struct book_list bl);

int write_session_list(FILE* file, struct session_list sl);
#endif /* sessions.h */
