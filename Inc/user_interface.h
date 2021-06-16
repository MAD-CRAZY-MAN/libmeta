#ifndef USER_INTERFACE_
#define USER_INTERFACE_

//#include "autopilot_interface.h"
//#include "serial_port.h"


class Xvd_Metadata 
{
    public:
       // Xvd_Metadata(){}
       // ~Xvd_Metadata(){}
        // void xvd_metadata_query_init(); //ttyACM0
        // void xvd_metadata_query_release(); //uart, thread 종료

        // void xvd_metadata_query_timestamp();
        // void xvd_metadata_query_platform_attitude();
        // void xvd_metadata_query_sensor_position();
        // void xvd_metadata_query_sensor_attitude();
        // void xvd_metadata_query_fov();
        uint64_t time_unix_usec;

    private:
        

};


#endif