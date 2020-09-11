
#include <unistd.h>
#include <zmq.hpp>
#include <zmq.h>

#define within(num) (int)((float)num * random()/(RAND_MAX/1.0)) 

int main()
{
	zmq::context_t context(1);
	zmq::socket_t	publisher(context, ZMQ_PUB);

	publisher.bind("tcp://*:5556");
	srandom((unsigned)time(NULL));
	while (true) {
		int zipcode, temperator;
		zipcode = random() %5 + 100;
		temperator = within(215) - 80;
		zmq::message_t message(20);
		
#if 1
		snprintf((char *)message.data(), message.size(), "%03d %d", zipcode, temperator);
		if (publisher.send(message) < 0) {
			return -1;
		}
#endif
		printf("send success:%s\n", (char *)message.data());
	
		sleep(1);
	}
	publisher.close();
	context.close();
}
