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


uint64_t get_time_usec();
void* start_autopilot_interface_read_thread(void *args);
void* start_autopilot_interface_write_thread(void *args);

class Autopilot_Interface : public Serial_Port
{
    public:
        Autopilot_Interface(Xvd_Metadata &metadata_);
        ~Autopilot_Interface();
        
        char reading_status;
        char writing_status;
        char control_status;
        uint64_t write_count;

        int system_id;
        int autopilot_id;
        int companion_id;

        void read_message();
        int write_message(mavlink_message_t message);

        void start_read_thread();
        void start_write_thread(void);

		int request_message(uint32_t msg_id, uint32_t interval_us);

        void start();
        void stop();

    private:
		Xvd_Metadata *metadata;

        bool time_to_exit;

        pthread_t read_tid;
        pthread_t write_tid;

        void read_thread();
        void write_thread(void);
};


#endif