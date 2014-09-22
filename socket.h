#ifndef SOCKET_H_
#define SOCKET_H_

#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <functional>
#include <atomic>
#include <thread>
#include <vector>

class Connection {
public:
    bool recv(void* data, size_t len);
    template<typename T>
    bool recv(T* t) {
        return recv(t, sizeof(T));
    }
    ssize_t try_recv(void* data, size_t len) {
        return ::recv(_sock, (char*)data, len, 0);
    }
    ssize_t poll(short flags) {
        struct pollfd fd { _sock, flags };
        return ::poll(&fd, 1, 100);
    }

    bool send(void* data, size_t len);
    template<typename T>
    bool send(T* t) {
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
    friend class TcpSocket;
    friend class UnixSocket;

    Connection(int sock, bool client)
        : _client(client), _sock(sock), _remote{} { }
    Connection(int sock, const sockaddr_in& remote, bool client)
        : _client(client), _sock(sock), _remote(remote) { }

    virtual ~Connection() { ::close(_sock); }

    bool _client;
    int _sock;
    sockaddr_in _remote;
};

class Socket {
public:
    virtual ~Socket() = 0;

    virtual void listen(int max_pending, std::function<void(Connection*)>& callback);

    virtual void shutdown() {
        _shutdown = true;
    }

protected:

    std::atomic<bool> _shutdown;
    int _sock;
};

inline Socket::~Socket() { close(_sock); }

class TcpSocket : public Socket {
public:
    TcpSocket();
    virtual ~TcpSocket() { }

    Connection* connect(unsigned long addr, int port, uint64_t msec_timeout = 0);
    void bind(int port);
};

class UnixSocket : public Socket {
public:
    UnixSocket();
    virtual ~UnixSocket() { }

    Connection* connect(const char* path, uint64_t msec_timeout = 0);
    void bind(const char* path);
protected:
};

#endif  // SOCKET_H_
