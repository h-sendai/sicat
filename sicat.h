#ifndef _SELECT_TEST_H
#define _SELECT_TEST_H 1

#define SICAT_VERSION "1.2 (2008-12-17)"

/*
buufer too large.  consume much time to memset(0).
#define READ_BUF_SIZE	1 * 1024 * 1024
#define DATA_BUF_SIZE	1 * 1024 * 1024
*/
#define READ_BUF_SIZE	128 * 1024
#define DATA_BUF_SIZE	128 * 1024
#define LEN_BUF_SIZE    8

#define START			1
#define LEN_REQ_SENT	2
#define RECV_DATA		4
#define RECV_DATA_FIN	8
#define END				16

#define LEN_LEN			4

typedef struct {
	char ip_address[16];
	int  port;
	int  sockfd;
	int  status;
	int  len_counter;
	int  data_counter;
	int  data_len;
	int  request_counter;
	unsigned char len_buf[LEN_BUF_SIZE];
	unsigned char read_buf[READ_BUF_SIZE];
	unsigned char data_buf[DATA_BUF_SIZE];
} host_info;

extern host_info *host_list[1024];
extern host_info *reverse_host_list[1024];

extern host_info *create_host_info_struct(char *);
extern int prepare_host_list(int);
extern int prepare_len_request_array(int);
extern int get_reverse_host_list(host_info **);
extern int print_ip_address(host_info *);
extern int process_data(host_info *);
extern int send_len_request(host_info *);
extern int send_len_request_to_all(void);
extern int init_host_info_counter(host_info *);
extern int print_status(host_info *);
extern char *gf_time(void);
extern char *gf_time_sec(void);
extern int print_sockopt(int);
extern int print_tcp_moderate_rcvbuf(void);

//extern int eflag, hflag, nflag, sflag, qflag;
//extern char *progname;

extern unsigned char len_request[8];
/* = { 0xa3, 0, 0, 0, 0, 0, 0, 0 }; */

#endif
