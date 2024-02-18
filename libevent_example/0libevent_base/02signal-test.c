#include <sys/types.h>
#include <event2/event-config.h>
#include <sys/stat.h>
#include <sys/queue.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <event2/event.h>

int called = 0;
static void signal_cb(evutil_socket_t fd, short event, void *arg) {
	struct event* signal = arg;
	printf("signal_cb: got signal %d\n", event_get_signal(signal));
	if (called >= 2)
		event_del(signal); //信号没有未决这一说
	called++;
}

int main(int argc, char **argv)
{
	struct event *signal_int;
	struct event_base* base;

	base = event_base_new();
	signal_int = evsignal_new(base, SIGINT, signal_cb, event_self_cbarg());
	event_add(signal_int, NULL);
	event_base_dispatch(base);
	event_free(signal_int);
	event_base_free(base);
	return (0);
}

