#include "autopilot_interface.h"
// Struct containing information on the MAV we are currently connected to
uint64_t get_time_usec()
{
	static struct timeval _time_stamp;
	gettimeofday(&_time_stamp, NULL);
	return _time_stamp.tv_sec*1000000 + _time_stamp.tv_usec;
}

Autopilot_Interface::Autopilot_Interface(Serial_Port &port_)
{
    // initialize attributes
	write_count = 0;

	reading_status = 0;      // whether the read thread is running
	writing_status = 0;      // whether the write thread is running
	control_status = 0;      // whether the autopilot is in offboard control mode
	time_to_exit   = false;  // flag to signal thread exit

	read_tid  = 0; // read thread id
	write_tid = 0; // write thread id

	system_id    = 0; // system id
	autopilot_id = 0; // autopilot component id
	companion_id = 0; // companion computer component id

	current_messages.sysid  = system_id;
	current_messages.compid = autopilot_id;
    port = &port_;
}

Autopilot_Interface::~Autopilot_Interface()
{}

void Autopilot_Interface::start()
{
	int result;

	// --------------------------------------------------------------------------
	//   CHECK PORT
	// --------------------------------------------------------------------------
    printf("Autopilot_Interface::start");
	if ( !port->is_running() ) // PORT_OPEN
	{
		fprintf(stderr,"ERROR: port not open\n");
        return;
	}

    result = pthread_create(&read_tid, NULL, &start_autopilot_interface_read_thread, this);
}

void Autopilot_Interface::stop()
{
    printf("close threads\n");

    time_to_exit = true;

    pthread_join(read_tid, NULL);
    pthread_join(write_tid, NULL);
}

void* start_autopilot_interface_read_thread(void *args)
{
    printf("start_autopilot_interface_read_thread");
    Autopilot_Interface *autopilot_interface = (Autopilot_Interface *)args;

    autopilot_interface->start_read_thread();

    return NULL;
}

void Autopilot_Interface::start_read_thread()
{
    printf("Autopilot_Interface::start_read_thread");

    if(reading_status != 0)
    {
        fprintf(stderr, "read thread already running\n");
        return;
    }
    else
    {
        read_thread();
        return;
    }
}

void Autopilot_Interface::read_thread()
{
    printf("Autopilot_Interface::read_thread");
    reading_status = true;

    while(!time_to_exit)
    {
        read_message();
        usleep(100000);
    }
    reading_status = false;
    return;
}


void Autopilot_Interface::read_message()
{
    bool success;
    bool received_all = false;
    Time_Stamps this_timestamps;
    printf("Autopilot_Interface::read_message");
    // Blocking wait for new data
	while ( !received_all and !time_to_exit )
	{
		// ----------------------------------------------------------------------
		//   READ MESSAGE
		// ----------------------------------------------------------------------
		mavlink_message_t message;
		success = port->read_message(message);

		// ----------------------------------------------------------------------
		//   HANDLE MESSAGE
		// ----------------------------------------------------------------------
		if( success )
		{

			// Store message sysid and compid.
			// Note this doesn't handle multiple message sources.
			current_messages.sysid  = message.sysid;
			current_messages.compid = message.compid;
            // Handle Message ID
			switch (message.msgid)
			{

				case MAVLINK_MSG_ID_HEARTBEAT:
				{
					//printf("MAVLINK_MSG_ID_HEARTBEAT\n");
					mavlink_msg_heartbeat_decode(&message, &(current_messages.heartbeat));
					current_messages.time_stamps.heartbeat = get_time_usec();
					this_timestamps.heartbeat = current_messages.time_stamps.heartbeat;
                    printf("%d", message.msgid);
					break;
				}
                default:
                    break;
            }
        }
        // Check for receipt of all items
		received_all =
				this_timestamps.heartbeat                  &&
//				this_timestamps.battery_status             &&
//				this_timestamps.radio_status               &&
//				this_timestamps.local_position_ned         &&
//				this_timestamps.global_position_int        &&
//				this_timestamps.position_target_local_ned  &&
//				this_timestamps.position_target_global_int &&
//				this_timestamps.highres_imu                &&
//				this_timestamps.attitude                   &&
				this_timestamps.sys_status
				;

            // give the write thread time to use the port
            if ( writing_status > false ) {
                usleep(100); // look for components of batches at 10kHz
            }

	    } // end: while not received all
	return;
}
