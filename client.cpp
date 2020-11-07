#include "client.h"

Client::Client(std::string ip, std::int16_t port)
{
    WSADATA wsaData;
    WORD dllVersion = MAKEWORD(2,2);

    if (WSAStartup(dllVersion, &wsaData) != 0)
    {
        throw std::runtime_error("Wrong ws2_32 DLL version!");
    }

    addrSize = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    sock = socket(AF_INET, SOCK_STREAM, NULL);
    if (connect(sock, (SOCKADDR*)&addr, addrSize) != 0)
        throw std::runtime_error("Failed to connect to server!");
}

bool Client::SendToServer(char *buffer, size_t bufferLength)
{
    return (send(sock, buffer, bufferLength, 0) == bufferLength?true:false);
}

char* Client::ReceiveFromServer(size_t length)
{
    char *buffer = new char[length];
    if (recv(sock,buffer, length,0) == length)
        return buffer;
    else
        throw std::runtime_error("Receiving error!");

}
