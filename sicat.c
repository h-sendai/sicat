#include "sicat.h"
#include "myheader.h"
#include "my_signal.h"

static int usage(void);
static void print_result(void);
static char *progname;

unsigned char len_request[8];

volatile sig_atomic_t total_events     = 0;
volatile sig_atomic_t total_bytes      = 0;
volatile sig_atomic_t has_interrupted  = 0;
int loop_count = 0;
int gflag = 0;

void sig_int(int signo)
{
	fprintf(stderr, "SIGINT\n");

	if (gflag) {
		has_interrupted = 1;
		return;
	}
	else {
		print_result();
		exit(0);
	}
}
		
void sig_quit(int signo)
{
	exit(0);
}

int main(int argc, char *argv[])
{
	int ch;
	int n_request;
	int n_event;
	host_info *hp;
	float sleep_time = 0.0;
	float timeout  = 2.0;
	int eflag = 0;
	int nflag = 0;
	int sflag = 0;
	int qflag = 0;
	int tflag = 0;
	int zflag = 0;
	int Fflag = 0;
	int Iflag = 0;
	int Lflag = 0;
	int Nflag = 0;
	int Qflag = 0;
	int Tflag = 0;
	int zero_count = 0;
	int interleave_count = 0;

	struct linger linger_time;
	linger_time.l_onoff  = 1;
	linger_time.l_linger = 0;

	if ( (progname = strrchr(argv[0], '/')) != 0) {
		progname ++;
	}
	else {
		progname = argv[0];
	}

	/* DEFAULT */
	n_request = 4;
	n_event   = 4096;

	while ((ch = getopt(argc, argv, "e:FghI:Ln:Ns:tT:qQz")) != -1) {
		switch (ch) {
			case 'e':
				eflag = 1;
				n_event = atoi(optarg);
				break;
			case 'g':
				gflag = 1;
				break;
			case 'n':
				nflag = 1;
				n_request = atoi(optarg);
				break;
			case 'q':
				qflag = 1;
				break;
			case 's':
				sflag = 1;
				sleep_time = atof(optarg);
				break;
			case 't':
				tflag = 1;
				break;
			case 'z':
				zflag = 1;
				break;
			case 'F':
				Fflag = 1;
				break;
			case 'I':
				/* -I means: quiet, signal graceful, forever */
				Iflag = 1;
				gflag = 1;
				qflag = 1;
				Fflag = 1;
				interleave_count = atoi(optarg);
				break;
			case 'L':
				Lflag = 1;
				break;
			case 'N':
				Nflag = 1;
				break;
			case 'Q':
				Qflag = 1;
				qflag = 1;
				break;
			case 'T':
				Tflag = 1;
				timeout = atof(optarg);
				if (timeout < 0) {
					fprintf(stderr, "invalid timeout option: %s", optarg);
					exit(1);
				}
				break;
			case 'h':
				usage();
				exit(1);
				break;
			default:
				usage();
				exit(1);
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 1) {
		usage();
		exit(1);
	}
	
	my_signal(SIGINT,  sig_int);
	my_signal(SIGQUIT, sig_quit);

	hp = create_host_info_struct(argv[0]);
	prepare_len_request_array(n_event);

	if ( (hp->sockfd = connect_tcp(hp->ip_address, hp->port, timeout)) < 0) {
		fprintf(stderr, "connection fail: %s\n", hp->ip_address);
		perror("connection fail");
		exit(1);
	}
	if (Lflag) {
		setsockopt(hp->sockfd, SOL_SOCKET, SO_LINGER, &linger_time, sizeof(linger_time));
	}
	
	/* XXX wait for data preparation */
	if (sflag) {
		fprintf(stderr, "sleeping %4.2f sec (by -s option)\n", sleep_time);
		usleep(sleep_time * 1000000);
	}

	for ( ; ; ) {
		if (has_interrupted && gflag) {
			break;
		}
		if (! Fflag) {
			if (loop_count == n_request) {
				break;
			}
		}
		if (Iflag && interleave_count == loop_count) {
			fprintf(stderr, "%s ", gf_time());
			print_result();
			loop_count   = 0;
			total_events = 0;
			total_bytes  = 0;
		}
		loop_count ++;
		send_len_request(hp);
		while (hp->status != RECV_DATA_FIN) {
			process_data(hp);
		}
		total_events += hp->data_len / 4;
		total_bytes  += hp->data_counter;
		if (zflag) {
			if (hp->data_len == 0) {
				zero_count ++;
				if (zero_count > 5) {
					break;
				}
			}
		}
		if (! qflag) {
			if (tflag) {
				fprintf(stderr, "%s ", gf_time());
			}
			fprintf(stderr, "request # %4d ", loop_count);
			fprintf(stderr, "%5d events ( %5d bytes )\n",
				hp->data_len/4, hp->data_counter);
		}
		if (! isatty(STDOUT_FILENO) && ! Nflag) {
			write(STDOUT_FILENO, hp->data_buf, hp->data_counter);
		}
	}

	if (! Qflag) {
		print_result();
	}
	return 0;
}

void print_result(void)
{
	fprintf(stderr, "Request Counts: %d | ",    loop_count);
	fprintf(stderr, "Total Events: %d | ",      total_events);
	fprintf(stderr, "Total Bytes: %d bytes\n",  total_bytes );
	return;
}
	
int usage()
{
	char *help_message;
	help_message = 
"-e event_num:   Number of events for every length requests.\n"
"                Default is 4096.\n"
"-g:             When interrupted, read all remaining data.\n"
"-n request_num: Number of length requests.  You may specify 0 to verify\n"
"                the TCP connection between PC and SiTCP module.\n"
"                Default is 4.\n"
"-s sleep_time:  Sleep sleep_time before sending 1st request.\n"
"                You may use float number (e.g. -s 0.1).  Default is 0.\n"
"-q:             Do not display progress counter.  Does display summary.\n"
"-Q:             Really quiet mode.  Does not display progress, summary.\n"
"-t:             Prepend timestamp.\n" 
"-T timeout:     Specify timeout (sec).  May be float value.\n"
"-z:             Exit after 5 times zero event count\n"
"-F:             Forever.  Ignore event number in -n option.\n"
"-I wait:        Get data forever and print result every wait sec.\n"
"                This flag set -t, -q, and -F implicitly.\n"
"-L:             Do not send FIN but RST when socket closes.\n"
"-N:             Do not output received data even if output is redirected.\n"
"To save data, do not specify -N option and redirect output to file:\n"
"sicat 192.168.0.16 > datafile\n"
;

	fprintf(stderr, "%s [-t] [-z] [-N] [-e event_num] [-s sleep] [-T timeout] [-n n_request | -F] ip_address\n", progname);
	fprintf(stderr, "%s", help_message);
	exit(0);
}
