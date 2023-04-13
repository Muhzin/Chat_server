#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <termios.h>

#define PORT 12345
#define BUFLEN 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s serial_port\n", argv[0]);
        exit(1);
    }

    // Create the socket file descriptor
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        error("Error creating socket");

    // Fill in the address structure
    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to the address
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
        error("Error binding");

    // Open the serial port
    int serial_port;
    if ((serial_port = open(argv[1], O_RDWR | O_NOCTTY)) == -1)
        error("Error opening serial port");

    // Configure the serial port
    struct termios options;
    tcgetattr(serial_port, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(serial_port, TCIFLUSH);
    tcsetattr(serial_port, TCSANOW, &options);

    // Set up the message buffer
    char message[BUFLEN];
    socklen_t client_len = sizeof(client_addr);

    // Start receiving messages
    while (1) {
        memset(message, 0, BUFLEN);
        if (recvfrom(sockfd, message, BUFLEN, 0, (struct sockaddr *) &client_addr, &client_len) == -1)
            error("Error receiving");

        // Display the message
        printf("%s\n", message);

        // Check if the message contains "led"
        if (strstr(message, "led") != NULL) {
            // Write "led" to the serial port
            write(serial_port, "led", 3);
        }
    }

    return 0;
}
