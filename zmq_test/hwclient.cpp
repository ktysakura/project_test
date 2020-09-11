#include <zmq.hpp>
#include <zmq.h> 
#include <unistd.h>

int main()
{
	zmq::context_t context(1);
	zmq::socket_t requestor(context, ZMQ_REQ);

	requestor.connect("tcp://localhost:5555");
	for (int i = 0; i != 10; i++) {
		zmq::message_t request(10);

		memcpy(request.data(), "hello", strlen("hello"));
		requestor.send(request);

		zmq::message_t reply;

		requestor.recv(&reply);
		printf("received %d:%s\n", i, (char *)reply.data());
		sleep(1);
	}

	return 0;
}
