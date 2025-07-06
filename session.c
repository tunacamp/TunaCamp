#include "tunacamp.h"


extern struct record** books;
extern int booksc;
extern struct record** jets;
extern int jetsc;

int startsession(struct record** sessions, int* count) {
		struct record* session;
		time_t now;
		int booknumber, jetnumber;

		session = malloc(sizeof(struct record));
		if (session == NULL)
			return 1;

		now = time(NULL);
		session->f1.t = now;
		session->f2.t = 0;	// we don't know the stop time yet

		printf("Enter session type: ");
		scanf("%c", &session->f0.c);

		switch (session->f0.c) {
			case 'r':
				printbooks(books, booksc);
				printf("Select book number: ");
				scanf("%d", &booknumber);

				if (booknumber >= 0 && booknumber < booksc) {
					// make sure we're not duplicating
					if (isopen('r', books[booknumber]->f0.s, sessions, *count)) {
							fprintf(stderr, "%s: session already open\n",
								books[booknumber]->f0.s);
							return 2;
					}
					session->f3.s = books[booknumber]->f0.s;
				}
				else {
					fprintf(stderr, "Unknown book number: %d\n", booknumber);
					return 2;
				}

				printf("Enter start page: ");
				scanf("%d", &session->f4.i);	//TODO: add page range check

				session->f5.i = 0;	// we don't know the stop page yet
				break;
			case 'p':
				printjets(jets, jetsc);
				printf("Enter project number: ");
				scanf("%d", &jetnumber);

				if (jetnumber >= 0 && jetnumber < jetsc) {
					if (isopen('p', jets[jetnumber]->f0.s, sessions, *count)) {
						fprintf(stderr, "%s: session already open\n",
							jets[jetnumber]->f0.s);

						return 2;
					}
					
					session->f3.s = jets[jetnumber]->f0.s;
				}
				else {
					fprintf(stderr, "Unknown jet number: %d\n", jetnumber);
					return 2;
				}

				// these field are never unused for project session
				// initialize them to 0
				session->f4.i = 0;
				session->f5.i = 0;
				break;
			default:
				fprintf(stderr, "Unknown session type: %c\n", session->f0.c);
				return 2;
		}

		// these fields are never used in a session record
		// initialize them to 0
		session->f6.i = 0;
		session->f7.i = 0;
		session->f8.i = 0;
		session->f9.i = 0;
		session->f10.i = 0;

		*count = *count + 1;

		sessions = realloc(sessions, sizeof(struct record*) * (*count));
		if (sessions == NULL)
				return 1;

		sessions[*count-1] = session;

		return 0;
}


int pausesession(struct record** sessions, const int count) {
		return 0;
}


int resumesession(struct record**sessions, const int count) {
		return 0;
}


int stopsession(struct record** sessions, const int count) {
//		int
		return 0;
}


int writesessions(FILE* file, struct record** sessions, const int count) {
		int i;

		if (freopen(NULL, "w", file) == NULL)
				return 1;

		for (i = 0; i < count; ++i) {
			fprintf(file, "%c:%ld:%ld:%s",
				sessions[i]->f0.c,
				sessions[i]->f1.t,
				sessions[i]->f2.t,
				sessions[i]->f3.s);

			// reading sessions have more fields, remember?
			if (sessions[i]->f0.c == 'r')
						fprintf(file, ":%d:%d",
							sessions[i]->f4.i,
							sessions[i]->f5.i);

			fprintf(file, "\n");
		}

		return 0;
}


bool isopen(char type, char* id, struct record** sessions, const int count) {
		int i;

		for (i = 0; i < count; ++i) {
			if (sessions[i]->f0.c == type && strcmp(sessions[i]->f3.s, id) == 0) {
				if (sessions[i]->f1.t != 0 && sessions[i]->f2.t == 0)
					return true;
			}
		}

		return false;
}
