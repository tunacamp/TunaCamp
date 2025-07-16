#ifndef TUNACAMP_H
#define TUNACAMP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>


#define BUFF_SIZE 100
#define DOCID_SIZE 13


enum bookPriority {core, essential, important, misc};
enum status {complete, incomplete};
enum sessionType {study, project};


struct session {
	int type;
	time_t startTime;
	time_t stopTime;
	char* id;
	int startPage;
	int stopPage;
};


struct book {
	char* id;
	char* title;
	int minPage;	// The absolute pages
	int maxPage;	// in evince including only the core info.
	int priority;
};


struct project {
	char* name;
	char* description;
};



int parseList(FILE*, const int);
void freeList(const int);
struct session** parseSessions(FILE*, int*);
struct project** parseProjecs(FILE*, int*);
int writeSessions(FILE*, struct session**, const int);


void printActivity(struct session**, const int, struct book**, const int);
void printToday(void);
void printSummary(void);


char* genDocId(void);
void printBooks(struct book**, const int);
int updateLibrary(void);


void printProjects(struct project**, const int);
int addProject(void);
int deleteProject(void);
int editProjectInfo(void);

#endif /* tunacamp.h */
