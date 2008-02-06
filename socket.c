#include "myheader.h"

static int float2timeval(float sec, struct timeval *tv)
{
	unsigned int i;
	unsigned long tv_sec;
	unsigned long tv_usec;

	/*
	if (sec > 3600.0) {
		fprintf(stderr, "sleep time too large: %4.3f\n", sec);
		return -1;
	}
	*/

	/* round to mili sec */
	i = sec * 1000;
	tv_sec  = i / 1000;
	tv_usec = (sec - tv_sec) * 1000000;

	tv->tv_sec  = tv_sec;
	tv->tv_usec = tv_usec;

	return 0;
}

/* ************************************************************************
===> connect_udp() <===

DESCRIPTION:
	Connect to SiTCP.

SYNOPSIS:
	#include <SiTCPSocketLib.h>
	int sockfd;
	sockfd = connect_udp(char *ip_address, int port, float timeout);
	
	timeout specifies read timeout in seconds (in float format).

RETURN VALUES:
	Socket descriptor on success.
	-1 on error.  To lookup detail, use errno. 

USAGE:
	char ip_address[] = "130.87.234.35";
	int port = 23;

	if ( (fd = connect_to_sitcp(ip_address, port, 1.5)) < 0) {
		warnx("connect fail to %s port %d", ip_address, port);
	}
************************************************************************ */

int connect_udp(char *ip_address, int port, float timeout)
{
	int fd;
	fd = connect_sitcp(ip_address, port, timeout, SOCK_DGRAM);
	return fd;
}

int connect_tcp(char *ip_address, int port, float timeout)
{
	int fd;

	fd = connect_sitcp(ip_address, port, timeout, SOCK_STREAM);
	return fd;
}

int connect_sitcp(char *ip_address, int port, float timeout, int type)
{
	int sockfd;
	struct sockaddr_in servaddr;
	struct timeval tv;
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	if (port < 0 || port > 65535) {
		/* no errno */
		warnx("port number invalid: %d", port);
		return -1;
	}
	servaddr.sin_port = htons(port);
	if (inet_aton(ip_address, &servaddr.sin_addr) == 0) {
		/* no errno */
		warnx("IP address invalid: %s", ip_address);
		return -1;
	}

	if (timeout < 0) {
		/* no errno */
		warnx("timeout invalid: %f", timeout);
		return -1;
	}
	if (float2timeval(timeout, &tv) < 0) {
		warnx("fail conversion from timeout values to timeval structure");
		return -1;
	}

	if (type == SOCK_DGRAM) {
		if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			return -1;
		}
	}
	else if (type == SOCK_STREAM) {
		if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			return -1;
		}
	}
	else {
		warnx("unknown type: not SOCK_DGRAM.  not SOCK_STREAM");
		return -1;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
		return -1;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
		return -1;
	}

	if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
		return -1;
	}
	
	return sockfd;
}

/* ************************************************************************
===> write_udp() <===

DESCRIPTION:
	Write to SiTCP.

SYNOPSIS:
	#include <SiTCPSocketLib.h>
	write_udp(int sockfd, char *buf, int nbytes);

RETURN VALUES:
	Write to socket file descriptor sockfd.
	Return the number of bytes which were written.
	-1 on error.  To lookup detail, use errno. 

USAGE:
	char buf[] = "hello, world";
	if (write_udp(sockfd, buf, sizeof(buf)) < 0) {
		warn("write error");
	}
************************************************************************ */

int write_udp(int sockfd, char *buf, int nbytes)
{
	int n;
	if ( (n = write(sockfd, buf, nbytes)) < 0) {
		return -1;
	}
	return n;
}

/* ************************************************************************
===> read_udp() <===

DESCRIPTION:
	Read from SiTCP.

SYNOPSIS:
	#include <SiTCPSocketLib.h>
	read_udp(int sockfd, char *buf, int nbytes);

RETURN VALUES:
	Read n bytes from socket file descriptor sockfd into buf.
	Return the number of bytes which were written.
	-1 on error.  To lookup detail, use errno. 

USAGE:
	char buf[] = "hello, world";
	if (read_udp(sockfd, buf, sizeof(buf)) < 0) {
		warn("read error");
	}
************************************************************************ */

