#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12346
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Errore socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore bind");
        exit(EXIT_FAILURE);
    }

    printf("Server UDP pronto sulla porta %d\n", PORT);

    while (1) {
        // Ricevi lettera
        int bytes_received = recvfrom(sock, buffer, BUFFER_SIZE, 0,
                                      (struct sockaddr*)&client_addr, &addr_len);
        if (bytes_received <= 0) continue;

        char letter = buffer[0];
        int operation_valid = 1;
        if (letter == 'A' || letter == 'a') strcpy(buffer, "ADDIZIONE");
        else if (letter == 'S' || letter == 's') strcpy(buffer, "SOTTRAZIONE");
        else if (letter == 'M' || letter == 'm') strcpy(buffer, "MOLTIPLICAZIONE");
        else if (letter == 'D' || letter == 'd') strcpy(buffer, "DIVISIONE");
        else {
            strcpy(buffer, "TERMINE PROCESSO CLIENT");
            operation_valid = 0;
        }

        sendto(sock, buffer, strlen(buffer)+1, 0, (struct sockaddr*)&client_addr, addr_len);

        if (operation_valid) {
            int num1, num2;
            recvfrom(sock, &num1, sizeof(int), 0, (struct sockaddr*)&client_addr, &addr_len);
            recvfrom(sock, &num2, sizeof(int), 0, (struct sockaddr*)&client_addr, &addr_len);

            int result;
            switch (letter) {
                case 'A': case 'a': result = num1 + num2; break;
                case 'S': case 's': result = num1 - num2; break;
                case 'M': case 'm': result = num1 * num2; break;
                case 'D': case 'd': result = num2 != 0 ? num1 / num2 : 0; break;
            }

            sendto(sock, &result, sizeof(int), 0, (struct sockaddr*)&client_addr, addr_len);
        }
    }

    close(sock);
    return 0;
}
