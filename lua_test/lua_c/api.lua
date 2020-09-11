
require "myapi"


tcp = myapi.socket.tcp()
udp = myapi.socket.udp()

--tcp.connect()
tcp.send()
tcp.receive()
tcp.close()

udp.send()
udp.receive()


