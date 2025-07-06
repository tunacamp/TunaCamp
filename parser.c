#include "tunacamp.h"

struct record** parserecords(FILE* file, char* delim, int* count) {
	struct record** records;
	struct record* record;
	char buffer[BUFF_SIZE];

	*count = 0;
	records = malloc(sizeof(struct record*));
	if (records == NULL)
		return NULL;

	while (fgets(buffer, sizeof(buffer), file) != NULL) {
			// strip newline
			for (int i = 0; i < strlen(buffer); ++i)
				if (buffer[i] == '\n')
						buffer[i] = '\0';

		record = malloc(sizeof(struct record));
		if (record == NULL)
				return NULL;

		parserecord(buffer, delim, record);

		records[*count] = record;
		*count = *count + 1;

		records = realloc(records, sizeof(char*) * (*count+1));
		if (records == NULL)
			return NULL;
	}

	return records;
}


void parserecord(char* buffer, char* delim, struct record* record) {
		int i = 0;
		union field field;
		char* token = strtok(buffer, delim);
		while (token != NULL) {
				switch (type(token)) {
					case 'i': field.i = atoi(token); break;
					case 's': field.s = strdup(token); break;
					case 'c': field.c = token[0]; break;
					case 't': field.t = atol(token); break;
				}

				switch (i) {
					case 0:	record->f0 = field; break;
					case 1:	record->f1 = field; break;
					case 2: record->f2 = field; break;
					case 3: record->f3 = field; break;
					case 4: record->f4 = field; break;
					case 5:	record->f5 = field; break;
					case 6:	record->f6 = field; break;
					case 7:	record->f7 = field; break;
					case 8:	record->f8 = field; break;
					case 9:	record->f9 = field; break;
					case 10: record->f10 = field; break;
				}

				token = strtok(NULL, delim);
				i++;
		}
}


char type(char* string) {

		int len = strlen(string);

		if (len == 1 && isalpha(string[0]))
				return 'c';

		for (int i = 0; i < len; ++i)
				if (!isdigit(string[i]))
						return 's';

		if (atol(string) <= INT_MAX)
				return 't';

		return 'i';
}

void freerecords(struct record** records, const int count) {
		for (int i = 0; i < count; ++i)
				free(records[i]);
		free(records);
}
