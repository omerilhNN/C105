#include <stdio.h>
#include <winsock2.h>
#include "TCPClient.h"
#pragma comment(lib, "ws2_32.lib")

#define PORT 23

void TCPClient() {
    WSADATA wsa; //Winsock api'nin baþlatýlmasý için
    SOCKET client_fd;//iletisim kurmak icin bir client socket tanýmlanýr - bu socket icin bir tam sayý atanýr
    struct sockaddr_in serv_addr;//sunucu adresi ve baglanti bilgilerini tutacak olan tanýmlayýcý
    char* hello = "Hello from client";
    char buffer[1024] = { 0 };

    // Initialize Winsock - 0'sa basarili
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { //MAKEWORD 8bit deðerini birleþtirerek 16bitlik bir deðer oluþturur - 0x0202 deðerini döndürür-> WinsockApi 2.2 sürümü
        printf("WSAStartup failed.\n");
        return 1;
    }

    //client'ý socket'e tanýmlamak icin / AF_INET = IPv4 / SOCK_STREAM = TCP
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("\n Socket creation error \n");
        return 1;
    }

    serv_addr.sin_family = AF_INET;//socket internetin(sin) ailesini IPv4 olarak ayarla -
    serv_addr.sin_port = htons(PORT);//Host TO Network Short - host byte sýrasýndaki bir degeri network byte'a cevirir - SORUN YASANMAMASI ICIN

    // inet_pton(internet presentation to network) Sunucunun IP adresini ayarlar - IP adresini text'den BINARY formuna getirir
    //192.168.254.12'i serv_addr'in addresine atamasýný yapar
    if (inet_pton(AF_INET, "192.168.254.12", &serv_addr.sin_addr) <= 0) {
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

    if (send(client_fd, hello, strlen(hello), 0) == SOCKET_ERROR) {
        printf("Sending message failed\n");
        // Hata iþleme kodu buraya eklenebilir
    }
    printf("Hello message sent\n");

    int valread;
    //gelen veriyi client soketi alýr ve buffer degerine kopyalar, flags = ek kontrol islemi yapýlacaksa ona göre ayarlanýr 
    if ((valread = recv(client_fd, buffer, 1024, 0)) ==
        SOCKET_ERROR) {
        printf("\nReceive failed \n");
    }
    else {
        buffer[valread] = '\0'; // add null terminator
        printf("%s\n", buffer);
        printf("Receive succesful\n");
    }

    // closing the connected socket
    closesocket(client_fd);
    WSACleanup();
}