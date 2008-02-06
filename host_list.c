#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sicat.h"

//int init_host_info_counter(host_info *hp) {
//	hp->len_counter  = 0;
//	hp->data_counter = 0;
//	hp->data_len     = 0;
	
	//bzero(hp->len_buf,  sizeof(hp->len_buf));
	//bzero(hp->read_buf, sizeof(hp->read_buf));
	//bzero(hp->data_buf, sizeof(hp->data_buf));

	/*
	 * to get better performance, we will not clear read_buf, data_buf
	 * each time.  We use hp->data_len not to overread after all
	 * data reading completed.
	 */
	//memset(hp->len_buf,  0, sizeof(hp->len_buf));
	//memset(hp->read_buf, 0, sizeof(hp->read_buf));
	//memset(hp->data_buf, 0, sizeof(hp->data_buf));

//	hp->status = RECV_DATA_FIN;
//	return 0;
//}

host_info *create_host_info_struct(char *ip_address)
{
	host_info *hp;
	hp = malloc(sizeof(host_info));
	strcpy(hp->ip_address, ip_address);
	hp->port = 23;
	hp->status = START;
	hp->request_counter = 0;

	return hp;
}

int prepare_host_list(int n_hosts)
{
	int i;
	host_info *ptr;

	for (i = 0; i < n_hosts; i ++) {
		ptr = malloc(sizeof(host_info));
		snprintf(ptr->ip_address, sizeof(ptr->ip_address),
			"192.168.0.%d", i + 16);
		ptr->port          = 23;
		//init_host_info_counter(ptr);
		//ptr->len_counter   = 0;
		//ptr->event_counter = 0;
		//ptr->status        = START;

		host_list[i] = ptr;
	}
	host_list[i] = NULL;
	return 0;
}

/* 
 * get_reverse_host_list(): create reverse host list
 * 1. count number of hosts
 * 2. create reverse pointer array
 */

int get_reverse_host_list(host_info **host_list_rev)
{
	int i, j;
	host_info **ptr;
	i = 0;
	for (ptr = host_list; (*ptr) != NULL; ptr ++) {
		i++;
	}
	for (j = 0; j < i; j ++) {
		host_list_rev[j] = host_list[ (i - 1) - j];
	}
	
	return 0;
}

int print_ip_address(host_info *hp)
{
	fprintf(stderr, "%s\n", hp->ip_address);
	return 0;
}
