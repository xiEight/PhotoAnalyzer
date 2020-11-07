#ifndef CLIENT_H
#define CLIENT_H
#include <winsock2.h>
#include <stdexcept>

class Client
{
public:
    Client(std::string ip, std::int16_t port);
    bool SendToServer(char *buffer, size_t bufferLength);
    char* ReceiveFromServer(size_t length);
private:
    SOCKADDR_IN addr;
    size_t addrSize;
    SOCKET sock;
};

#endif // CLIENT_H