int read_udp(int sockfd, char *buf, int nbytes)
{
	int n;
	if ( (n = read(sockfd, buf, nbytes)) < 0) {
		return -1;
	}
	return n;
}

/* ************************************************************************
===> pack() <===

DESCRIPTION:
	Pack bcp header and/or data to packet array.
	This packet array is send to SiTCP as payload.
	We have to pack header structure to avoid aligment problem
	(we should not write structure directly as payload).

SYNOPSIS:
	#include <SiTCPSocketLib.h>
	pack(char *packet, char *header, char *data);

	If we don't have data, write NULL in the last argument.

	Argument order is same as memcpy(3).

RETURN VALUES:
	Result packet length (header length + data length).
	Mininum is HEADER_LEN (defined 8 bytes in sitcp.h as of this writing).

USAGE:
	XXX
************************************************************************ */
int pack(unsigned char *packet, bcp_header *p, unsigned char *data)
{
	int rv;
	int address = htonl(p->address);

	/* See operator(7) manual page on operator precedence */

	bcopy(&p->type,    &packet[0], 1);
	bcopy(&p->command, &packet[1], 1);
	bcopy(&p->id,      &packet[2], 1);
	bcopy(&p->length,  &packet[3], 1);
	bcopy(&address,    &packet[4], 4);
	if (data != NULL) {
		bcopy(data,        &packet[8], p->length);
		rv = HEADER_LEN + p->length;
	}
	else {
		rv = HEADER_LEN;
	}

	return rv;
}

/* ************************************************************************
===> unpack() <===

DESCRIPTION:
	Unpack received UDP packet to bcp header structure and/or data.

SYNOPSIS:
	#include <SiTCPSocketLib.h>
	unpack(char *packet, char *header, char *data);

	If we don't have data, write NULL in the last argument.

RETURN VALUES:
	XXX

USAGE:
	XXX
************************************************************************ */

int unpack(unsigned char *packet, bcp_header *p, unsigned char *data)
{
	//unsigned int address = (
	//	packet[4] << 24 | packet[5] << 16 | packet[6] << 8 | packet[7]);

	unsigned int address;
	bcopy(&packet[4], &address, 4);
	address = ntohl(address);

	p->type    = packet[0];
	p->command = packet[1];
	p->id      = packet[2];
	p->length  = packet[3];
	p->address = address;
	
	if (data != NULL) {
		bcopy(&packet[8], data, p->length);
	}

	return 0;
}

/* ************************************************************************
===> set_data() <===

DESCRIPTION:
	Send a data packet to SiTCP.
	We may use this function to request to get setting data.

SYNOPSIS:
	#include <SiTCPSocketLib.h>
	set_data(sitcp_host_info *host_info,
		bcp_header *send_header, char *send_data,
		bcp_header *recv_header, char *recv_data);
	If we don't have write data, specify NULL at *send_data.

RETURN VALUES:
	XXX

USAGE:
	XXX
************************************************************************ */

int
set_data(sitcp_host_info *host_info, 
	bcp_header *send_header, char *send_data,
	bcp_header *recv_header, char *recv_data)
{
	int            sockfd;
	int            n, packet_len;

	unsigned char  packet[MAX_DATA_LEN + HEADER_LEN];
	unsigned char  recv_buf[MAX_DATA_LEN + HEADER_LEN];

	if ( (sockfd = connect_udp(host_info->ip_address, host_info->port, host_info->timeout)) < 0) {
		return -1;
	}

	packet_len = pack(packet, send_header, send_data);

	/* write to sitcp */
	if (write_udp(sockfd, packet, packet_len) < 0) {
		return -1;
	}

	/* read reply from sitcp */
	if ( (n = read_udp(sockfd, recv_buf, sizeof(recv_buf))) < 0) {
		return -1;
	}

	unpack(recv_buf, recv_header, recv_data);
	
	if (recv_header->length > 0) {
		return recv_header->length;
	}
	else {
		/* something wrong */
		return -1;
	}
}
