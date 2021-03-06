#ifndef __monitor_h
#define __monitor_h

#include <sys/param.h>

typedef enum {
	lxc_msg_state,
	lxc_msg_priority,
} lxc_msg_type_t;

struct lxc_msg {
	lxc_msg_type_t type;
	char name[MAXPATHLEN];
	int value;
};

void lxc_monitor_send_state(const char *name, lxc_state_t state);
void lxc_monitor_send_priority(const char *name, int priority);
void lxc_monitor_cleanup(const char *name);

#endif
