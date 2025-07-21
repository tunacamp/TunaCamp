#include "books.h"

char* gen_bookid(struct book_list bl) {
	int i, c;
	char bookid[BOOKID_SIZE];
	bool found = false;

	srand((unsigned) time(NULL));

	while (!found) {
		i = 0;
		while (i < BOOKID_SIZE) {
			c = rand() % 123;

			if (islower(c) || isdigit(c))
				bookid[i++] = c;
		}

		bookid[i] = '\0';	// null terminate the string;

		// make sure the id is unique
		for (i = 0; i < bl.count; ++i)
			if (strcmp(bookid, bl.items[i]->id) != 0)
				found = true;
	}

	return strdup(bookid);
}


int find_book(const char* bookid, const struct book_list bl) {
	for (int i = 0; i < bl.count; ++i)
		if (strcmp(bl.items[i]->id, bookid) == 0)
			return i;
	return -1;
}


void print_book_list(const struct book_list bl) {
	for (int i = 0; i < bl.count; ++i)
		printf("%2d %s\n", i, bl.items[i]->title);
}
