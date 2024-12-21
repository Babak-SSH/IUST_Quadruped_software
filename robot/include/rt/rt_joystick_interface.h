/*!
 * @file rt_joystick_interface.h
 * @brief Communication with USB Joystick
 */

#ifndef _rt_joystick_interface
#define _rt_joystick_interface

#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/joystick.h>


class rc_control_settings {
  public:
    double     mode;
    double     p_des[2]; // (x, y) -1 ~ 1
    double     height_variation; // -1 ~ 1
    double     v_des[3]; // -1 ~ 1 * (scale 0.5 ~ 1.5)
    double     rpy_des[3]; // -1 ~ 1
    double     omega_des[3]; // -1 ~ 1
    double     variable[3];
    double     step_height;
};

void get_js_control_settings(void* settings);

int receive_data(int fd);

int init_joystick();

void update_joystick();


#endif
