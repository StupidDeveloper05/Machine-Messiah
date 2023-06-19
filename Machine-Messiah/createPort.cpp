#include <winsock2.h>
#include <iostream>

//#pragma comment(lib, "ws2_32.lib")

unsigned short GetAvailablePort()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 0;
    }

    SOCKET tempSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (tempSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return 0;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serverAddress.sin_port = 0;

    if (bind(tempSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind socket." << std::endl;
        closesocket(tempSocket);
        WSACleanup();
        return 0;
    }

    if (listen(tempSocket, 1) == SOCKET_ERROR)
    {
        std::cerr << "Failed to listen on socket." << std::endl;
        closesocket(tempSocket);
        WSACleanup();
        return 0;
    }

    sockaddr_in tempAddress{};
    int addressSize = sizeof(tempAddress);
    if (getsockname(tempSocket, (struct sockaddr*)&tempAddress, &addressSize) == SOCKET_ERROR)
    {
        std::cerr << "Failed to get socket address." << std::endl;
        closesocket(tempSocket);
        WSACleanup();
        return 0;
    }

    unsigned short portNumber = ntohs(tempAddress.sin_port);

    closesocket(tempSocket);
    WSACleanup();

    return portNumber;
}

int main()
{
    unsigned short port = GetAvailablePort();
    std::cout << "Available port: " << port << std::endl;

    return 0;
}
