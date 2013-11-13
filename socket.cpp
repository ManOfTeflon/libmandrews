#include "socket.h"
#include "util.h"
#include <signal.h>

static void handle_SIGPIPE(int signal) {
}

Connection* Connection::accept(int server_sock) {
  struct sockaddr_in client_addr;
  socklen_t client_len;
  int sock;

  /* Set the size of the in-out parameter */
  client_len = sizeof(client_addr);
  
  /* Wait for a client to connect */
  if ((sock = ::accept(server_sock, (struct sockaddr *) &client_addr, 
  					   &client_len)) < 0) {
    perror("accept");
    P(ERR) << "Accept failed on socket " << server_sock << "!";
    return NULL;
  }
  
  return new Connection(sock, client_addr);
}

Connection* Connection::connect(int client_sock, unsigned long addr, int port) {
	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family      = AF_INET;
	server_addr.sin_addr.s_addr = addr;
	server_addr.sin_port        = htons(port);

	if (::connect(client_sock, (struct sockaddr *) &server_addr,
              sizeof(server_addr)) < 0) {
    perror("connect");
    P(ERR) << "Connecting to socket " << client_sock << " failed!";
    return NULL;
	}

  return new Connection(client_sock, server_addr);
}

int Connection::recv(void* data, int len) {
  int total_bytes_rcvd = 0;

  while (total_bytes_rcvd < len) {
  	/* See if there is more data to receive */
  	int recv_msg_size = ::recv(_sock, (char*)data + total_bytes_rcvd,
                               len - total_bytes_rcvd, 0);

    if (recv_msg_size < 0) {
      perror("recv");
      P(ERR) << "Receive failed on client socket " << _sock << "!";
      return -1;
  	} else if (recv_msg_size == 0) {
      break;
    }
  	total_bytes_rcvd += recv_msg_size;
  }

  return total_bytes_rcvd;
}

int Connection::send(void* data, int len) {
  int sent;
  if ((sent = ::send(_sock, data, len, 0)) != len) {
    perror("send");
    P(ERR) << "Send to client socket " << _sock << " failed!";
    return -1;
  }
  return sent;
}

Socket::Socket() {
	/* Create socket for incoming connections */
	if ((_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("socket");
    P(FTL) << "Socket failed!";
	}
}

void Socket::listen(int port, int max_pending, void(*callback)(Connection*)) {
	struct sockaddr_in server_addr;

	/* Construct local address structure */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	/* Bind to the local address */
	if (::bind(_sock, (struct sockaddr*)&server_addr,
           sizeof(server_addr)) < 0) {
    perror("bind");
    P(ERR) << "Socket " << _sock << " failed to bind on port "
      << port << "!";
    return;
	}

	/* Mark the socket so it will listen for incoming connections */
	if (::listen(_sock, max_pending) < 0) {
    perror("bind");
    P(ERR) << "Listed failed on server socket " << _sock << "!";
    return;
	}

  void (*handle)(int) = signal (SIGPIPE, handle_SIGPIPE);
	while (1) {
    Connection* conn = Connection::accept(_sock);

		callback(conn);

    conn->close();
	}
  signal (SIGPIPE, handle);
}

Connection* Socket::connect(unsigned long addr, int port) {
  return Connection::connect(_sock, addr, port);
}
