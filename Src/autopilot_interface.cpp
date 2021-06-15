#include "../Inc/autopilot_interface.h"
// Struct containing information on the MAV we are currently connected to
uint64_t get_time_usec()
{
	static struct timeval _time_stamp;
	gettimeofday(&_time_stamp, NULL);
	return _time_stamp.tv_sec*1000000 + _time_stamp.tv_usec;
}

Autopilot_Interface::Autopilot_Interface(Serial_Port &port_, Xvd_Metadata &metadata_)
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
	metadata = &metadata_;
}

Autopilot_Interface::~Autopilot_Interface()
{}

void Autopilot_Interface::start()
{
	int result;

	// --------------------------------------------------------------------------
	//   CHECK PORT
	// --------------------------------------------------------------------------
    printf("Autopilot_Interface::start\n");
	if ( !port->is_running() ) // PORT_OPEN
	{
		fprintf(stderr,"ERROR: port not open\n");
        return;
	}
	printf("is_running\n");

    result = pthread_create(&read_tid, NULL, &start_autopilot_interface_read_thread, this);
	printf("%d\n", result);
}

void Autopilot_Interface::stop()
{
    printf("close threads\n");

  //  time_to_exit = true;

    pthread_join(read_tid, NULL);
   // pthread_join(write_tid, NULL);
}

void* start_autopilot_interface_read_thread(void *args)
{
    printf("start_autopilot_interface_read_thread\n");
    Autopilot_Interface *autopilot_interface = (Autopilot_Interface *)args;

    autopilot_interface->start_read_thread();

    return NULL;
}

void Autopilot_Interface::start_read_thread()
{
    printf("Autopilot_Interface::start_read_thread\n");

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
    printf("Autopilot_Interface::read_thread\n");
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
    uint32_t count = 0;
    // Blocking wait for new data
	while (!time_to_exit )
	{
		// ----------------------------------------------------------------------
		//   READ MESSAGE
		// ----------------------------------------------------------------------
		mavlink_message_t message;
		success = port->read_message(message);
		//printf("read thread\n");
		//printf("succes: %d\n", success);
		// ----------------------------------------------------------------------
		//   HANDLE MESSAGE
		// ----------------------------------------------------------------------
		if( success )
		{
			// Store message sysid and compid.
			// Note this doesn't handle multiple message sources.
			current_messages.sysid  = message.sysid;
			current_messages.compid = message.compid;
            // Handle Message 
			//printf("message id: %d\n", message.msgid);
			switch (message.msgid)
			{
				case MAVLINK_MSG_ID_HEARTBEAT:
				{
					mavlink_msg_heartbeat_decode(&message, &(current_messages.heartbeat));
					current_messages.time_stamps.heartbeat = get_time_usec();
					this_timestamps.heartbeat = current_messages.time_stamps.heartbeat;
                    break;
				}
				case MAVLINK_MSG_ID_ATTITUDE:
				{
					mavlink_msg_attitude_decode(&message, &(current_messages.attitude));

					break;
				}
				case MAVLINK_MSG_ID_SYSTEM_TIME:
				{
					mavlink_system_time_t packet;
					mavlink_msg_system_time_decode(&message, &packet);
					metadata->time_unix_usec = packet.time_boot_ms;
					printf("timestamp: %ld\n", metadata->time_unix_usec);
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

int Autopilot_Interface::request_message(uint32_t msg_id, uint32_t interval_us)
{
	mavlink_command_long_t set_msg_interval = {0};
	set_msg_interval.target_system = 1;
	set_msg_interval.target_component = 1;
	set_msg_interval.command = MAV_CMD_SET_MESSAGE_INTERVAL;
	set_msg_interval.confirmation = 0;
	set_msg_interval.param1 = msg_id;
	set_msg_interval.param2 = interval_us;
	set_msg_interval.param7 = 0;

	mavlink_message_t message;
	mavlink_msg_command_long_encode(1, 195, &message, &set_msg_interval);

	int len = port->write_message(message);

	return len;
}