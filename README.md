# MSc IT simple chat project

This repository contains code for the Msc IT simple chat project. It provides two executable: a server and a client. The server is able to receive messages from at most two clients and it transfer the message sent from one client to another.


## CMake

The project uses CMake for building.

## Server

You can start the server from the command line as simple as

```
server
```

Once launched, the server automatically accepts connection from any client until there are two connected.

## Chat

You can start the chat from the command line. The executable takes a single parameters which is your name.

```
client <name>
```
