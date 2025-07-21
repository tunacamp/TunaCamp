#include "sessions.h"

static int bookmark(const char* bookid, const struct session_list sl) {
	int i, last_page = 0;
	for (i = 0; i < sl.count; ++i) {
		if ((sl.items[i]->type == study) && (strcmp(sl.items[i]->id, bookid) == 0)
			&& (session_status(sl.items[i]) == closed))
			last_page = sl.items[i]->stop_page;
	}

	return last_page;
}


void print_reading_gaps(const struct session_list sl, const struct book_list bl,
	const char* bookid) {

	int bn = find_book(bookid, bl);
	int stop = bl.items[bn]->min_page;
	for (int i = 0; i < sl.count; ++i)
		if (strcmp(sl.items[i]->id, bookid) == 0) {
			if (sl.items[i]->start_page != stop) {
				printf("skipped: %d - %d\n", stop, sl.items[i]->start_page);
				stop = sl.items[i]->stop_page;
			}
		}
}


int start_session(struct session_list* sl, const struct book_list bl,
	const struct project_list pl) {
	int sn, bn, pn, lp;
	if ((sn = find_open_session(*sl)) != -1)
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
			print_book_list(bl);

			printf("\nSelect book number(%d-%d): ", 0, bl.count-1);
			scanf("%d", &bn);

			if (bn < 0 || bn > bl.count-1)
				return 3;

			session->id = bl.items[bn]->id;
			print_reading_gaps(*sl, bl, session->id);

			lp = bookmark(session->id, *sl);
			printf("Enter start page(%d-%d)",
				bl.items[bn]->min_page, bl.items[bn]->max_page);
			if (lp)
				printf(". Hit 0 to continue from %d", lp);
			printf(": ");
			scanf("%d", &session->start_page);
			if (session->start_page == 0 && lp)
				session->start_page = lp;

			if (session->start_page < bl.items[bn]->min_page ||
				session->start_page > bl.items[bn]->max_page)
				return 3;

			session->stop_page = 0;	// we don't know the stop page yet
			break;
		case project:
			print_project_list(pl);

			printf("\nEnter project number(%d-%d): ", 0, pl.count-1);
			scanf("%d", &pn);

			if (pn < 0 || pn > pl.count-1)
				return 3;

			session->id = pl.items[pn]->name;
			break;
		default:
			return 4;
	}

	++sl->count;
	sl->items = realloc(sl->items, sizeof(struct session*) * sl->count);
	if (sl->items == NULL)
		return 1;

	sl->items[sl->count-1] = session;
	return 0;
}


int stop_session(struct session_list* sl, struct book_list bl) {
	int sn, bn, sp;
	sn = find_open_session(*sl);
	if (sn == -1)
		return 1;

	if (sl->items[sn]->type == study) {
		bn = find_book(sl->items[sn]->id, bl);

		printf("Enter stop page(%d-%d): ",
			sl->items[sn]->start_page, bl.items[bn]->max_page);
		scanf("%d", &sp);

		if (sp < sl->items[sn]->start_page ||
			sp > bl.items[bn]->max_page)
			return 2;
		sl->items[sn]->stop_page = sp;
	}

	sl->items[sn]->stop_time = time(NULL);
	return 0;
}


int write_session_list(FILE* file, struct session_list sl) {
	file = freopen(NULL, "w", file);
	if (file == NULL)
		return 1;

	for (int i = 0; i < sl.count; ++i) {
		fprintf(file, "%d:%ld:%ld:%s",
			sl.items[i]->type,
			sl.items[i]->start_time,
			sl.items[i]->stop_time,
			sl.items[i]->id);

		if (sl.items[i]->type == study)
			fprintf(file, ":%d:%d",
				sl.items[i]->start_page,
				sl.items[i]->stop_page);
		fprintf(file, "\n");
	}

	return 0;
}


int session_status(const struct session* session) {
	if (session->stop_time == 0)
		return open;
	else
		return closed;
}


int find_open_session(const struct session_list session_list) {
	for (int i = 0; i < session_list.count; ++i)
		if (session_status(session_list.items[i]) == open)
			return i;
	return -1;
}
