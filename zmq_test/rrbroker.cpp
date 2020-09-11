
#include <zmq.hpp>
#include <iostream>

int main()
{
	zmq::context_t context(1);
	zmq::socket_t  frontend(context, ZMQ_ROUTER);
	zmq::socket_t  backend(context, ZMQ_DEALER);

	frontend.bind("tcp://*:5559");
	backend.bind("tcp://*:5560");

	zmq::pollitem_t items[] = {
		{ static_cast<void *>(frontend), 0, ZMQ_POLLIN, 0 },
		{ static_cast<void *>(backend), 0, ZMQ_POLLIN, 0 }
	};

	while (1) {
		zmq::message_t msg;
		int more;

		zmq::poll(&items[0], 2, -1);

		if (items[0].revents & ZMQ_POLLIN) {
			while (1) {
				frontend.recv(&msg);

				size_t more_size = sizeof(more);
				
				frontend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
				printf("frontend: %d, %ld, %s\n", more, msg.size(), (char *)msg.data());
				backend.send(msg, more ? ZMQ_SNDMORE : 0);
				if (!more) { 
					break;
				}
			}

		}

		if (items[1].revents & ZMQ_POLLIN) {
			while (1) {
				backend.recv(&msg);

				size_t more_size = sizeof(more);

				backend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
				printf("backend: %d, %ld, %s\n", more, msg.size(), (char *)msg.data());
				frontend.send(msg, more ? ZMQ_SNDMORE : 0);
				if (!more) {			
					break;
				}
			}
		}
		std::cout << std::endl; 
	}

}
