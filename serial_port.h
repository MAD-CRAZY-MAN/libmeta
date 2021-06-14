#ifndef SERIAL_PORT_H_
#define SERIAL_PORT_H_

#include <stdio.h>
#include <pthread.h>
#include <common/mavlink.h>
#include <cstdlib>
#include <unistd.h>  // UNIX standard function definitions
#include <fcntl.h>   // File control definitions
#include <termios.h> // POSIX terminal control definitions
#include <signal.h>

class Serial_Port
{
    public:
        Serial_Port();
        ~Serial_Port();
        
        int read_message(mavlink_message_t &message);
        int write_message(mavlink_message_t &message);

        bool is_running(){
            return is_open;
        }
        void start();
        void stop();

    private:
        int fd;
        mavlink_status_t lastStatus;
	    pthread_mutex_t  lock;

        const char *uart_name;
        int baudrate;
        bool is_open;
        bool debug;

        int  _open_port(const char* port);
        bool _setup_port(int baud, int data_bits, int stop_bits, bool parity, bool hardware_control);
        int  _read_port(uint8_t &cp);
        int _write_port(char *buf, unsigned len);
};

#endif