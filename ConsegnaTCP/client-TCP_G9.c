#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char server_name[100];

    printf("Inserisci il nome del server: ");
    scanf("%s", server_name);

    // Risoluzione nome
    struct hostent *he = gethostbyname(server_name);
    if (he == NULL) {
        perror("Impossibile risolvere il nome del server");
        exit(EXIT_FAILURE);
    }

    // Creazione socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Errore creazione socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr = *((struct in_addr *)he->h_addr);

    // Connessione al server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore connessione");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Ricevi messaggio connessione
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("Server: %s\n", buffer);

    // Leggi lettera
    char letter;
    printf("Inserisci una lettera (A,S,M,D): ");
    scanf(" %c", &letter);
    send(sock, &letter, 1, 0);

    // Ricevi risposta
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("Server: %s\n", buffer);

    if (strcmp(buffer, "TERMINE PROCESSO CLIENT") != 0) {
        int num1, num2, result;
        printf("Inserisci due numeri interi: ");
        scanf("%d %d", &num1, &num2);

        send(sock, &num1, sizeof(int), 0);
        send(sock, &num2, sizeof(int), 0);

        recv(sock, &result, sizeof(int), 0);
        printf("Risultato: %d\n", result);
    }

    close(sock);
    return 0;
}
