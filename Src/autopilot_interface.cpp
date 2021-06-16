#include "../Inc/autopilot_interface.h"

Autopilot_Interface::Autopilot_Interface(Xvd_Metadata &metadata_)
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

    //port = &port_;
	metadata = &metadata_;
}

Autopilot_Interface::~Autopilot_Interface()
{}

void Autopilot_Interface::start()
{
	Serial_Port::start();

	// --------------------------------------------------------------------------
	//   CHECK PORT
	// --------------------------------------------------------------------------
    printf("Autopilot_Interface::start\n");
	if ( !Serial_Port::is_running() ) // PORT_OPEN
	{
		fprintf(stderr,"ERROR: port not open\n");
        return;
	}
	printf("is_running\n");

    pthread_create(&read_tid, NULL, &start_autopilot_interface_read_thread, this);
}

void Autopilot_Interface::stop()
{
    printf("close threads\n");

  //  time_to_exit = true;

    pthread_join(read_tid, NULL);
   // pthread_join(write_tid, NULL);
}

void *start_autopilot_interface_read_thread(void *args)
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
  
    // Blocking wait for new data
	while (!time_to_exit )
	{
		// ----------------------------------------------------------------------
		//   READ MESSAGE
		// ----------------------------------------------------------------------
		mavlink_message_t message;
		success = Serial_Port::read_message(message);
		//printf("read thread\n");
		//printf("succes: %d\n", success);
		// ----------------------------------------------------------------------
		//   HANDLE MESSAGE
		// ----------------------------------------------------------------------
		if( success )
		{
			// Handle Message 
			//printf("message id: %d\n", message.msgid);
			switch (message.msgid)
			{
				case MAVLINK_MSG_ID_HEARTBEAT:
				{
					mavlink_heartbeat_t packet;
					mavlink_msg_heartbeat_decode(&message, &packet);
					//store system id, comp id
                    break;
				}
				case MAVLINK_MSG_ID_ATTITUDE:
				{
					mavlink_attitude_t packet;
					mavlink_msg_attitude_decode(&message, &packet);

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

	int len = Serial_Port::write_message(message);

	return len;
}