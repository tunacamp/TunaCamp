#ifndef TUNACAMP_H
#define TUNACAMP_H

#include <time.h>
#include <stdio.h>

#define BOOKID_SIZE 13


enum session_type {study, project};
enum session_status {open, closed};
enum book_tag {science, tech_theorectical, tech_practical, life};


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

struct project {
	char* name;
	char* description;
};

struct project_list {
	struct project** items;
	int count;
};


int read_session_list(FILE*, struct session_list*);
void free_session_list(struct session_list);
int find_open_session(struct session_list);
int session_status(struct session*);
int read_book_list(FILE*, struct book_list*);
int find_book(char*, struct book_list);
void print_book_list(struct book_list);
void free_book_list(struct book_list);
int read_project_list(FILE*, struct project_list*);
void print_project_list(struct project_list);
void free_project_list(struct project_list);
#endif /* tunacamp.h */
