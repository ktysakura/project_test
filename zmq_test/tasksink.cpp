#include <zmq.hpp>
#include <time.h>
#include <sys/time.h>
#include <iostream>

int main(int argc, char *argv[])
{

	zmq::context_t context(1);
	zmq::socket_t  receiver(context, ZMQ_PULL);

	receiver.bind("tcp://*:5558");
	zmq::message_t msg;
//	receiver.recv(&msg);

	struct timeval tv_start;
	gettimeofday(&tv_start, NULL);

	int task_nbr;
	int total_msec = 0;

	//for (task_nbr = 0;  task_nbr < 7; task_nbr++) {
	while (true) {
		receiver.recv(&msg);
		if (task_nbr % 10 == 0) {
			std::cout << ":" << std::flush;
		} else {
			std::cout << ":" << std::flush;
		}
	}
	//}

	struct timeval tv_end, tv_diff;
	gettimeofday(&tv_end, NULL);
	tv_diff.tv_sec = tv_end.tv_sec - tv_start.tv_sec;
	tv_diff.tv_usec = tv_end.tv_usec - tv_start.tv_usec;

	total_msec = tv_diff.tv_sec * 1000 + tv_diff.tv_usec / 1000;
	std::cout << "\n elapsed time" << total_msec << " msec\n" << std::endl;
}

