#ifndef _MYHEADER_H
#define _MYHEADER_H 1

#include "sitcp.h"

#include <sys/wait.h>
#include <sys/stat.h>    /* for S_xxx file mode constants */
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>     /* for iovec{} and readv/writev */
#include <sys/time.h>
#include <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include <netinet/tcp.h>
#include <arpa/inet.h>   /* inet(3) functions */
#include <err.h>
#include <errno.h>
#include <fcntl.h>       /* for nonblocking */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#define SA  struct sockaddr

/*
int socket_e(int, int, int);
int connect_e(int, const SA *, socklen_t);
int inet_pton_e(int, const char *, void *);
*/

#endif
