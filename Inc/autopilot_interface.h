#ifndef AUTOPILOT_INTERFACE_H_
#define AUTOPILOT_INTERFACE_H_

#include "serial_port.h"
#include "user_interface.h"

#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h> // This uses POSIX Threads
#include <unistd.h>  // UNIX standard function definitions
#include <mutex>

#include <common/mavlink.h>


void* start_autopilot_interface_read_thread(void *args);

struct Platform_Attitude
{
    float roll_rad;
    float pitch_rad;
    float yaw_rad;
    float roll_angle;
    float pitch_angle;
    float yaw_angle;
};
struct Time_Stamps
{
    uint64_t time_unix_usec;
    uint32_t time_boot_ms;
};

class Autopilot_Interface
{
    public:
        Autopilot_Interface(Serial_Port &port);
        ~Autopilot_Interface();

        int system_id;
        int autopilot_id;
        int companion_id;
        
        void update_receive();
        int write_message(const mavlink_message_t &message);

        void start_read_thread();
		int request_message(uint32_t msg_id, uint32_t interval_us);

        void start();
        void stop();
        Platform_Attitude platform_attitude;
        Time_Stamps time_stamps;
    
    private:
        Serial_Port *port;
           
        bool time_to_exit;

        pthread_t read_tid;
        mavlink_status_t lastStatus;
        char reading_status;

        int read_message(mavlink_message_t &message);
        void handle_message(const mavlink_message_t &msg);
        void read_thread();
};


#endif