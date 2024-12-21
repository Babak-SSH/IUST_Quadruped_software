#include <errno.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <string>

#include "rt/rt_joystick_interface.h"

pthread_mutex_t joy_data_m;

struct js_event event;

#define J_BUS_PORT "/dev/input/js0"


// Controller Settings
rc_control_settings rc_control;


// Controller Settings
void get_js_control_settings(void *settings) {
  v_memcpy(settings, &rc_control, sizeof(rc_control_settings));
}

/*!
 * Receive data packets
 */
int receive_data(int fd) {
	ssize_t bytes;
    struct js_event event_;

	bytes = read(fd, &event_, sizeof(js_event));

	if (bytes == sizeof(js_event)) {
        pthread_mutex_lock(&joy_data_m);
        event = event_;
        pthread_mutex_unlock(&joy_data_m);
		return 0;
    }
	
	printf("Error, could not read full event.");
    return -1;
}

/*!
 * Initialize joystick port
 */
int init_joystick() {
	if (pthread_mutex_init(&joy_data_m, NULL) != 0) {
    	printf("Failed to initialize sbus data mutex.\n");
  	}

	int fd = open(J_BUS_PORT, O_RDONLY | O_NOCTTY | O_SYNC);
	if (fd < 0) {
    	printf("Error opening %s: %s\n", J_BUS_PORT, strerror(errno));
    }
	return fd;
}

/**
 * Current state of an axis.
 */
struct axis_state {
    short x, y;
};

/**
 * Keeps track of the current axis state.
 *
 * NOTE: This function assumes that axes are numbered starting from 0, and that
 * the X axis is an even number, and the Y axis is an odd number. However, this
 * is usually a safe assumption.
 *
 * Returns the axis that the event indicated.
 */
size_t get_axis_state(struct js_event *event, struct axis_state axes[3])
{
    size_t axis = event->number / 2;

    if (axis < 3)
    {
        if (event->number % 2 == 0)
            axes[axis].x = event->value;
        else
            axes[axis].y = event->value;
    }

    return axis;
}


void update_joystick(Taranis_X7_data* data) {
    struct axis_state axes[3] = {0};
    size_t axis;
	pthread_mutex_lock(&joy_data_m);
	pthread_mutex_unlock(&joy_data_m);
}
