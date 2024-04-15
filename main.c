// Client side C program to demonstrate Socket
// programming
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 23

int main(int argc, char const* argv[])
{
    WSADATA wsa; //Winsock api'nin ba�lat�lmas� i�in
    SOCKET client_fd;//iletisim kurmak icin bir client socket tan�mlan�r - bu socket icin bir tam say� atan�r
    struct sockaddr_in serv_addr;//sunucu adresi ve baglanti bilgilerini tutacak olan tan�mlay�c�
    char* hello = "Hello from client";
    char buffer[1024] = { 0 };

    // Initialize Winsock - 0'sa basarili
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { //MAKEWORD 8bit de�erini birle�tirerek 16bitlik bir de�er olu�turur - 0x0202 dondurur -> WinsockApi 2.2 s�r�m�
        printf("WSAStartup failed.\n");
        return 1;
    }

    //client'� socket'e tan�mlamak icin / AF_INET = IPv4 / SOCK_STREAM = TCP
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("\n Socket creation error \n");
        return 1;
    }

    serv_addr.sin_family = AF_INET;//socket internetin(sin) ailesini IPv4 olarak ayarla -
    serv_addr.sin_port = htons(PORT);//Host TO Network Short - host byte s�ras�ndaki bir degeri network byte'a cevirir - SORUN YASANMAMASI ICIN

    // inet_pton(internet presentation to network) Sunucunun IP adresini ayarlar - IP adresini text'den BINARY formuna getirir
    if (inet_pton(AF_INET, "192.168.254.12", &serv_addr.sin_addr)<= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return 1;
    }
    //client icin olusturulan socket sunucuya baglanir
    if (connect(client_fd, (struct sockaddr*)&serv_addr,
        sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return 1;
    }

    send(client_fd, hello, strlen(hello), 0);
    printf("Hello message sent\n");

    int valread;
    if ((valread = recv(client_fd, buffer, 1024, 0)) ==
        SOCKET_ERROR) {
        printf("\nReceive failed \n");
    }
    else {
        buffer[valread] = '\0'; // add null terminator
        printf("%s\n", buffer);
        printf("Receive succesful");
    }

    // closing the connected socket
    closesocket(client_fd);
    WSACleanup();
    return 0;
}