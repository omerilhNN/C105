#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define PORT 4
#define CLIENT_SIZE 2

int main() {
    WSADATA wsaData;
    SOCKET server_fd, client_fd[CLIENT_SIZE];
    struct sockaddr_in server_addr, client_addr[CLIENT_SIZE];
    char recvbuf[100];

    // WSA baþlatma
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Sunucu soketini oluþturma
    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Sunucu adresini ve port numarasýný ayarlama
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET,"192.168.254.19",&server_addr.sin_addr) <= 0) {
        printf("Invalid IP address for server\n");
        return 1;
    }

     //Server soketini(server_fd) server_addr ile iliþkilendir 
    if (bind(server_fd, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("bind failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Baðlantýlarý dinleme
    if (listen(server_fd, 2) == SOCKET_ERROR){ // Maksimum 2 istemci baðlantýsýný kabul eder
        printf("listen failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    printf("Server listening on port %d\n", PORT);

    while (1) {
        for (int i = 0; i < CLIENT_SIZE; ++i) {
            // Ýstemci baðlantýsýný kabul etme
            int clientLen = sizeof(client_addr[i]);
            if ((client_fd[i] = accept(server_fd, (SOCKADDR*)&client_addr[i], &clientLen)) == INVALID_SOCKET) {
                printf("accept failed: %d\n", WSAGetLastError());
                closesocket(server_fd);
                WSACleanup();
                return 1;
            }
            printf("Client %d connected.\n", i + 1);
        }

        for (int i = 0; i < CLIENT_SIZE; ++i) {
            // Client'tan gelen veriyi alma
            int bytesReceived = recv(client_fd[i], recvbuf, sizeof(recvbuf), 0);
            if (bytesReceived < 0) {
                printf("recv failed with error: %d\n", WSAGetLastError());
                closesocket(client_fd[i]);
                closesocket(server_fd);
                WSACleanup();
                return 1;
            }
            recvbuf[bytesReceived] = '\0';
            printf("Received from client %d: %s\n", i + 1, recvbuf);

            // Ýstemciye mesaj gönderme
            const char* message = "Hello from server";
            if (send(client_fd[i], message, strlen(message), 0) != strlen(message)) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(client_fd[i]);
                closesocket(server_fd);
                WSACleanup();
                return 1;
            }

            

            // Ýstemci soketini kapatma
            closesocket(client_fd[i]);
        }

    }
    
    closesocket(server_fd);
    WSACleanup();

    return 0;
}
