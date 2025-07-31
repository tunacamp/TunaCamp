#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define BUFFER_SIZE 100
#define SECONDS_IN_HOUR 3600.00
#define to_hours(x)	((x)/SECONDS_IN_HOUR)

enum session_type {study, project};
enum session_status {open, closed};
enum return_values {
	success, failure, open_session_present,
	open_session_absent, null_mem, null_env, no_logs, null_file,
	invalid_argument, invalid_bookid, invalid_page, no_bookmark
};
enum session_status {open, closed};
enum date_filters {day, week, month, year};


struct book {
	int id;
	char* title;
	int min_page;	// The absolute pages
	int max_page;	// in evince including only the core info.
};

struct book_list {
	struct book** items;
	int count;
};


struct project {
	int id;
	char* name;
	char* description;
};

struct project_list {
	struct project** items;
	int count;
};


struct session {
	int type;
	time_t start_time;
	time_t stop_time;
	int id;
	int start_page;
	int stop_page;
};

struct session_list {
	struct session** items;
	int count;
};

struct date {
	int day;
	int month;
	int year;
};


void print_project_list(const struct project_list);
int add_project(struct project_list* pl);
int edit_project(struct project_list* pl);
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
void print_book_list(const struct book_list bl);
int find_book(const char*, const struct book_list bl);
char* gen_bookid(struct book_list bl);


// Determine if a string holds an integer.
bool is_integer(char* string) {
	for (int i = 0; i < (int)strlen(string); i++)
		// if it contains a non-digit then its no integer
		if (!isdigit(string[i]))
			return false;
	return true;
}


// Read list of books from file specified by environment variable $book_logs
// into an array of struct books.
int read_books(struct books* books) {
	struct book* book;
	char buffer[BUFFER_SIZE], string[BUFFER_SIZE];
	char* path;
	FILE* file;

	path = getenv("book_logs");
	if (path == NULL)
		return null_env;

	file = fopen(path, "r");
	if (file == NULL)
		return null_file;

	books->count = 0;
	books->items = NULL;
	while (fgets(buffer, sizeof(buffer), file)) {
		books->count++;
		books->items = realloc(books->items,
			sizeof(struct book*) * (books->count));
		if (books->items == NULL)
			return null_mem;

		book = malloc(sizeof(struct book));
		if (book == NULL)
			return null_mem;

		sscanf(buffer, "%d:%[^:]:%d:%d", &book->id, string,
			&book->min_page, &book->max_page);
		book->title = strdup(string);
		books->items[books->count-1] = book;
	}

	fclose(file);

	return success;
}


// Print list of books.
void print_books(const struct books books) {
	for (int i = 0; i < books.count; i++)
		printf("%02d %s\n", books.items[i]->id, books.items[i]->title);
}
#include "projects.h"


void print_project_list(const struct project_list pl) {
	for (int i = 0; i < pl.count; ++i)
		printf("%2d %s\n", i, pl.items[i]->name);
}

int add_project(struct project_list* pl) {
	char name[15], description[81];
	int i;

	printf("Project name: ");
	scanf("%s", name);
	printf("Description\n: ");
	scanf("%s", description);

	for (i = 0; i < pl->count; ++i)
		if (strcmp(pl->items[i]->name, name) == 0)
			return 1;	// project with this name exista

	++pl->count;

	pl->items = realloc(pl->items, sizeof(struct project*) * pl->count);
	if (pl->items == NULL)
		return 2;

	pl->items[pl->count-1]->name = strdup(name);
	pl->items[pl->count-1]->description = strdup(description);

	return 0;
}


int edit_project(struct project_list* pl) {
	int pn;	// project number
	char name[15], description[81];

	print_project_list(*pl);
	printf("Enter project number: ");
	scanf("%d", &pn);

	if (pn < 0 || pn >= pl->count)
		return 1;

	printf("Old name: %s\n", pl->items[pn]->name);
	printf("New name ('o' to use old): ");
	scanf("%s", name);
	printf("Old description: %s\n", pl->items[pn]->description);
	printf("New description ('o' to use old)\n: ");
	scanf("%s", description);

	if (strcmp(name, "o") != 0 || strcmp(name, "O") != 0)
		pl->items[pn]->name = strdup(name);

	if(strcmp(description, "o") != 0 || strcmp(description, "O") != 0)
		pl->items[pn]->description = strdup(description);

	return 0;
}
int read_sessions(struct sessions* sessions) {
	struct session* session;
	char buffer[BUFFER_SIZE];
	FILE* file;
	char* path;

	path = getenv("booktime_logs");
	if (path == NULL)
		return null_env;

	file = fopen(path, "r");
	if (file == NULL)
		return null_file;

	sessions->count = 0;
	sessions->items = NULL;
	while (fgets(buffer, sizeof(buffer), file)) {
		sessions->count++;
		sessions->items = realloc(sessions->items,
			sizeof(struct session*) * (sessions->count));
		if (sessions->items == NULL)
			return null_mem;

		session = malloc(sizeof(struct session));
		if (session == NULL)
			return null_mem;

		sscanf(buffer, "%ld:%ld:%d:%d:%d",
			&session->start_time, &session->stop_time,
			&session->id, &session->start_page, &session->stop_page);

		sessions->items[sessions->count-1] = session;
	}

	fclose(file);

	return success;
}


