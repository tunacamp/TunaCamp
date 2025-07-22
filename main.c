#include "fileio.h"
#include "sessions.h"
#include "books.h"
#include "projects.h"


int main(int argc, char** argv) {
#ifdef debugging
	char* session_logs_path = "test-data/sessions";
	char* project_logs_path = "test-data/jets";
	char* book_logs_path = "test-data/books";
#else
	char* session_logs_path = getenv("session_logs");
	char* project_logs_path = getenv("project_logs");
	char* book_logs_path = getenv("book_logs");
#endif

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

	struct session_list sl = {NULL, 0};
	struct project_list pl = {NULL, 0};
	struct book_list bl = {NULL, 0};

   	read_session_list(session_logs_file, &sl);
	read_project_list(project_logs_file, &pl);
	read_book_list(book_logs_file, &bl);

	if (argc < 2) {
		fprintf(stderr, "usage [sessionman, projectman, libraryman] [options]\n");
		return 1;
	}

	int return_value, session_number;
	if (strcmp(argv[1], "sessionman") == 0) {
		if (argc < 3) {
			char* usage = "usage: sessionman  [start, stop]\n";
			fprintf(stderr, usage);
			return  1;
		}

		if (argc > 3) {
			fprintf(stderr, "unknown argument \'%s\'\n", argv[3]);
			return 1;
		}

		if (strcmp(argv[2], "start") == 0) {
			return_value = start_session(&sl, bl, pl);
			if (return_value == 1) {
				session_number = find_open_session(sl);
				fprintf(stderr, "another session with id \'%s\' is open\n",
					sl.items[session_number]->id);
			}
			else if (return_value == 2)
				perror("");
			else if (return_value == 3)
				fprintf(stderr, "number out of range\n");
			else if (return_value == 4)
				fprintf(stderr, "unknown session type\n");
			else
				write_session_list(session_logs_file, sl);
		}
		else if (strcmp(argv[2], "stop") == 0) {
			return_value = stop_session(&sl, bl);
			if (return_value == 1)
				fprintf(stderr, "no session was started\n");
			else if (return_value == 2)
				fprintf(stderr, "number out of range\n");
			else
				write_session_list(session_logs_file, sl);
		}
		else {
			fprintf(stderr, "unknown argument \'%s\'\n", argv[2]);
			return_value =  1;
		}
	}
	else {
		fprintf(stderr, "unknown argument \'%s\'\n", argv[1]);
		return_value = 1;
	}

	fclose(session_logs_file);
	fclose(book_logs_file);
	fclose(project_logs_file);

	free_session_list(sl);
	free_book_list(bl);
	free_project_list(pl);

	return return_value;
}
