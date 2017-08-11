#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 51515
#define BUFSIZE 64

void logexit(const char *str) // This function was copied from the client.c file for error handling
{
    if(errno) perror(str);
    else puts(str);
    exit(EXIT_FAILURE);
}

int main()
{
    int sock, client;
    int counter = 0, tmpcounter, message;
    char buffer[BUFSIZE];
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    int addrlen = sizeof(client_addr); // Get the address length of the client address

    sock = socket(AF_INET, SOCK_STREAM, 0); // Create an endpoint for communication and returns a descriptor

    if(sock == -1) // If the connection cannot be estabilished, return an error message
        logexit("socket");

    server_addr.sin_family = AF_INET; // Address Family
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server address
    server_addr.sin_port = htons(PORT); // Port number

    if(bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) // Port assignment to the socket
        logexit("bind");

    if(listen(sock, 20) != 0) // Listen assignment to the socket with a backlog integer or how many processes can queue up
        logexit("listen");

    while(1)
    {
		client = accept(sock, (struct sockaddr*)&client_addr, &addrlen); // Accept a connection

        recv(client, buffer, 1, MSG_WAITALL); // Receive a byte of data, containing the '+' or '-' sign

        if(buffer[0] == '+') // If it is '+', increments the actual counter by 1 and do the mod 1000
            tmpcounter = (counter + 1) % 1000;

        else if(buffer[0] == '-') // If it is '-', decrements the actual counter by 1 and do the mod 1000
            tmpcounter = (counter - 1) % 1000;

        message = htonl(tmpcounter); // Transform the integer to a network byte order

		send(client, &message, 4, 0); // Send the temporary counter in network byte order to the client

        recv(client, buffer, 3, MSG_WAITALL); // Receive 3 bytes of data, containing the understood counter of the client

        if(atoi(buffer) == tmpcounter) // If the received counter from the client is equal to the temporary counter, it updates the counter value
            counter = tmpcounter;

        printf("%d\n", counter); // Prints the new counter

		close(client); // Close the client connection
    }

    close(sock); // Close the socket connection
    exit(EXIT_SUCCESS);
}
