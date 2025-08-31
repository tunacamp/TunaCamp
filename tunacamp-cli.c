#include "common.h"
#include "sessions.h"
#include "library.h"

int main(void) {
	FILE *file = fopen("test", "r");
	if (file == NULL) {
		perror("test");
		return 1;
	}
	struct publisher_table table = {0, NULL};
	load_publisher_table(file, &table);
//	printf("log count: %d\n", logs.count);
//	for (int i = 0; i < table.count; i++)
//		printf("%s\n", table.items[i]->first);
	free_publisher_table(&table);
	fclose(file);
	return success;
}
