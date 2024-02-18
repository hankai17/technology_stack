/*
 * =====================================================================================
 *
 *       Filename:  showmem.c
 *
 *    Description:  show process pid memory usage.
 *
 *        Version:  1.0
 *        Created:  2020-03-29 10:20:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Xiaoyu Du (souldump), tingsrain@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


size_t zmalloc_get_rss(pid_t pid) {
	int page = sysconf(_SC_PAGESIZE);
	size_t rss;
	char buf[4096];
	char filename[256];
	int fd, count;
	char *p, *x;

	snprintf(filename,256,"/proc/%d/stat", pid);
	if ((fd = open(filename,O_RDONLY)) == -1) return 0;
	if (read(fd,buf,4096) <= 0) {
		close(fd);
		return 0;
	}
	close(fd);

	p = buf;
	count = 23; /* RSS is the 24th field in /proc/<pid>/stat */
	while(p && count--) {
		p = strchr(p,' ');
		if (p) p++;
	}
	if (!p) return 0;
	x = strchr(p,' ');
	if (!x) return 0;
	*x = '\0';

	rss = strtoll(p,NULL,10);
	rss *= page;
	return rss;
}

int main(int argc, char *argv[])
{
	pid_t pid;
	size_t rss;

	if (argc != 2) {
		printf("Usage: %s pid\n", argv[0]);
		return 0;
	}

	rss = zmalloc_get_rss(atoi(argv[1]));
	printf("rss size = %lu bytes =  %2fKB  =  %2fMB, \n", rss, rss/1024.0, rss/(1024*1024.0));

	return 0;
}

