#include "tunacamp.h"


// etxternal global variables
document** documentList = NULL;
project** projectList = NULL;
session** sessionList = NULL;

int documentCount = 0;
int projectCount = 0;
int sessionCount = 0;

FILE* documentListFile = NULL;
FILE* sessionListFile = NULL;
FILE* projectListFile = NULL;

int main(int argc, char** argv) {
		char* filePaths[] = {
			"test/library",
			"test/sessions",
			"test/jets"
		};


		sessionListFile = fopen(filePaths[1], "r");
		if (sessionListFile == NULL) {
			perror(filePaths[1]);
			return 1;
		}

		projectListFile = fopen(filePaths[2], "r");
		if (projectListFile == NULL) {
			perror(filePaths[2]);
			return 1;
		}

		documentListFile = fopen(filePaths[0], "r");
		if (documentListFile == NULL) {
			perror(filePaths[0]);
			return 1;
		}


		if (parseList(sessionListFile, sessions)) {
			perror("");
			return 1;
		}

		if (parseList(projectListFile, projects)) {
			perror("");
			return 1;
		}

		if (parseList(documentListFile, documents)) {
			perror("");
			return 1;
		}


		if (argc < 2) {
			fprintf(stderr, "Expected at least an argument\n");
			return 1;
		}


		if (strcmp(argv[1], "session") == 0) {
			if (argc < 3) {
				fprintf(stderr, "Expected at least two arguments\n");
				return 1;
			}

			if (strcmp(argv[2], "start") == 0) {
				startSession();
			}
			else if ( strcmp(argv[2], "stop") == 0) {
				stopSession();
			}
			else if (strcmp(argv[2], "pause") == 0) {
				pauseSession();
			}
			else if (strcmp(argv[2], "resuume") == 0) {
				resumeSession();
			}
			else {
				fprintf(stderr, "Unknown argument %s for session\n", argv[2]);
				return 1;
			}
		}
		else {
			fprintf(stderr, "Unknown argument %s\n", argv[1]);
			return 1;
		}

		return 0;
}
