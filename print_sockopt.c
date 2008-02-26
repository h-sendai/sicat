#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

struct sockopt_nameval {
	char *name;
	int   val;
} sockopt[] = {
	{ "SO_DEBUG",     SO_DEBUG      },
	{ "SO_REUSEADDR", SO_REUSEADDR  },
#ifndef __linux__
	{ "SO_REUSEPORT", SO_REUSEPORT  },
#endif
	{ "SO_KEEPALIVE", SO_KEEPALIVE  },
	{ "SO_DONTROUTE", SO_DONTROUTE  },
	{ "SO_LINGER",    SO_LINGER     },
	{ "SO_BROADCAST", SO_BROADCAST  },
	{ "SO_OOBINLINE", SO_OOBINLINE  },
	{ "SO_SNDLOWAT",  SO_SNDLOWAT   },
	{ "SO_RCVLOWAT",  SO_RCVLOWAT   },
	{ "SO_SNDBUF",    SO_SNDBUF     },
	{ "SO_RCVBUF",    SO_RCVBUF     },
	{ "SO_SNDTIMEO",  SO_SNDTIMEO   },
	{ "SO_RCVTIMEO",  SO_RCVTIMEO   },
	{ NULL,           -1            },
};


int print_tcp_moderate_rcvbuf(void)
{
	FILE *fp;
	int  c;

	if ( (fp = fopen("/proc/sys/net/ipv4/tcp_moderate_rcvbuf", "r")) == NULL) {
		fprintf(stderr, "cannot open /proc/sys/net/ipv4/tcp_moderate_rcvbuf\n");
		return -1;
	}
	fprintf(stderr, "TCP_MODERATE_RCVBUF ");
	while ( (c = fgetc(fp)) != EOF) {
		fprintf(stderr, "%c", c);
	}
	return 0;
}

int print_sockopt(int sockfd)
{
	int optval;
	socklen_t len = sizeof(optval);
	struct sockopt_nameval *ptr;

	for (ptr = sockopt; ptr->name != NULL; ptr++) {
		if (getsockopt(sockfd, SOL_SOCKET, ptr->val, &optval, &len) < 0) {
			perror("getsockopt");
			exit(1);
		}
		fprintf(stderr, "%-12s %8d\n", ptr->name, optval);
	}
	return 0;
}
