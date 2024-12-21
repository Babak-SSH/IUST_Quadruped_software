#include <cstdio>
#include <thread>
#include <unistd.h>

#include "rt/rt_joystick_interface.h"


/// TODO: add test cases
int main() {
    int fd = init_joystick();
    auto js_thread = std::thread([&](){
        while(true) {
            int o = receive_data(fd);
            if (o == 0) {
                update_joystick();
            }
        }
    });

    js_thread.join();
    
    return 0;
}