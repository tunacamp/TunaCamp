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


enum recordType {documents, sessions, projects};
enum docType {book, documentation, article, periodical};
enum docPriority {core, essential, important, misc};
enum status {complete, incomplete};
enum sessionType {study, programming, art};


typedef struct {
	int type;
	time_t startTime;
	time_t stopTime;
	char* id;
	int startPage;
	int stopPage;
} session;


typedef struct {
	int type;
	char* id;
	char* title;
	int startPage;	// The absolute pages
	int stopPage;	// in evince including only the core info.
	int priority;
	int status;
} document;


typedef struct {
	enum status status;
	char* name;
	char* description;
} project;


extern session** sessionList;
extern document** documentList;
extern project** projectList;

extern int sessionCount;
extern int documentCount;
extern int projectCount;

extern FILE* documentListFile;
extern FILE* sessionListFile;
extern FILE* projectListFile;



/*********************************************
 * module parser.c:                          *
 * Functions for reading and writing records *
 * *******************************************/

/* struct record** parserecords(FILE*/
int parseList(FILE*, const int);
void freeList(const int);
int writeList(const int);


/********************************
 * module progress.c            *
 * ******************************/
void printActivity(void);
void printToday(void);
void printSummary(void);


/*******************************
 * module library.c              *
 * *****************************/
char* genDocId(void);
void printLibrary(void);
int updateLibrary(void);


/*************************
 * module sessions.c
 * ***********************/
int startSession(void);
int pauseSession(void);
void resumeSession(void);
void stopSession(void);

/************************
 * module projects.c
 * *********************/
void printProjects(void);
int addProject(void);
int deleteProject(void);
int editProjectInfo(void);

#endif /* tunacamp.h */
