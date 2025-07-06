#include "tunacamp.h"


void printactivity(struct record** sessions, const int count) {
		int i;

		for (i = 0; i < count; ++i) {
				printf("%c %ld %ld %s", sessions[i]->f0.c, sessions[i]->f1.t,
						sessions[i]->f2.t, sessions[i]->f3.s);
				
				if (sessions[i]->f0.c == 'r')
						printf(" %d %d", sessions[i]->f4.i, sessions[i]->f5.i);

				printf("\n");
		}
}
