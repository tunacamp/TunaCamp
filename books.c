#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#include "books.h"

char* gen_bookid(struct book_list book_list) {
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
		for (i = 0; i < book_list.count; ++i)
			if (strcmp(bookid, book_list.items[i]->id) != 0)
				found = true;
	}

	return strdup(bookid);
}


int main(void) {
	FILE* book_logs_file = fopen("/home/ric/data/tunacamp/book_logs", "r");
	struct book_list book_list = {NULL, 0};
	read_book_list(book_logs_file, &book_list);

	char* bookid = gen_bookid(book_list);
	printf("bookid: %s\n", bookid);
	return 0;
}
