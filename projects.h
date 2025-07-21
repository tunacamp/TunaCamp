#ifndef PROJECTS_H
#define PROJECTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct project {
	char* name;
	char* description;
};

struct project_list {
	struct project** items;
	int count;
};

void print_project_list(const struct project_list);

int add_project(struct project_list* pl);

int edit_project(struct project_list* pl);
#endif /* projects.h */
