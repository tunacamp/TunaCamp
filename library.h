/*
 * library module
 * Jairo R. K. Kipnusu
 * Thursday August 28, 2025
 */

#ifndef LIBRARY_H
#define LIBRARY_H

#define FIELDS_IN_DOCINFO 9
#define EMPTY -1

// about an author
struct author {
	int id;
	char* first;
	char* middle;
	char* last;
};

// table of authors
struct author_table {
	int count;
	struct author** items;
};

// info about a publisher
struct publisher {
	int id;
	char* name;
};

// table of publishers
struct publisher_table {
	int count;
	struct publisher** items;
};

// info about a subject
struct subject {
	int id;
	char* name;
};

// table of subjects
struct subject_table {
	int count;
	struct subject** items;
};

// document publication info
struct document {
	int id;
	int type;
	char* title;
	int edition;
	struct {
		int count;
		int* items;
	} authors;
	struct {
		int count;
		long* items;
	} isbns;
	int publisher;
	int year;
	struct {
		int count;
		int* items;
	} subjects;
};

// table of document info
struct document_table {
	int count;
	struct document** items;
};


/*
 * prototype for library functions
 * defined in library.c
 */
int load_document_table(FILE*, struct document_table*);
int free_document_table(struct document_table*);
int load_author_table(FILE*, struct author_table*);
int free_author_table(struct author_table*);
int load_subject_table(FILE*, struct subject_table*);
int free_subject_subject(struct subject_table);
int load_publisher_table(FILE*, struct publisher_table*);
int free_publisher_table(struct publisher_table*);
void print_catalogue(struct document_table*);
#endif /* library.h */
