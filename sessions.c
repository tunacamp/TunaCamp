#include <stdlib.h>
#include <string.h>
#include "tunacamp.h"


static int start_session(struct session_list* session_list,
	struct book_list book_list, struct project_list project_list) {
	int sn, bn, pn;
	if ((sn = find_open_session(*session_list)) != -1)
		return 1;

	struct session* session = malloc(sizeof(struct session));
	if (session == NULL)
		return 2;

	session->start_time = time(NULL);
	session->stop_time = 0;	// we don't know the stop time yet

	printf("Enter session type(0:study, 1:project): ");
	scanf("%d", &session->type);

	switch (session->type) {
		case study:
			print_book_list(book_list);

			printf("Select book number(%d-%d): ",
				0, book_list.count-1);
			scanf("%d", &bn);

			if (bn < 0 || bn > book_list.count-1)
				return 3;

			session->id = book_list.items[bn]->id;

			printf("Enter start page(%d-%d): ",
				book_list.items[bn]->min_page, book_list.items[bn]->max_page);
			scanf("%d", &session->start_page);

			if (session->start_page < book_list.items[bn]->min_page ||
				session->start_page > book_list.items[bn]->max_page)
				return 3;

			session->stop_page = 0;	// we don't know the stop page yet
			break;
		case project:
			print_project_list(project_list);

			printf("Enter project number(%d-%d): ",
				0, project_list.count-1);
			scanf("%d", &pn);

			if (pn < 0 || pn > project_list.count-1)
				return 3;

			session->id = project_list.items[pn]->name;
			break;
		default:
			return 4;
	}

	++session_list->count;
	session_list->items = realloc(session_list->items,
		sizeof(struct session*) * session_list->count);
	if (session_list->items == NULL)
		return 1;

	session_list->items[session_list->count-1] = session;
	return 0;
}


static int stop_session(struct session_list* session_list,
	struct book_list book_list) {
	int sn, bn, sp;
	sn = find_open_session(*session_list);
	if (sn == -1)
		return 1;

	if (session_list->items[sn]->type == study) {
		bn = find_book(session_list->items[sn]->id, book_list);

		printf("Enter stop page(%d-%d): ",
			session_list->items[sn]->start_page, book_list.items[bn]->max_page);
		scanf("%d", &sp);

		if (sp < session_list->items[sn]->start_page ||
			sp > book_list.items[bn]->max_page)
			return 2;
		session_list->items[sn]->stop_page = sp;
	}

	session_list->items[sn]->stop_time = time(NULL);
	return 0;
}


static int write_session_list(FILE* file, struct session_list session_list) {
	file = freopen(NULL, "w", file);
	if (file == NULL)
		return 1;

	for (int i = 0; i < session_list.count; ++i) {
		fprintf(file, "%d:%ld:%ld:%s",
			session_list.items[i]->type,
			session_list.items[i]->start_time,
			session_list.items[i]->stop_time,
			session_list.items[i]->id);

		if (session_list.items[i]->type == study)
			fprintf(file, ":%d:%d",
				session_list.items[i]->start_page,
				session_list.items[i]->stop_page);
		fprintf(file, "\n");
	}

	return 0;
}


int main(int argc, char** argv) {
	char* session_logs_path = "test/session_logs";
	char* project_logs_path = "test/project_logs";
	char* book_logs_path = "test/book_logs";

	FILE* session_logs_file = fopen(session_logs_path, "r");
	if (session_logs_file == NULL) {
		perror(session_logs_path);
		return 1;
	}

	FILE* project_logs_file = fopen(project_logs_path, "r");
	if (project_logs_file == NULL) {
		perror(project_logs_path);
		return 1;
	}

	FILE* book_logs_file = fopen(book_logs_path, "r");
	if (book_logs_file == NULL) {
		perror(book_logs_path);
		return 1;
	}

	struct session_list session_list = {NULL, 0};
	struct project_list project_list = {NULL, 0};
	struct book_list book_list = {NULL, 0};

   	read_session_list(session_logs_file, &session_list);
	read_project_list(project_logs_file, &project_list);
	read_book_list(book_logs_file, &book_list);

	if (argc != 2) {
		fprintf(stderr, "usage: session [start, stop]\n");
		return 1;
	}

	int return_value, session_number;
	if (strcmp(argv[1], "start") == 0) {
		return_value = start_session(&session_list, book_list, project_list);
		if (return_value == 1) {
			session_number = find_open_session(session_list);
			fprintf(stderr, "another session with id \'%s\' was started\n",
				session_list.items[session_number]->id);
		}
		else if (return_value == 2)
			perror("");
		else if (return_value == 3)
			fprintf(stderr, "number out of range\n");
		else if (return_value == 4)
			fprintf(stderr, "unknown session type\n");
		else
			write_session_list(session_logs_file, session_list);
	}
	else if (strcmp(argv[1], "stop") == 0) {
		return_value = stop_session(&session_list, book_list);
		if (return_value == 1)
			fprintf(stderr, "no session was started\n");
		else if (return_value == 2)
			fprintf(stderr, "number out of range\n");
		else
			write_session_list(session_logs_file, session_list);
	}
	else {
		fprintf(stderr, "unknown argument \'%s\'\n", argv[1]);
		return_value =  1;
	}

	free_session_list(session_list);
	free_book_list(book_list);
	free_project_list(project_list);

	return return_value;
}
