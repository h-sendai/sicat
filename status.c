#include <stdio.h>
#include "sicat.h"

char *status_message[20] = {
	NULL,
	"START",
	"LEN_REQ_SENT",
	NULL,
	"RECV_DATA",
	NULL,
	NULL,
	NULL,
	"RECV_DATA_FIN",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"END",
	NULL,
	NULL,
	NULL,
};

int print_status(host_info *hp)
{
	fprintf(stderr, "%s status: %s\n",
		hp->ip_address, status_message[hp->status]);
	return 0;
}
	
