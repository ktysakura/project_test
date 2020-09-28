#include "timewheel.h"

#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#if defined(__APPLE__)
#include <AvailabilityMacros.h>
#include <sys/time.h>
#include <mach/task.h>
#include <mach/mach.h>
#endif

static timer_t * TI = NULL;

timer_node_t *
link_clear(link_list_t *list) {
	timer_node_t * ret = list->head.next;
	list->head.next = 0;
	list->tail = &(list->head);

	return ret;
}

void
link(link_list_t *list,timer_node_t *node) {
	list->tail->next = node;
	list->tail = node;
	node->next=0;
}

void
add_node(timer_t *T, timer_node_t *node) {
	uint32_t time=node->expire;
	uint32_t current_time=T->time;
	uint32_t msec = time - current_time;
	if (msec < TIME_NEAR) { //[0, 0x100)
		link(&T->near[time&TIME_NEAR_MASK],node);
	} else if (msec < (1 << (TIME_NEAR_SHIFT+TIME_LEVEL_SHIFT))) {//[0x100, 0x4000)
		link(&T->t[0][((time>>TIME_NEAR_SHIFT) & TIME_LEVEL_MASK)],node);	
	} else if (msec < (1 << (TIME_NEAR_SHIFT+2*TIME_LEVEL_SHIFT))) {//[0x4000, 0x100000)
		link(&T->t[1][((time>>(TIME_NEAR_SHIFT + TIME_LEVEL_SHIFT)) & TIME_LEVEL_MASK)],node);	
	} else if (msec < (1 << (TIME_NEAR_SHIFT+3*TIME_LEVEL_SHIFT))) {//[0x100000, 0x4000000)
		link(&T->t[2][((time>>(TIME_NEAR_SHIFT + 2*TIME_LEVEL_SHIFT)) & TIME_LEVEL_MASK)],node);	
	} else {//[0x4000000, 0xffffffff]
		link(&T->t[3][((time>>(TIME_NEAR_SHIFT + 3*TIME_LEVEL_SHIFT)) & TIME_LEVEL_MASK)],node);	
	}
}

timer_node_t*
add_timer(int time, handler_pt func) {
	timer_node_t *node = (timer_node_t *)malloc(sizeof(*node));
	// SPIN_LOCK(TI);
		node->expire=time+TI->time;// 每10ms加1 0
        node->callback = func;
		if (time <= 0) {
			node->callback(node);
			free(node);
			return NULL;
		}
		add_node(TI, node);
	// SPIN_UNLOCK(TI);
    return node;
}

void
move_list(timer_t *T, int level, int idx) {
	timer_node_t *current = link_clear(&T->t[level][idx]);
	while (current) {
		timer_node_t *temp=current->next;
		add_node(T,current);
		current=temp;
	}
}

//T->time 指第一级的时针
void
timer_shift(timer_t *T) {
	int mask = TIME_NEAR;
	uint32_t ct = ++T->time;
	if (ct == 0) {
		move_list(T, 3, 0);
	} else {
		uint32_t time = ct >> TIME_NEAR_SHIFT;
		int i=0;
		while ((ct & (mask-1))==0) {
			int idx=time & TIME_LEVEL_MASK;
			if (idx!=0) {
				move_list(T, i, idx);
				break;				
			}
			mask <<= TIME_LEVEL_SHIFT;
			time >>= TIME_LEVEL_SHIFT;
			++i;
		}
	}
}

void
dispatch_list(timer_node_t *current) {
	do {
		timer_node_t * temp = current;
		current=current->next;
        if (temp->cancel == 0)
            temp->callback(temp);
		free(temp);
	} while (current);
}

void
timer_execute(timer_t *T) {
	int idx = T->time & TIME_NEAR_MASK;
	
	while (T->near[idx].head.next) {
		timer_node_t *current = link_clear(&T->near[idx]);
		// SPIN_UNLOCK(T);
		dispatch_list(current);
		// SPIN_LOCK(T);
	}
}

void 
timer_update(timer_t *T) {
	// SPIN_LOCK(T);
	timer_execute(T);
	timer_shift(T);
	timer_execute(T);
	// SPIN_UNLOCK(T);
}

void
del_timer(timer_node_t *node) {
    node->cancel = 1;
}

timer_t *
timer_create_timer() {
	timer_t *r=(timer_t *)malloc(sizeof(timer_t));
	memset(r,0,sizeof(*r));
	int i,j;
	for (i=0;i<TIME_NEAR;i++) {
		link_clear(&r->near[i]);
	}
	for (i=0;i<4;i++) {
		for (j=0;j<TIME_LEVEL;j++) {
			link_clear(&r->t[i][j]);
		}
	}
	// SPIN_INIT(r)
	r->current = 0;
	return r;
}

uint64_t
gettime() {
	uint64_t t;
#if !defined(__APPLE__) || defined(AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER)
	struct timespec ti;
	clock_gettime(CLOCK_MONOTONIC, &ti);
	t = (uint64_t)ti.tv_sec * 100;
	t += ti.tv_nsec / 10000000;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	t = (uint64_t)tv.tv_sec * 100;
	t += tv.tv_usec / 10000;
#endif
	return t;
}

void
update_timer(void) {
	uint64_t cp = gettime();
	if (cp != TI->current_point) {
		uint32_t diff = (uint32_t)(cp - TI->current_point);
		TI->current_point = cp;
		int i;
		for (i=0; i<diff; i++) {
			timer_update(TI);
		}
	}
}

void 
init_timer(void) {
	TI = timer_create_timer();
	TI->current_point = gettime();
}
