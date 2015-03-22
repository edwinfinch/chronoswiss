#include <pebble.h>
#include "chronoswiss.h"

void init(){
    chro_init();
    window_stack_push(chro_window_get_window(), true);
}

void deinit(){
    chro_deinit();
}

int main() {
    init();
    app_event_loop();
    deinit();
    return 1337;
}
