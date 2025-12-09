#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 12346
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    char buffer[BUFFER_SIZE];
    char server_name[100];

    printf("Inserisci il nome del server: ");
    scanf("%s", server_name);

    struct hostent *he = gethostbyname(server_name);
    if (!he) { perror("Errore risoluzione nome"); exit(EXIT_FAILURE); }

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { perror("Errore socket"); exit(EXIT_FAILURE); }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr = *((struct in_addr *)he->h_addr);

    char letter;
    printf("Inserisci una lettera (A,S,M,D): ");
    scanf(" %c", &letter);

    sendto(sock, &letter, 1, 0, (struct sockaddr*)&server_addr, addr_len);

    recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, &addr_len);
    printf("Server: %s\n", buffer);

    if (strcmp(buffer, "TERMINE PROCESSO CLIENT") != 0) {
        int num1, num2, result;
        printf("Inserisci due numeri interi: ");
        scanf("%d %d", &num1, &num2);

        sendto(sock, &num1, sizeof(int), 0, (struct sockaddr*)&server_addr, addr_len);
        sendto(sock, &num2, sizeof(int), 0, (struct sockaddr*)&server_addr, addr_len);

        recvfrom(sock, &result, sizeof(int), 0, (struct sockaddr*)&server_addr, &addr_len);
        printf("Risultato: %d\n", result);
    }

    close(sock);
    return 0;
}
