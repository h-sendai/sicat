#include "err.h"
#include "event_data.h"
#include <stdio.h>

struct event_data_count event_data_counter[N_PSD_PER_MODULE] = 
{ 
	{0, 0, 0},
	{1, 0, 0},
	{2, 0, 0},
	{3, 0, 0},
	{4, 0, 0},
	{5, 0, 0},
	{6, 0, 0},
	{7, 0, 0}
};

int count_event_data(unsigned char *buf, int byte_len)
{
	int i;
	int psd_no;

	if (byte_len % EVENT_DATA_BYTE_SIZE != 0) {
		err(1, "length of the data buffer is not a multiple of %d", 
			EVENT_DATA_BYTE_SIZE);
	}
	
	for (i = 0; i < byte_len; i = i + EVENT_DATA_BYTE_SIZE) {
		if (buf[i] == 0x5a) {
			psd_no = (buf[i + PSD_NO_OFFSET] &0x7);
			event_data_counter[psd_no].total ++;
		}
	}

	return 0;
}
