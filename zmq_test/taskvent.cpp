#include <zmq.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))


int main()
{
	zmq::context_t context(1);
	zmq::socket_t  sender(context, ZMQ_PUSH);
	sender.bind("tcp://*:5557");

	std::cout << "Press Enter when the workers are ready: " << std::endl;
	getchar ();
	std::cout << "Sending tasks to workers...\n" << std::endl;

	zmq::message_t msg(2);
#if 0
	zmq::socket_t sink(context, ZMQ_PUSH);
	sink.connect("tcp://localhost:5558");

	memcpy(msg.data(), "0", 1);
	sink.send(msg);
#endif

	srandom((unsigned)time(NULL));

	int task_nbr;
	int total_msec = 0;
	for (task_nbr = 0; task_nbr < 7; task_nbr++) {
		int workload;
		workload = within(100) + 1;
		total_msec += workload;

		msg.rebuild(10);
		memset(msg.data(), '\0', 10);
		sprintf((char *)msg.data(), "%d", workload); 
		sender.send(msg);
	}
	std::cout << "total expected cost:" << total_msec << " msec" << std::endl;
	//sleep(1);
	return 0;
}

