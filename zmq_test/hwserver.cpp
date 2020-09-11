#include <zmq.hpp>
#include <zmq.h>


int main()
{
	zmq::context_t context(1);
	zmq::socket_t  responsor(context, ZMQ_REP);

	responsor.bind("tcp://*:5555");
	while(true) {
		char buf[10] = {0};
		zmq::message_t response;
		responsor.recv(&response);
		memcpy(buf, response.data(), response.size());

		printf("received :%s\n", (char *)response.data());
		//printf("received :%s\n", buf);

		zmq::message_t reply(10);
		memcpy(reply.data(), "world", strlen("world"));
		responsor.send(reply);
	}
}
