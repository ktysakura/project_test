#include <unistd.h>
#include <iostream>
#include <zmq.hpp>
#include <sstream>

int main(int argc, char *argv[])
{
	zmq::context_t context(1);
	zmq::socket_t  requester(context, ZMQ_REQ);

	requester.connect("tcp://localhost:5559");

	const char *channel = argc > 1 ? argv[1] : "";

	for (int request = 0; request < 5; request++) {
		zmq::message_t msg(20);
		std::ostringstream oss;
				
		oss << channel << "	world";
		memcpy(msg.data(), oss.str().data(), oss.str().size());
		requester.send(msg);
		requester.recv(&msg);

		std::cout << "received reponse" << request 
			<< " [" << (char *)msg.data() <<"]"
			<< std::endl;
		sleep(2);
	}
}
