#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

char * gf_time(void)
{
	struct timeval tv;
	static char str[30];
	char   *ptr;

	if ( (gettimeofday(&tv, NULL)) < 0) {
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}

	ptr = ctime((time_t *)&tv.tv_sec);
	strcpy(str, &ptr[11]);
	/* Fri Sep 13 00:00:00 2007\n\0 */
	/* 0123456789012345678901234 5  */
	snprintf(str + 8, sizeof(str) - 8, ".%06ld", tv.tv_usec);
	return (str);
}

char * gf_time_sec(void)
{
	struct timeval tv;
	static char str[30];
	char   *ptr;

	if ( (gettimeofday(&tv, NULL)) < 0) {
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}

	ptr = ctime((time_t *)&tv.tv_sec);
	strcpy(str, &ptr[11]);
	/* Fri Sep 13 00:00:00 2007\n\0 */
	/* 0123456789012345678901234 5  */
	//snprintf(str + 8, sizeof(str) - 8, ".%06ld", tv.tv_usec);
	str[8] = '\0';
	return (str);
}
