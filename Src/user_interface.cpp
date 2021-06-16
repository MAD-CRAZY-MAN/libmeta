#include "../Inc/user_interface.h"

Serial_Port port;
Autopilot_Interface autopilot_interface(port);

void xvd_metadata_query_init()
{
    autopilot_interface.start();
    autopilot_interface.request_message(30, 33333);
    autopilot_interface.request_message(2, 1000000);
}

void xvd_metadata_query_release()
{
    autopilot_interface.stop();
}

struct Time_Stamps xvd_metadata_query_timestamp()
{
    Time_Stamps time_stamps;
    time_stamps = autopilot_interface.time_stamps;
    return time_stamps;
}

struct Platform_Attitude xvd_metadata_query_platform_attitude()
{
    Platform_Attitude platform_attitude;
    platform_attitude = autopilot_interface.platform_attitude;
    platform_attitude.roll_angle = rad2deg(platform_attitude.roll_rad);
    platform_attitude.pitch_angle = rad2deg(platform_attitude.pitch_rad);
    platform_attitude.yaw_angle = rad2deg(platform_attitude.yaw_rad);
    return platform_attitude;
}

void xvd_metadata_query_sensor_position()
{

}

void xvd_metadata_query_sensor_attitude()
{

}

void xvd_metadata_query_fov()
{
    
}
double rad2deg(double radian)
{
    return radian*180/PI;
}
 