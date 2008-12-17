#ifndef _EVENT_DATA_H
#define _EVENT_DATA_H 1

#define PSD_NO_OFFSET        4
#define N_PSD_PER_MODULE     8
#define EVENT_DATA_BYTE_SIZE 8

struct event_data_count {
	int psd_no;
	int prev;
	int total;
};

extern struct event_data_count event_data_counter[N_PSD_PER_MODULE];
extern int count_event_data(unsigned char *buf, int len);

#endif
