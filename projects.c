/*int addProject(void) {
	char name[15], description[81];
	int i;

	printf("Project name: ");
	scanf("%s", name);
	printf("Description\n: ");
	scanf("%s", description);

	for (i = 0; i < projectCount; ++i)
		if (strcmp(projectList[i]->name, name) == 0)
			return 1;	// project with this name exista

	++projectCount;

	projectList = realloc(projectList, sizeof(struct project*) * projectCount);
	if (projectList == NULL)
		return 2;

	projectList[projectCount-1]->name = strdup(name);
	projectList[projectCount-1]->description = strdup(description);

	return 0;
}*/


/*int editProject(void) {
	int jn;	// jet number
	char name[15], description[81];

	printProjects();
	printf("Enter jet number: ");
	scanf("%d", &jn);

	if (jn < 0 || jn >= projectCount)
		return 1;

	printf("Old name: %s\n", projectList[jn]->name);
	printf("New name ('o' to use old): ");
	scanf("%s", name);
	printf("Old description: %s\n", projectList[jn]->description);
	printf("New description ('o' to use old)\n: ");
	scanf("%s", description);

	if (strcmp(name, "o") != 0 || strcmp(name, "O") != 0)
		projectList[jn]->name = strdup(name);

	if(strcmp(description, "o") != 0 || strcmp(description, "O") != 0)
		projectList[jn]->description = strdup(description);

	return 0;
}*/
