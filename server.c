#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "chat.h"
#include "server.h"

#define PORT 12345

void error(const char* msg) {
    perror(msg);
    exit(1);
}

struct Server createServer() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        error("Error creating socket");
    }
    struct Server retval;
    retval.nbClients = 0;
    memset(&retval.fds[0], 0, sizeof(struct pollfd));
    retval.fds[0].fd = fd;
    retval.fds[0].events = POLLIN;
    // Bind the socket to the address
    struct sockaddr_in serverAddress = setupServer(PORT);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(retval.fds[0].fd, (struct sockaddr*)&serverAddress,
            sizeof(serverAddress))
        == -1) {
        error("Error binding");
    }
    int on;
    if (setsockopt(
            retval.fds[0].fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on))
        < 0) {
        error("Error setting reuse option\n");
    }
    if (ioctl(retval.fds[0].fd, FIONBIO, (char*)&on) < 0) {
        error("Error setting non block option\n");
    }
    if (listen(retval.fds[0].fd, 32) < 0) {
        error("Error setting reuse option\n");
    }
    return retval;
}

int acceptNewClients(struct Server* _server) {
    if (_server == NULL) {
        return -2;
    }
    int newFd;
    do {
        newFd = accept(_server->fds[0].fd, NULL, NULL);
        if (newFd < 0) {
            if (errno != EWOULDBLOCK) {
                perror("accept() failed");
                return -1;
            }
            break;
        }
        _server->nbClients++;
        memset(&_server->fds[_server->nbClients], 0, sizeof(struct pollfd));
        _server->fds[_server->nbClients].fd = newFd;
        _server->fds[_server->nbClients].events = POLLIN;
        printf(
            "New client connected: %d\n", _server->fds[_server->nbClients].fd);
    } while (newFd != -1 && _server->nbClients < 2);
    return 0;
}

int receiveAndBroadcastMessage(struct Server* _server, int _sendingClientFd) {
    if (_server == NULL) {
        return -3;
    }
    struct Message message;
    int read = readMessage(_sendingClientFd, &message);
    if (read < 0) {
        return -1;
    }
    // Send message to all other clients
    for (int j = 1; j < _server->nbClients + 1; ++j) {
        if (_server->fds[j].fd == _sendingClientFd) {
            continue;
        }
        int sent = sendMessage(_server->fds[j].fd, &message);
        if (sent < 0) {
            _server->fds[j].fd = -1;
            return -2;
        }
    }
    // Print the message
    printf("%s> %s\n", message.nickname, message.message);
    return 0;
}

void compactDescriptor(struct Server* _server) {
    if (_server == NULL) {
        return;
    }
    for (int i = 1; i < _server->nbClients + 1; i++) {
        if (_server->fds[i].fd == -1) {
            for (int j = i; j < _server->nbClients + 1; j++) {
                _server->fds[j].fd = _server->fds[j + 1].fd;
            }
            i--;
            _server->nbClients--;
        }
    }
}

void runServer(struct Server* _server) {
    if (_server == NULL) {
        return;
    }
    while (1) {
        printf("Current nb clients: %d\n", _server->nbClients);
        int retpoll = poll(_server->fds, _server->nbClients + 1, -1);
        if (retpoll < 0) {
            perror("Call to poll failed");
            break;
        }
        if (retpoll == 0) {
            continue;
        }
        printf("Checking %d fds\n", retpoll);
        if (_server->fds[0].revents == POLLIN) {
            if (acceptNewClients(_server) == -1) {
                perror("Error accepting new clients");
                break;
            }
        }
        // Check client's sockets
        for (int i = 1; i < _server->nbClients + 1; i++) {
            // Skip file descriptor with no event
            if (_server->fds[i].revents == 0) {
                continue;
            }
            if (_server->fds[i].revents != POLLIN) {
                _server->fds[i].fd = -1;
                break;
            }
            int broadcast =
                receiveAndBroadcastMessage(_server, _server->fds[i].fd);
            if (broadcast == -1) {
                perror("Error reading message");
                _server->fds[i].fd = -1;
                break;
            }
        }
        compactDescriptor(_server);
    }
    for (int i = 0; i < _server->nbClients; ++i) {
        close(_server->fds[i].fd);
    }
}

int main() {
    struct Server server = createServer();
    runServer(&server);
    return 0;
}
