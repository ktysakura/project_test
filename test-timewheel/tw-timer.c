#include <stdio.h>
#include <unistd.h>
#include "timewheel.h"

void hello_world(timer_node_t *node) {
    printf("hello world msec = %u\n", node->expire);
}

// 10ms 100 * 10ms 1000ms = 1s
int main() {
    init_timer();
    add_timer(100, hello_world);

    timer_node_t * node;
    node = add_timer(150, hello_world);
    del_timer(node);
    for (;;) {
        update_timer();
        usleep(2500);
    }
    return 0;
}