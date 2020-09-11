#include <zmq.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <zmq.hpp>

int main(int argc, char *argv[])
{
	zmq::context_t context(1);
	zmq::socket_t  receiver(context, ZMQ_PULL);

	receiver.connect("tcp://localhost:5557");

	zmq::socket_t sender(context, ZMQ_PUSH);
	sender.connect("tcp://localhost:5558");

	while (1) {
		zmq::message_t message;
		int workload;

		receiver.recv(&message);
		std::string smsg(static_cast<char *>(message.data()));
		std::istringstream iss(smsg);
		iss >> workload;
		//do the work
		printf("workdload:%d\n", workload);
		sleep(1);

		message.rebuild();
		sender.send(message);

		std::cout << "." << std::flush;
	}

}
