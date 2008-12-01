#include <sys/socket.h>

#ifndef _SITCP_H
#define _SITCP_H 1

#define HEADER_LEN 8
/* XXX */
#define MAX_DATA_LEN (4096 - 8)
#define ACK_FLAG 0x08

typedef struct sitcp_host_info {
	char *ip_address;
	int   port;
	float timeout;
} sitcp_host_info;

typedef struct bcp_header {
	unsigned char type;
	unsigned char command;
	unsigned char id;
	unsigned char length;
	unsigned int  address;
} bcp_header;

extern int connect_udp(char *ip_address, int port, float timeout);
extern int connect_tcp(char *ip_address, int port, float timeout);
extern int connect_sitcp(char *ip_address, int port, float timeout, int type);
extern int write_udp(int sockfd, unsigned char *buf, int nbytes);
extern int read_udp(int sockfd, unsigned char *buf, int nbytes);
extern int pack(unsigned char *packet, bcp_header *p, unsigned char *data);
extern int unpack(unsigned char *packet, bcp_header *p, unsigned char *data);
extern int set_data(sitcp_host_info *host_info, 
    bcp_header *send_header, unsigned char *send_data,
	bcp_header *recv_header, unsigned char *recv_data);

#endif /* _SITCP_H */
