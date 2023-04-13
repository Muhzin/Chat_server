#ifndef SERVER_H
#define SERVER_H

#include <poll.h>

/**
 * The Server structure stores the file descriptors of the listening socket and
 * the client socket. It can store up to 2 client socket to communicate with
 * them
 */
struct Server {
    struct pollfd
        fds[3];    ///< Array storing the listening socket + the client socket
    int nbClients; ///< Current number of clients connected to the server
};

/**
 * Given a server, accept new clients if there are less than 2
 * currently connected.
 */
int acceptNewClients(struct Server* _server);

/**
 * Given a server and a socket, reads a Message from the socket and broadcast it
 * to the other client. Return -1 if the message could not be read, -2 if the
 * message could not be sent to the other client.
 *
 * If the message cannot be read, the socket descriptor of the sending client is
 * set to -1.
 *
 * If the message cannot be sent, the socket descriptor of the would be
 * receiving client is set to -1.
 */
int receiveAndBroadcastMessage(struct Server* _server, int _sendingClientFd);

/**
 * Given a server, compact the array of descriptor if any client descriptor is
 * set to -1.
 */
void compactDescriptor(struct Server* _server);

#endif

