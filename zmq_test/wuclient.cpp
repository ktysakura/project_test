#include <zmq.hpp>
#include <zmq.h>
#include <iostream>
#include <sstream>

int main(int argc, char *argv[])
{
	zmq::context_t context(1);
	zmq::socket_t subscriber(context, ZMQ_SUB);
	
	const char *filter = (argc > 1) ? argv[1] : "101 ";
	
	subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
	subscriber.connect("tcp://localhost:5556");

	int update_nbr;
	int total_temp = 0;

	for (update_nbr = 0; update_nbr != 10; update_nbr++) {
		zmq::message_t update_msg;

		int zipcode, temperature, relhumiddity;

		subscriber.recv(&update_msg);

		std::istringstream iss(static_cast<char *>(update_msg.data()));
		iss >> zipcode >> temperature >> relhumiddity;
		total_temp += temperature;
		printf("recv:%d\n", zipcode); 
	}

	std::cout << "average temperature for zipcode '" <<
		filter << "' was" << total_temp / update_nbr << "F"<< std::endl;  

}

