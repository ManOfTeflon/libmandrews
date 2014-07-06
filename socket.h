#ifndef SOCKET_H_
#define SOCKET_H_

#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <functional>

class Connection {
public:
    int recv(void* data, int len);
    template<typename T>
    int recv(T* t) {
        return recv(t, sizeof(T));
    }

    int send(void* data, int len);
    template<typename T>
        int send(T* t) {
            return send(t, sizeof(T));
        }
    char* remote() {
        return inet_ntoa(_remote.sin_addr);
    }
    int port() {
        return _remote.sin_port;
    }

    void close() { delete this; }

private:
    static Connection* connect(int client_sock, unsigned long addr, int port);
    static Connection* accept(int server_sock);
    friend class Socket;

    Connection(int sock, const sockaddr_in& remote)
        : _sock(sock), _remote(remote) { }

    virtual ~Connection() { ::close(_sock); }

    int _sock;
    sockaddr_in _remote;
};

class Socket {
public:
    Socket();
    virtual ~Socket() { close(_sock); }

    virtual void listen(int port, int max_pending, std::function<void(Connection*)>& callback);
    virtual Connection* connect(unsigned long addr, int port);

private:
    int _sock;
};

#endif  // SOCKET_H_
