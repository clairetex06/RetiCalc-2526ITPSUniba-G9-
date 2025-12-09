#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345    // Porta su cui il server rimane in ascolto
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Creazione socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Errore creazione socket");
        exit(EXIT_FAILURE);
    }

    // Preparazione indirizzo server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // accetta connessioni da qualsiasi IP
    server_addr.sin_port = htons(PORT);

    // Binding
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 5) < 0) {
        perror("Errore listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server in ascolto sulla porta %d...\n", PORT);

    while (1) {
        // Accept
        if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len)) < 0) {
            perror("Errore accept");
            continue;
        }
        printf("Nuovo client connesso: %s\n", inet_ntoa(client_addr.sin_addr));

        // Invia stringa di connessione
        strcpy(buffer, "connessione avvenuta");
        send(client_fd, buffer, strlen(buffer)+1, 0);

        // Ricevi lettera dal client
        int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            close(client_fd);
            continue;
        }

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

        send(client_fd, buffer, strlen(buffer)+1, 0);

        if (operation_valid) {
            int num1, num2;
            // Ricevi due interi
            recv(client_fd, &num1, sizeof(int), 0);
            recv(client_fd, &num2, sizeof(int), 0);

            int result;
            switch (letter) {
                case 'A': case 'a': result = num1 + num2; break;
                case 'S': case 's': result = num1 - num2; break;
                case 'M': case 'm': result = num1 * num2; break;
                case 'D': case 'd':
                    if(num2 != 0) result = num1 / num2;
                    else result = 0;
                    break;
            }

            send(client_fd, &result, sizeof(int), 0);
        }

        close(client_fd);
        printf("Client disconnesso.\n");
    }

    close(server_fd);
    return 0;
}
/*
 * server-TCP_G9.c
 *
 *  Created on: 9 dic 2025
 *      Author: jane
 */


