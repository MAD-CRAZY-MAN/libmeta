all: git_submodule mavlink_control

mavlink_control: main.cpp Src/serial_port.cpp Src/autopilot_interface.cpp 
	g++ -g -Wall -I mavlink/include/mavlink/v2.0 main.cpp Src/serial_port.cpp Src/autopilot_interface.cpp  -o mavlink_control -lpthread

git_submodule:
	git submodule update --init --recursive

clean:
	 rm -rf *o mavlink_control
