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
#define BOOKID_SIZE 13


union field {
		int i;
		time_t t;
		char* s;
		char c;
};


struct record {
		union field f0;
		union field f1;
		union field f2;
		union field f3;
		union field f4;
		union field f5;
		union field f6;
		union field f7;
		union field f8;
		union field f9;
		union field f10;
};

/*******************************
 * module parser.c  *
 * *****************************/

struct record** parserecords(FILE*, char*, int*);
void freerecords(struct record**, const int);
void parserecord(char*, char*, struct record*);
char type(char*);


/********************************
 * module progress.c            *
 * ******************************/
void printactivity(struct record**, const int);
void printtoday(struct record**, const int);
void printsummary(struct record**, const int);


/*******************************
 * module books.c              *
 * *****************************/
void genbookid(char*, struct record**, const int);
void printbooks(struct record**, const int);
int addbook(struct record**, int*);
int removebook(struct record**, int*);
int modifybook(struct record**, const int);
int updatebooks(void);


/*************************
 * module sessions.c
 * ***********************/
int startsession(struct record**, int*);
int pausesession(struct record**, const int);
int resumesession(struct record**, const int);
int stopsession(struct record**, const int);
int writesessions(FILE*, struct record**, const int);
bool isopen(char, char*, struct record**, const int);


/************************
 * module jets.c
 * *********************/
void printjets(struct record**, const int);
int addjet(struct record**, int*);
int removejet(struct record**, int*);
int modifyjet(struct record**, const int);

#endif /* tunacamp.h */
