#ifndef CHAT_H
#define CHAT_H

#include <netinet/in.h>

#define BUFFER_LENGTH 1024
#define NAME_LENGTH 13

/**
 * Given a message error, prints it on stderr and exit the program
 */
void error(const char* msg);

/**
 * Represents a message send between users of the server
 */
struct Message {
    char nickname[NAME_LENGTH];  ///< Nickname of the user that sent the message
    char message[BUFFER_LENGTH]; ///< Message sent
    int flags;                   ///< Reserved for Digital manufacturing
};

/**
 * Fills the socket information for the server
 */
struct sockaddr_in setupServer(uint16_t _port);

/**
 * Given a socker and a client address, tries to read a message
 */
int readMessage(int _connectedFd, struct Message* _message);

/**
 * Given a socket and a client address, returns 0 if the send was succesfully
 * sent, -1 otherwise.
 */
int sendMessage(int _sockfd, struct Message* _message);

#endif

