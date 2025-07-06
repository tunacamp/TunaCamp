#include "tunacamp.h"


int main(int argc, char** argv) {
		extern struct record** books;
		struct record** sessions;
		extern struct record** jets;
	    struct record** schema;

		extern int booksc, jetsc;
		int sessionsc, schemac;

		FILE* booksf;
		FILE* sessionsf;
		FILE* jetsf;
		FILE* schemaf;

		booksf = fopen("test/books", "r");
		sessionsf = fopen("test/sessions", "r");
		jetsf = fopen("test/jets", "r");
		schemaf = fopen("test/schema", "r");

		books = parserecords(booksf, ":", &booksc);
		sessions = parserecords(sessionsf, ":", &sessionsc);
		jets = parserecords(jetsf, ":", &jetsc);
		schema = parserecords(schemaf, ":", &schemac);

		startsession(sessions, &sessionsc);
		writesessions(sessionsf, sessions, sessionsc);

		freerecords(books, booksc);
		freerecords(sessions, sessionsc);
		freerecords(jets, jetsc);
		freerecords(schema, schemac);

		return 0;
}
