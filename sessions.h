#ifndef SESSIONS_H
#define SESSIONS_H


// all times use the unix epoch
// a study log record
struct study_log {
	int book_id;
	time_t start_time;
	time_t stop_time;
	int start_page;
	int stop_page;
};

// a project log record
struct jet_log {
	int jet_id;
	time_t start_time;
	time_t stop_time;
};

// list of study logs
struct study_log_list {
	int count;
	struct study_log **items;
};

// list of project logs
struct jet_log_list {
	int count;
	struct jet_log **items;
};

// hold together all the logs
struct logs {
	struct study_log_list *study_logs;
	struct jet_log_list *jet_logs;
};


// prototypes for functions defined in sessions.c
int get_study_logs(FILE*, struct study_log_list*);
int write_study_logs(FILE*, struct study_log_list);
int free_study_logs(struct study_log_list*);
int get_jet_logs(FILE*, struct jet_log_list*);
int write_jet_logs(FILE*, struct jet_log_list);
int free_jet_logs(struct jet_log_list*);

#endif /* sessions.h */
