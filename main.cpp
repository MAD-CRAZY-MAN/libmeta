#include "main.h"

int main(int argc, char **argv)
{
    //Serial_Port port;
    Xvd_Metadata metadata;
    Autopilot_Interface autopilot_interface(metadata);

    autopilot_interface.start();
    autopilot_interface.request_message(30, 33333);
    autopilot_interface.request_message(2, 1000000);
    // while(1)
    // {
    //     printf("timestamp: %ld\n", metadata.time_unix_usec);
    //     sleep(1);
    // }
    autopilot_interface.stop();
   // port.stop();

    return 0;

    // xvd_metadata_query_init();
    // xvd_metadata_query_release();

    // uint64_t unix_time_stamp;

    // while(1)
    // {
    //     unix_time_stamp = xvd_metadata_query_timestamp();
    //     printf("unix_time_stamp: %ld", unix_time_stamp);
    //     sleep(1);
    // }
}