int write_sessions(struct sessions sessions) {
	char* path = getenv("booktime_logs");
	if (path == NULL)
		return null_env;

	FILE* file = fopen(path, "w");
	if (file == NULL)
		return null_file;

	for (int i = 0; i < sessions.count; i++)
		fprintf(file, "%ld:%ld:%04d:%04d:%04d\n",
			sessions.items[i]->start_time,
			sessions.items[i]->stop_time,
			sessions.items[i]->id,
			sessions.items[i]->start_page,
			sessions.items[i]->stop_page);
	fclose(file);

	return success;
}

int print_progress(const struct book b, const struct sessions s) {
	int pages_read, total_pages, remaining_pages,
		remaining_days;
	float percentage, rate, remaining_hours, hours_per_day;

	total_pages = b.max_page - b.min_page + 1;
	pages_read = total_pages_read(b, s);
	remaining_pages = total_pages - pages_read;
	percentage = (float)pages_read / total_pages*100;
	rate = average_rate(b, s);
	remaining_hours = remaining_pages / rate;
	hours_per_day  = average_hours_per_day(b, s);
	remaining_days = remaining_hours / hours_per_day;

	if (pages_read == 0)
		return no_logs;

//	print_gaps(b, s);
	printf("%s\n", b.title);
	printf("read %d/%d pages, %5.2f%% \n", pages_read,
		total_pages, percentage);
	printf("average reading rate: %5.2f pages/hour\n", rate);
	printf("average hours per day: %5.2f\n", hours_per_day);
	printf("remaining: %d pages, ~%5.2f hours, ~%d days, ~%d months\n",
		remaining_pages, remaining_hours, remaining_days,
		(int)remaining_days/30);

	return success;
}


int total_pages_read(const struct book b, const struct sessions s) {
	int i, pages_read = 0, pages;
	pages = b.max_page - b.min_page + 1;
	bool profile[pages];
	reading_profile(b, s, profile);

	for (i = 0; i < pages; i++)
		if (profile[i] == true)
			pages_read++;

	return pages_read;
}


float average_rate(const struct book b, const struct sessions s) {
	int i, pages = 0;
	float time = 0;

	pages = total_pages_read(b, s);
	for (i = 0; i < s.count; i++)
		if (session_status(*s.items[i]) == closed && s.items[i]->id == b.id)
			time += s.items[i]->stop_time - s.items[i]->start_time;
	if (time == 0)
		return 0;

	return pages/to_hours(time);
}


float average_hours_per_day(const struct book b, const struct sessions s) {
	int day = 0, days = 0;
	float time = 0;
	struct tm tm;
	for (int i = 0; i < s.count; i++)
		if (session_status(*s.items[i]) == closed && s.items[i]->id == b.id) {
			time += s.items[i]->stop_time - s.items[i]->start_time;

			tm = *localtime(&s.items[i]->start_time);
			if (tm.tm_yday != day)
				days++;
			day = tm.tm_yday;
		}

	return to_hours(time)/days;
}


void reading_profile(const struct book b, const struct sessions s, bool* p) {
	int i, j, index, pages = b.max_page - b.min_page + 1;
	// say that we've not read any page
	for (i = 0; i < pages; i++)
		p[i] = false;

	// the most complex algorithm: find simpler alt.
	// the problem here is space complexity of the array read[].
	// if a book has n pages then n * sizeof(bool) is used.
	// that is n * sizeof(int) which on my system is 4n.
	for (i = 0; i < s.count; i++)
		if (s.items[i]->id == b.id && session_status(*s.items[i]) == closed)
			for (j = s.items[i]->start_page; j <= s.items[i]->stop_page; j++) {
				index = j - b.min_page; // scale for indexing
				if (!p[index])
					p[index] = true;
			}
}


void print_gaps(const struct book b, const struct sessions s) {
	int pages = b.max_page - b.min_page + 1;
	bool profile[pages];
	reading_profile(b, s, profile);

	int p = 0;
	for (int i = 0; i < pages; i++)
		if(profile[i])
			printf("%d-%d\n", p, i + b.min_page -1);
		else if (!p)
			p = i + b.min_page;
}
