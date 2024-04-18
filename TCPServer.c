#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define PORT 23

int TCPServer()
{
    WSADATA wsaData;
    SOCKET listen_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    int clientLen = sizeof(client_addr);
    char recvbuf[100];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }
    //IPv4 adresleri üzerinden iletiþim saðlayan - SOCK_STREAM(TCP) tipinde soket - soketin kullanacaðý taþýma protokolü (TCP
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        WSACleanup(); //API'yi temizle - Socketler serbest
        return 1;
    }
  /*  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("\n Socket creation error \n");
        return 1;
    }*/
    //server_addr struct'ýnýn özelliklerini ata
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(PORT);

    //bu server_addr'in IP atamasýný yap - inet_pton -> IP'yi text to Binary
    if (inet_pton(AF_INET, "192.168.0.36", &server_addr.sin_addr) <= 0) {
        printf("Invalid address");
        return 1;
    }

    //Server soketini(listen_fd) server_addr ile iliþkilendir 
    if (bind(listen_fd, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("bind failed: %d\n", WSAGetLastError());
        closesocket(listen_fd);
        WSACleanup();
        return 1;
    }

    //listen_fd soketinin gelen baðlantýlarý dinlemesini saðlar - ayný anda max 1 tane baðlantý kabul edebilir 
    if (listen(listen_fd, 1) == SOCKET_ERROR) {
        printf("listen failed: %d\n", WSAGetLastError());
        closesocket(listen_fd);
        WSACleanup();
        return 1;
    }
    printf("Server listening on port %d\n", PORT);

    while (1) {
        //accept() - yeni bir baðlantý isteði geldiðinde yeni bir socket oluþturur
        //listen_fd soketini kullanarak gelen baðlantýlarý kabul eder - Client'a ait bilgileri -> IP ve Port'u tutan client_addr ile verdik
        //accept'ten önce socket() ile client_fd soketini oluþturmaya gerek yok
        if ((client_fd = accept(listen_fd, (SOCKADDR*)&client_addr, &clientLen)) == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(listen_fd);
            WSACleanup();
            return 1;
        }
        //Kabul edilen client_fd soketi üzerinden veri almak için kullanýlýr
        int bytesReceived = recv(client_fd, recvbuf, sizeof(recvbuf), 0);
        if (bytesReceived < 0) {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(client_fd);
            WSACleanup();
            return 1;
        }
        recvbuf[bytesReceived-1] = '\0';

        //client fd soketinin Veri gönderme yönünü kapatýr -
        //SD_RECEIVE veri alma yönünü kapatmak için - SD_BOTH çift yönü 
        if (shutdown(client_fd, SD_SEND) == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(client_fd);
            WSACleanup();
            return 1;
        }

        printf("Received: %s", recvbuf);

        closesocket(client_fd);
    }

    WSACleanup();

    return 0;
}