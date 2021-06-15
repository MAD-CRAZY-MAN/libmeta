#include "../Inc/mavlink_control.h"

int main(int argc, char **argv)
{
    Serial_Port port;
    Xvd_Metadata metadata;
    Autopilot_Interface autopilot_interface(port, metadata);

    port.start();
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
}