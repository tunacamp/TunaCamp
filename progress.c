#include <math.h>
#include <float.h>
#include "tunacamp.h"

#define SECONDS_IN_HOUR 3600.00
#define to_hours(x)	((x)/SECONDS_IN_HOUR)

enum date_time_filter {day, week, month, year};
int week_number(const struct tm);

void print_summary(const struct session_list sl,
	const int filter, const struct tm tm) {

	long int ttime = 0, rtime = 0, ptime = 0;
	struct tm tm1 = {0};

	for (int i = 0; i < sl.count; ++i) {
		if (session_status(sl.items[i]) == open)
			continue;

		tm1 = *localtime(&sl.items[i]->start_time);

		if ((filter == day && tm.tm_yday == tm1.tm_yday) ||
			(filter == week && week_number(tm) == week_number(tm1))
			|| (filter == month && tm.tm_mon == tm1.tm_mon) ||
			(filter == year && tm.tm_year == tm1.tm_year)) {

			if ( sl.items[i]->type == study)
				rtime += sl.items[i]->stop_time - sl.items[i]->start_time;
			else if (sl.items[i]->type == project)
				ptime += sl.items[i]->stop_time - sl.items[i]->start_time;
		}
	}

	ttime = rtime + ptime;

	printf("\ttotal time:   %5.2f\n", to_hours(ttime));
	printf("\treading time: %5.2f %5.2f%%\n", to_hours(rtime),
		to_hours(rtime)/to_hours(ttime)*100);
	printf("\tproject time: %5.2f %5.2f%%\n", to_hours(ptime),
		to_hours(ptime)/to_hours(ttime)*100);
}


// this is not the algorithm
int week_number(const struct tm tm) {
	if ((tm.tm_yday+1)%7 == 0)
		return (tm.tm_yday+1)/7;
	else
		return (tm.tm_yday+1)/7+1;
}


int last_day(const struct session_list sl) {
	struct tm day = *localtime(&sl.items[sl.count-1]->start_time);
	return day.tm_yday;
}


void print_activity(const struct session_list sl, const struct book_list bl) {
	int i, bn, pages_read, duration;
	float reading_rate = 0.0f;
	struct tm st = {0}, sp = {0}, next = {0};
	char header[30];

	st = *localtime(&sl.items[0]->start_time);
	strftime(header, 30, "%Y %U %B %A, %d", &st);
	printf("\t[%s]\n", header);
	for (i = 0; i < sl.count; ++i) {
		if (session_status(sl.items[i]) == open)
			continue;

		duration = sl.items[i]->stop_time - sl.items[i]->start_time;
		st = *localtime(&sl.items[i]->start_time);
		sp = *localtime(&sl.items[i]->stop_time);

		if (sl.items[i]->type == study)
			printf("study   ");
		else
			printf("project ");

		printf("%02d:%02d:%02d %02d:%02d:%02d %5.2f ",
			st.tm_hour, st.tm_min, st.tm_sec,
			sp.tm_hour, sp.tm_min, sp.tm_sec,
			to_hours(duration));

		if (sl.items[i]->type == study) {
			bn = find_book(sl.items[i]->id, bl);
			pages_read = sl.items[i]->stop_page - sl.items[i]->start_page + 1;
			reading_rate = pages_read / to_hours(duration);

			printf("%-41s %3d %3d %3d %5.2f\n",
				bl.items[bn]->title, sl.items[i]->start_page,
				sl.items[i]->stop_page, pages_read, reading_rate);
		}
		else
			printf("%-41s --- --- --- -----\n", sl.items[i]->id);

		if (st.tm_yday != last_day(sl)) {
			next = *localtime(&sl.items[i+1]->start_time);
			if (st.tm_yday != next.tm_yday) {
				printf("\t[DAY SUMMARY]\n");
				print_summary(sl, day, st);

				if (week_number(st) != week_number(next)) {
					printf("\n\t[WEEK SUMMARY]\n");
					print_summary(sl, week, st);
				}

				if (st.tm_mon != next.tm_mon) {
					printf("\n\t[MONTH SUMMARY]\n");
					print_summary(sl, month, st);
				}

				if (st.tm_year != next.tm_year) { 
					printf("\n\t[YEAR SUMMARY]\n");
					print_summary(sl, year, st);
				}

				strftime(header, 30, "%Y %U %B %A, %d", &next);
				printf("\n\t[%s]\n", header);
			}
		}
		else if (i == sl.count -1) {
			printf("\t[DAY SUMMARY]\n");
			print_summary(sl, day, st);
		}
	}
}


void print_commitment_summary(struct session_list sl, struct book_list bl) {
	int i, j, bn;
	long int table[3][2];
	time_t now = time(NULL);
	struct tm tm = *localtime(&now);
	struct tm tm1 = {0};
	long int duration = 0;

	j = 0;
	for (i = 0; i < bl.count; ++i)
		if (bl.items[i]->commitment == true) {
			printf("%d\n", i);
			table[j++][0] = i;
		}

	for (i = 0; i < sl.count; ++i) {
		tm1 = *localtime(&sl.items[i]->start_time);
		if (session_status(sl.items[i]) == closed && tm1.tm_yday == tm.tm_yday)
			for (j = 0; j < (int)sizeof(table)/sizeof(table[0]); ++j)
				if (sl.items[i]->type == study)
					if ((bn = find_book(sl.items[i]->id, bl)) == table[j][0]) {
						duration = sl.items[i]->stop_time - sl.items[i]->start_time;
						table[j][1] += duration;
					}
	}
	printf("book number: %ld, time spent %ld\n", table[0][0], table[0][1]);
}


struct date {
	int day;
	int month;
	int year;
};


void plot_bar(const char* label, double percent) {
	int term_width = 100;
	int bar_len = (int)ceil((percent / 100.0) * term_width);
	int i;

	printf("%-10s:[", label);
	for (i = 0; i < bar_len; ++i)
		putchar('#');
	for (i = 0; i < term_width - bar_len-13; ++i)
		putchar(' ');
	printf("]\n");
}





int main(int argc, char** argv) {
	char* session_logs_path = "tests/session_logs";
	char* book_logs_path = "tests/book_logs";

	FILE* session_logs_file = fopen(session_logs_path, "r");
	if (session_logs_file == NULL) {
		perror(session_logs_path);
		return 1;
	}

	FILE* book_logs_file = fopen(book_logs_path, "r");
	if (book_logs_file == NULL) {
		perror(book_logs_path);
		return 1;
	}

	struct session_list session_list = {NULL, 0};
	struct book_list book_list = {NULL, 0};

	read_session_list(session_logs_file, &session_list);
	read_book_list(book_logs_file, &book_list);
	print_day_summary(session_list, book_list);

	return 0;
}
