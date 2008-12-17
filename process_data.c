#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include "event_data.h"
#include "sicat.h"

extern int Cflag;
extern int rflag; 
extern int wflag;
extern int sleep_before_read;

int init_host_info_counter(host_info *hp) {
	hp->len_counter  = 0;
	hp->data_counter = 0;
	hp->data_len     = 0;
	
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

	hp->status = RECV_DATA_FIN;
	return 0;
}

int prepare_len_request_array(int event_num)
{
	int word_num = event_num * 4;
	len_request[0] = 0xa3;
	len_request[1] = 0;
	len_request[2] = 0;
	len_request[3] = 0;
	len_request[4] = ((word_num & 0xff000000) >> 24) & 0xff;
	len_request[5] = ((word_num & 0xff0000)   >> 16) & 0xff;
	len_request[6] = ((word_num & 0xff00)     >>  8) & 0xff;
	len_request[7] =   word_num & 0xff;
	
	return 0;
}

int send_len_request_to_all(void)
{
	host_info **ptr;
	for (ptr = host_list; (*ptr) != NULL; ptr++) {
		init_host_info_counter(*ptr);
	}
	for (ptr = host_list; (*ptr) != NULL; ptr++) {
		if (send_len_request(*ptr) < 0) {
			fprintf(stderr, "send_len_request fail on %s\n",
				(*ptr)->ip_address);
			exit(1);
		}
	}
	return 0;
}

int send_len_request(host_info *hp)
{
	init_host_info_counter(hp);
	if (write(hp->sockfd, len_request, sizeof(len_request)) < 0) {
		perror("send_len_request fail");
		exit(1);
	}
	hp->status = LEN_REQ_SENT;
	return 0;
}

int read_data(host_info *hp, unsigned char *ptr, int n)
{
	bcopy(ptr, &(hp->data_buf[hp->data_counter]), n);
	hp->data_counter += n;
	if (hp->data_counter == hp->data_len * 2) {
		hp->status = RECV_DATA_FIN;
		/* write(1, hp->data_buf, hp->data_counter); */
	}
	return 0;
}

int read_len(host_info *hp, int n)
{
	if (n >= LEN_LEN - hp->len_counter) {
		/*
		 * has read all length bytes.
		 * hp->read_buf may contain event data.
		 */
		bcopy(
			hp->read_buf, 
			&(hp->len_buf[hp->len_counter]),
			LEN_LEN - hp->len_counter
		);
		/* we got data length */
		hp->data_len =
			(hp->len_buf[0] << 24) +
			(hp->len_buf[1] << 16) +
			(hp->len_buf[2] <<  8) +
			 hp->len_buf[3];
		//fprintf(stderr, "%s data_len: %u\n", hp->ip_address, hp->data_len);
		if (hp->data_len > 0) {
			hp->status = RECV_DATA;
			read_data(hp, &(hp->read_buf[LEN_LEN - hp->len_counter]), n - LEN_LEN + hp->len_counter); 
		}
		else if (hp->data_len == 0) {
			/* XXX */
			//fprintf(stderr, "length 0 returns (harmless if this is the 1st request)\n");
			hp->status = RECV_DATA_FIN;
		}
	}
	else {
		/*
		 * has not read all length bytes.
		 * All read bytes (in hp->read_buf) are length bytes elements only
		 * (no event data in hp->read_buf).
		 */
		bcopy(hp->read_buf, &(hp->len_buf[hp->len_counter]), n);
		hp->len_counter += n;
	}
	return 0;
}

int process_data(host_info *hp)
{
	int n;

	//fprintf(stderr, "%s READ START on %s\n", gf_time(), hp->ip_address);
	//fprintf(stderr, "%s READ END   on %s ( %d bytes )\n", gf_time(), hp->ip_address, n);

	/*
	 * On Linux, read() returns EINTR even if we set SA_RESTART
	 * when sigaction() if we setsockopt() SO_RCVTIMEO.
	 */

	if (wflag) {
		usleep(sleep_before_read);
	}
	read_again:
	n = read(hp->sockfd, hp->read_buf, sizeof(hp->read_buf));
	if (n < 0) {
		if (errno == EINTR) {
			goto read_again;
		}
		else {
			perror("read error in process_data");
			exit(1);
		}
	}
	if (rflag) {
		fprintf(stderr, "%d bytes read\n", n);
	}

	switch(hp->status) {
		case LEN_REQ_SENT:
			read_len(hp, n);
			break;
		case RECV_DATA:
			read_data(hp, hp->read_buf, n);
			break;
		default:
			break;
	}
	return 0;
}
