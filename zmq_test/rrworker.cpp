#include <time.h>
#include <unistd.h>
#include <zmq.hpp>
#include <iostream>
#include <sstream>

using namespace std;
int main()
{
	zmq::context_t context(1);
	zmq::socket_t  socket(context, ZMQ_REP);

	socket.connect("tcp://localhost:5560");
	int i = 0;

	while (1) {
		zmq::message_t msg;

		socket.recv(&msg);
		std::cout << (char *) msg.data() 
				  << std::flush;
		std::cout << std::endl;
		i++;
		msg.rebuild(20);
		
		ostringstream oss;
		oss << "world" << i;
		memcpy(msg.data(), oss.str().data(), oss.str().size() + 1);
		socket.send(msg);

	}
	return 0;
}
