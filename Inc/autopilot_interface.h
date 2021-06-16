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
void* start_autopilot_interface_write_thread(void *args);

class Autopilot_Interface
{
    public:
        Autopilot_Interface(Serial_Port &port);
        ~Autopilot_Interface();
        
        char reading_status;
        char writing_status;
        char control_status;
        uint64_t write_count;
        uint64_t time_unix_usec;
        int system_id;
        int autopilot_id;
        int companion_id;

        void update_receive();
        int write_message(const mavlink_message_t &message);

        void start_read_thread();
        void start_write_thread(void);

		int request_message(uint32_t msg_id, uint32_t interval_us);

        void start();
        void stop();

    private:
        Serial_Port *port;
           
        bool time_to_exit;

        pthread_t read_tid;
        pthread_t write_tid;

        mavlink_status_t lastStatus;
        
        int read_message(mavlink_message_t &message);
        void handle_message(const mavlink_message_t &msg);
        void read_thread();
        void write_thread(void);
};


#endif