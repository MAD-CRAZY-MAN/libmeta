#include "mavlink_control.h"

int main(int argc, char **argv)
{
    Serial_Port port;
    Autopilot_Interface autopilot_interface(port);

    port.start();
    autopilot_interface.start();
   // sleep(1);
    autopilot_interface.stop();
   // port.stop();

    return 0;
}