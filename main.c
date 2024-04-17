#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

int main()
{
    WSADATA wsaData;
    SOCKET ListenSocket, ClientSocket;
    struct sockaddr_in server, client;
    int clientLen = sizeof(client);
    char recvbuf[100];
    int iResult;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(23);

    if (inet_pton(AF_INET,"192.168.0.36",&server.sin_addr)<= 0 ) {
        printf("Invalid address");
        return 1;
    }

    if (bind(ListenSocket, (SOCKADDR*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("bind failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    listen(ListenSocket, 1);

    while (1) {
        ClientSocket = accept(ListenSocket, (SOCKADDR*)&client, &clientLen);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        if (recv(ClientSocket, recvbuf, sizeof(recvbuf), 0)< 0) {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        if (shutdown(ClientSocket, SD_SEND) == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        printf("Received: %s", recvbuf);

        closesocket(ClientSocket);
    }

    WSACleanup();

    return 0;
}