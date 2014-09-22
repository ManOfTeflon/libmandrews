#include "socket.h"
#include "util.h"
#include <signal.h>
#include <sys/un.h>

static void handle_SIGPIPE(int signal) {
}

Connection* Connection::accept(int server_sock) {
    struct sockaddr_in client_addr;
    socklen_t client_len;
    int sock;

    /* Set the size of the in-out parameter */
    client_len = sizeof(client_addr);

    /* Wait for a client to connect */
    if ((sock = ::accept4(server_sock, (struct sockaddr *) &client_addr, 
                    &client_len, SOCK_NONBLOCK)) < 0) {
        return NULL;
    }

    return new Connection(sock, client_addr, false);
}

bool Connection::recv(void* data, size_t len) {
    struct pollfd fd { _sock, POLLIN };
    while (len) {
        int p = ::poll(&fd, 1, 100);
        if (p == 0) continue;
        else if (p < 0) goto error;

        /* See if there is more data to receive */
        int recved = ::recv(_sock, (char*)data, len, 0);

        if (recved < 0) goto error;
        data = (char*)data + recved;
        len -= recved;
    }

    return true;

error:
    P(ERR) << "Receive failed on " << (_client ? "client" : "server") << " socket " << _sock << ": " << strerror(errno);
    return false;
}

bool Connection::send(void* data, size_t len) {
    struct pollfd fd { _sock, POLLOUT };
    bool polling = true;
    while (len) {
        polling = true;
        int p = ::poll(&fd, 1, 100);
        if (p == 0) continue;
        else if (p < 0) goto error;

        polling = false;
        ssize_t sent = ::send(_sock, data, len, 0);
        if (sent < 0) goto error;
        data = (char*)data + sent;
        len -= sent;
    }
    return true;

error:
    P(ERR) << (polling ? "Poll " : "Send to ") << (_client ? "client" : "server") << " socket " << _sock
           << " failed: " << strerror(errno);
    return false;
}

TcpSocket::TcpSocket() {
    /* Create socket for incoming connections */
    _shutdown = false;
    if ((_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        P(FTL) << "Socket failed: " << strerror(errno);
    }
}

void TcpSocket::bind(int port)
{
    struct sockaddr_in server_addr;

    /* Construct local address structure */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    /* Bind to the local address */
    if (::bind(_sock, (struct sockaddr*)&server_addr,
                sizeof(server_addr)) < 0) {
        P(ERR) << "TcpSocket " << _sock << " failed to bind on port "
            << port << ": " << strerror(errno);
        return;
    }
}

void Socket::listen(int max_pending, std::function<void(Connection*)>& callback) {
    /* Mark the socket so it will listen for incoming connections */
    if (::listen(_sock, max_pending) < 0) {
        P(ERR) << "Listen failed on server socket " << _sock << ": " << strerror(errno);
        return;
    }

    std::vector<std::thread> threads;
    void (*handle)(int) = signal (SIGPIPE, handle_SIGPIPE);
    struct pollfd fd { _sock, POLLIN | POLLPRI };
    while (!_shutdown) {
        if (::poll(&fd, 1, 100) == 0) continue;
        Connection* conn = Connection::accept(_sock);
        if (conn)
        {
            threads.emplace_back([conn, &callback] {
                callback(conn);
                conn->close();
            });
        }
    }
    for (auto& thread : threads)
    {
        thread.join();
    }
    signal (SIGPIPE, handle);
}

Connection* TcpSocket::connect(unsigned long addr, int port, uint64_t msec_timeout) {
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = addr;
    server_addr.sin_port        = htons(port);

    if (::connect(_sock, (struct sockaddr *) &server_addr,
                sizeof(server_addr)) < 0) {
        P(ERR) << "Connecting to socket " << _sock << " failed: " << strerror(errno);
        return NULL;
    }

    return new Connection(_sock, server_addr, true);
}

UnixSocket::UnixSocket() {
    /* Create socket for incoming connections */
    _shutdown = false;
    if ((_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        P(FTL) << "Socket failed: " << strerror(errno);
    }
}

Connection* UnixSocket::connect(const char* path, uint64_t msec_timeout) {
    struct sockaddr_un server_addr;

    bzero((char *)&server_addr,sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, path);
    size_t servlen = strlen(server_addr.sun_path) + sizeof(server_addr.sun_family);

    auto start = now();
    do
    {
        if (::connect(_sock, (struct sockaddr *) &server_addr, servlen) >= 0) {
            return new Connection(_sock, true);
        }
        usleep(10000);
    }
    while (now() - start < 1 * US_PER_SEC);

    if (::connect(_sock, (struct sockaddr *) &server_addr, servlen) < 0) {
        P(ERR) << "Connecting socket " << _sock << " to " << path << " failed: " << strerror(errno);
        return NULL;
    }
    return new Connection(_sock, true);
}

void UnixSocket::bind(const char* path)
{
    /* Construct UNIX address structure */
    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, path);

    size_t servlen=strlen(server_addr.sun_path) + sizeof(server_addr.sun_family);

    /* Bind to the local address */
    if (::bind(_sock, (struct sockaddr*)&server_addr, servlen) < 0) {
        P(ERR) << "UnixSocket " << _sock << " failed to bind on path "
            << path << ": " << strerror(errno);
        return;
    }
}
