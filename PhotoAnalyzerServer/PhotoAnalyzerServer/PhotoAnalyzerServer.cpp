// PhotoAnalyzerServer.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <set>
#define POLL_SIZE 7001

int main()
{
    //dll init
    WORD versionReq = MAKEWORD(2, 2);
    WSAData wsaData;
    if (WSAStartup(versionReq, &wsaData))
        return -1;
    //server socket init
    SOCKET masterSocket = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5441);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int size = sizeof(serverAddr);
    bind(masterSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    listen(masterSocket, SOMAXCONN);
    //poll init
    std::set<SOCKET> SlaveSockets;
    std::set<SOCKET> usingSockets;
    pollfd Set[POLL_SIZE];
    Set[0].fd = masterSocket;
    Set[0].events = POLLIN;
    while (true)
    {
        unsigned index = 1;
        for (auto Iter : SlaveSockets)
        {
            Set[index].fd = Iter;
            Set[index].events = POLLIN;
            index++;
        }
        unsigned setSize = 1 + SlaveSockets.size();
        WSAPoll(Set, setSize, -1);

        for (unsigned int i = 0; i < setSize; i++)
        {
            if (Set[i].revents & POLLHUP)
            {
                //if client disconnected
                std::cout << "Disconnected!" << std::endl;
                SlaveSockets.erase(SlaveSockets.find(Set[i].fd));
            }

            if (Set[i].revents & POLLIN)
            {
                if (i)
                {
                        //Receiving image and recognition
                }
                else
                {
                    //New connection add to poll set
                    SOCKET slave = accept(masterSocket, (SOCKADDR*)&serverAddr, &size);
                    SlaveSockets.insert(slave);
                    std::cout << "Connected!" << std::endl;
                    
                }
            }
        }

    }
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
