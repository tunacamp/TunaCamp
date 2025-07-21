#include "projects.h"


void print_project_list(const struct project_list pl) {
	for (int i = 0; i < pl.count; ++i)
		printf("%2d %s\n", i, pl.items[i]->name);
}

int add_project(struct project_list* pl) {
	char name[15], description[81];
	int i;

	printf("Project name: ");
	scanf("%s", name);
	printf("Description\n: ");
	scanf("%s", description);

	for (i = 0; i < pl->count; ++i)
		if (strcmp(pl->items[i]->name, name) == 0)
			return 1;	// project with this name exista

	++pl->count;

	pl->items = realloc(pl->items, sizeof(struct project*) * pl->count);
	if (pl->items == NULL)
		return 2;

	pl->items[pl->count-1]->name = strdup(name);
	pl->items[pl->count-1]->description = strdup(description);

	return 0;
}


int edit_project(struct project_list* pl) {
	int pn;	// project number
	char name[15], description[81];

	print_project_list(*pl);
	printf("Enter project number: ");
	scanf("%d", &pn);

	if (pn < 0 || pn >= pl->count)
		return 1;

	printf("Old name: %s\n", pl->items[pn]->name);
	printf("New name ('o' to use old): ");
	scanf("%s", name);
	printf("Old description: %s\n", pl->items[pn]->description);
	printf("New description ('o' to use old)\n: ");
	scanf("%s", description);

	if (strcmp(name, "o") != 0 || strcmp(name, "O") != 0)
		pl->items[pn]->name = strdup(name);

	if(strcmp(description, "o") != 0 || strcmp(description, "O") != 0)
		pl->items[pn]->description = strdup(description);

	return 0;
}
