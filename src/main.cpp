#include "main.h"

int main(int argc, char **argv)
{
    Platform_Attitude platform_attitude;
    Time_Stamps time_stamps;
    xvd_metadata_query_init();

   // while(1)
   // {
        time_stamps = xvd_metadata_query_timestamp();
        platform_attitude = xvd_metadata_query_platform_attitude();

      //  printf("unix_time_stamp: %d\n\n", time_stamps.time_boot_ms);
       // printf("roll: %f\n", platform_attitude.roll_angle);
       // printf("pitch: %f\n", platform_attitude.pitch_angle);
       // printf("yaw: %f\n\n", platform_attitude.yaw_angle);
       // usleep(1);
    //}

    xvd_metadata_query_release();

    return 0;
}