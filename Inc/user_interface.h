#ifndef USER_INTERFACE_
#define USER_INTERFACE_
#include <stdio.h>
#include <pthread.h>
#include <common/mavlink.h>
#include <cstdlib>
#include <unistd.h>  // UNIX standard function definitions
#include <fcntl.h>   // File control definitions
#include <termios.h> // POSIX terminal control definitions
#include <signal.h>
#include "autopilot_interface.h"
#include "serial_port.h"

#define PI 3.14159265

void xvd_metadata_query_init(); 

void xvd_metadata_query_release(); //uart, thread 종료

struct Time_Stamps xvd_metadata_query_timestamp();
struct Platform_Attitude xvd_metadata_query_platform_attitude();
void xvd_metadata_query_sensor_position();
void xvd_metadata_query_sensor_attitude();
void xvd_metadata_query_fov();

double rad2deg(double radian);
 
#endif