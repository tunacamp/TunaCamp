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
#ifndef PROJECTS_H
#define PROJECTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct project {
	char* name;
	char* description;
};

struct project_list {
	struct project** items;
	int count;
};

void print_project_list(const struct project_list);

int add_project(struct project_list* pl);

int edit_project(struct project_list* pl);
#endif /* projects.h */
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



#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <unistd.h>

#define BUFFER_SIZE 100
#define SECONDS_IN_HOUR 3600.00
#define to_hours(x)	((x)/SECONDS_IN_HOUR)

enum return_values {success, failure, open_session_present, open_session_absent,
		null_mem, null_env, no_logs,
		null_file, invalid_argument, invalid_bookid, invalid_page, no_bookmark};
enum session_status {open, closed};
enum date_filters {day, week, month, year};

struct book {
	int id;
	int reading_list;
	char* title;
	int min_page;	// The absolute pages
	int max_page;	// in evince including only the core info.
};

struct books {
	struct book** items;
	int count;
};

struct session {
	time_t start_time;
	time_t stop_time;
	int id;
	int start_page;
	int stop_page;
};

struct sessions {
	struct session** items;
	int count;
};

struct date {
	int day;
	int month;
	int year;
};


int read_sessions(struct sessions* sessions);
int read_books(struct books* books);
bool is_integer(char* string);
int find_open_session(const struct sessions sessions);
int session_status(const struct session session);
void print_gaps(const struct book b, const struct sessions  s);
int write_sessions(struct sessions sessions);
int week_number(const struct tm tm);
void print_activity(const struct sessions sessions, const struct books books);
void plot_bar(const char* label, double percent);
int bookmark(const int bookid, const struct sessions sessions);
void print_books(const struct books books);
void print_summary(const struct sessions sessions,
	const int filter, const struct tm tm);
int start_session(struct sessions* sessions, struct books books);
int stop_session(const struct sessions sessions, const struct books books);

void reading_profile(const struct book b, const struct sessions sessions,
	bool* profile);
int print_progress(const struct book b, const struct sessions sessions);
int total_pages_read(const struct book b, const struct sessions sessions);
float average_rate(const struct book b, const struct sessions sessions);
float average_hours_per_day(const struct book b, const struct sessions sessions);
void print_gaps(const struct book b, const struct sessions sessions);
void print_activity(const struct date d, const struct sessions s,
	const struct books b);

