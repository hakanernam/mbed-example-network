#include "mbed.h"
#include "EthernetInterface.h"
#include <Timer.h>


class TCPEchoEndpoint {
    TCPEchoEndpoint
};

class TCPEchoServer {
public:
    TCPEchoServer():
        _server()
    void start(uint16_t port) {
        _server.bind("0.0.0.0",port);
        _server.start_listening(_onIncoming.entry())
    }
protected:
    void onIncoming(socket_event_t *event);
    void onError(socket_event_t *event);
protected:
    _TCPListener _server;
    _TCPStream*  _stream;
protected:

};


namespace {
    const int ECHO_SERVER_PORT = 7;
    const int BUFFER_SIZE = 64;
}

int main (void) {
    char buffer[BUFFER_SIZE] = {0};
    EthernetInterface eth;
    eth.init(); //Use DHCP
    eth.connect();
    printf("MBED: Server IP Address is %s:%d\r\n", eth.getIPAddress(), ECHO_SERVER_PORT);

    TCPSocketServer server;
    server.bind(ECHO_SERVER_PORT);
    server.listen();

    while (true) {
        printf("MBED: Wait for new connection...\n");
        TCPSocketConnection client;
        server.accept(client);
        client.set_blocking(false, 1500); // Timeout after (1.5)s
        printf("MBED: Connection from: %s\r\n", client.get_address());

        while (true) {
            const int n = client.receive(buffer, sizeof(buffer));
            if (n <= 0) {
                break;
            }
            const int buffer_string_end_index = n >= BUFFER_SIZE ? BUFFER_SIZE-1 : n;
            buffer[buffer_string_end_index] = '\0';
            client.send_all(buffer, n);
            if (n <= 0) {
                break;
            }
        }
        client.close();
    }
}
