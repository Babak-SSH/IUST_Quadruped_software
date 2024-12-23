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


void get_js_control_settings(void* settings);

int receive_data(int fd);

int init_joystick();

void update_joystick();


#endif
