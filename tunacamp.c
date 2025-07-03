#include "tunacamp.h"

int main(void) {
		FILE* sLogFile;
		char** logs = NULL;
		int logCount;


		sLogFile = fopen("/home/ric/data/tunacamp/sessions", "r");
		if (sLogFile == NULL) {
			perror("");
			return 1;
		}

	loadLogs(sLogFile, &logs, &logCount);
		printf("c %d\n", logCount);
		printf("%s\n", logs[2]);

		return 0;
}
