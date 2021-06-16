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
        Serial_Port(const char *uart_name_, int baudrate_);
        ~Serial_Port();

        bool is_running(){
            return is_open;
        }

        void start();
        void stop();

        int  open_port(const char* port);
        bool setup_port(int baud, int data_bits, int stop_bits, bool parity, bool hardware_control);

        void register_port(const char *uart_name, int baudrate_);
        
        int  _read(uint8_t &cp);
        int _write(char *buf, unsigned len);
        int fd;
    private:
        pthread_mutex_t  lock;
        const char *uart_name;
        int baudrate;
        bool is_open;
        
        void init_defaults();

                
};

#endif