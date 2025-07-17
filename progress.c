#include "progress.h"
#include "sessions.h"
#include "books.h"
#include <math.h>

#define SECONDS_IN_HOUR 3600

struct date {
	int day;
	int month;
	int year;
};


void plot_bar(const char* label, double percent) {
	int bar_width = 80;
	int bar_len = (int)ceil((percent / 100.0) * bar_width);

	printf("%-10s:", label);
	for (int i = 0; i < bar_len; ++i)
		putchar('#');
	putchar('\n');
}

void print_day_summary(struct session_list session_list,
	struct book_list book_list, struct date date) {
	int i;
	float duration, book_time, project_time, total_reading_time,
		total_project_time, total_time;

	duration = book_time = project_time = total_reading_time =
		total_project_time = total_time = 0.0;

	for (i = 0; i < session_list.count; ++i){
		if (session_status(session_list.items[i]) == open)
			continue;

		duration = ((float)session_list.items[i]->stop_time -
			session_list.items[i]->start_time)/SECONDS_IN_HOUR;

		if (session_list.items[i]->type == study)
			total_reading_time += duration;
		else
			total_project_time += duration;

		total_time += duration;
	}

	float reading_perc = 0, project_perc = 0;
	reading_perc = total_reading_time/total_time*100;
	project_perc = total_project_time/total_time*100;

	printf("reading time:\t%.2f\t%.2f %%\n", total_reading_time, reading_perc);
	printf("project time:\t%.2f\t%.2f %%\n", total_project_time, project_perc);
	printf("total_time:\t%.2f\t100.00 %%\n", total_time);

	plot_bar("reading", reading_perc);
	plot_bar("projects", project_perc);
}


int main(int argc, char** argv) {
	char* session_logs_path = "test/session_logs";
	char* book_logs_path = "test/book_logs";
	FILE* session_logs_file = fopen(session_logs_path, "r");
	FILE* book_logs_file = fopen(book_logs_path, "r");

	struct session_list session_list = {NULL, 0};
	struct book_list book_list = {NULL, 0};

	read_session_list(session_logs_file, &session_list);
	read_book_list(book_logs_file, &book_list);

	struct date today = {17, 7, 2025};

	print_day_summary(session_list, book_list, today);

	return 0;
}
