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
	lastStatus.packet_rx_drop_count = 0;
    //port = &port_;
	metadata = &metadata_;
}

Autopilot_Interface::~Autopilot_Interface()
{}

void Autopilot_Interface::start()
{
	Serial_Port::start();

	//   CHECK PORT
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
    Autopilot_Interface *autopilot_interface = (Autopilot_Interface *)args;

    autopilot_interface->start_read_thread();

    return NULL;
}

void Autopilot_Interface::start_read_thread()
{
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
    reading_status = true;

    while(!time_to_exit)
  	{
        update_receive();
        usleep(100000);
    }
    reading_status = false;
    return;
}


void Autopilot_Interface::update_receive()
{
    bool success;
  
    // Blocking wait for new data
	while (!time_to_exit )
	{
		// ----------------------------------------------------------------------
		//   READ MESSAGE
		// ----------------------------------------------------------------------
		mavlink_message_t msg;
		success = read_message(msg);
		//printf("read thread\n");
		//printf("succes: %d\n", success);
		// ----------------------------------------------------------------------
		//   HANDLE MESSAGE
		// ----------------------------------------------------------------------
		if( success )
			handle_message(msg);

		// give the write thread time to use the port
		if ( writing_status > false ) {
			usleep(100); // look for components of batches at 10kHz
		}
	
    } // end: while not received all
	return;
}

int Autopilot_Interface::read_message(mavlink_message_t &message)
{
	uint8_t          cp;
	mavlink_status_t status;
	uint8_t          msgReceived = false;

	// --------------------------------------------------------------------------
	//   READ FROM PORT
	// --------------------------------------------------------------------------

	// this function locks the port during read
	int result = _read_port(cp);
	// --------------------------------------------------------------------------
	//   PARSE MESSAGE
	// --------------------------------------------------------------------------
	if (result > 0)
	{
		// the parsing
		msgReceived = mavlink_parse_char(MAVLINK_COMM_1, cp, &message, &status);

		// check for dropped packets
		if ( (lastStatus.packet_rx_drop_count != status.packet_rx_drop_count) && debug )
		{
			printf("ERROR: DROPPED %d PACKETS\n", status.packet_rx_drop_count);
			unsigned char v=cp;
			fprintf(stderr,"%02x ", v);
		}
		lastStatus = status;
	}
	// Couldn't read from port
	else
	{
		fprintf(stderr, "ERROR: Could not read from fd %d\n", fd);
	}

	// --------------------------------------------------------------------------
	//   DEBUGGING REPORTS
	// --------------------------------------------------------------------------
	if(msgReceived && debug)
	{
		// Report info
		printf("Received message from serial with ID #%d (sys:%d|comp:%d):\n", message.msgid, message.sysid, message.compid);

		fprintf(stderr,"Received serial data: ");
		unsigned int i;
		uint8_t buffer[MAVLINK_MAX_PACKET_LEN];

		// check message is write length
		unsigned int messageLength = mavlink_msg_to_send_buffer(buffer, &message);

		// message length error
		if (messageLength > MAVLINK_MAX_PACKET_LEN)
		{
			fprintf(stderr, "\nFATAL ERROR: MESSAGE LENGTH IS LARGER THAN BUFFER SIZE\n");
		}

		// print out the buffer
		else
		{
			for (i=0; i<messageLength; i++)
			{
				unsigned char v=buffer[i];
				fprintf(stderr,"%02x ", v);
			}
			fprintf(stderr,"\n");
		}
	}

	// Done!
	return msgReceived;
}

void Autopilot_Interface::handle_message(const mavlink_message_t &msg)
{
	// Handle Message 
	//printf("message id: %d\n", message.msgid);
	switch (msg.msgid)
	{
		case MAVLINK_MSG_ID_HEARTBEAT:
		{
			mavlink_heartbeat_t packet;
			mavlink_msg_heartbeat_decode(&msg, &packet);
			//store system id, comp id
			break;
		}
		case MAVLINK_MSG_ID_ATTITUDE:
		{
			mavlink_attitude_t packet;
			mavlink_msg_attitude_decode(&msg, &packet);

			break;
		}
		case MAVLINK_MSG_ID_SYSTEM_TIME:
		{
			mavlink_system_time_t packet;
			mavlink_msg_system_time_decode(&msg, &packet);
			metadata->time_unix_usec = packet.time_boot_ms;
			printf("timestamp: %ld\n", metadata->time_unix_usec);
		}
		default:
			break;
	}
}

int Autopilot_Interface::write_message(const mavlink_message_t &message)
{
	char buf[300];

	// Translate message to buffer
	unsigned len = mavlink_msg_to_send_buffer((uint8_t*)buf, &message);

	// Write buffer to serial port, locks port while writing
	int bytesWritten = _write_port(buf,len);

	return bytesWritten;
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

	int len = write_message(message);

	return len;
}