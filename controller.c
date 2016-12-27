#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#include "plugin.h"
#include "controller.h"

#define HOST "localhost"
#define PORT 8082
#define BUFFER_SIZE 1024

int socket_connect(char *host, int portno) {
    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd;

    /* create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) DebugMessage(M64MSG_ERROR, "ERROR opening socket");

    /* lookup the ip address */
    server = gethostbyname(host);
    if (server == NULL) DebugMessage(M64MSG_ERROR, "ERROR, no such host");

    /* fill in the structure */
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

    /* connect the socket */
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        DebugMessage(M64MSG_ERROR, "ERROR connecting");

    return sockfd;
}

void read_controller() {
  int sockfd = socket_connect(HOST, PORT);

  int bytes, sent, received, total;
  char message[1024], response[4096]; // allocate more space than required.
  sprintf(message,"GET / HTTP/1.0\r\n\r\n");

  /* send the request */
  total = strlen(message);
  sent = 0;
  do {
      bytes = write(sockfd,message+sent,total-sent);
      if (bytes < 0)
          DebugMessage(M64MSG_ERROR, "ERROR writing message to socket");
      if (bytes == 0)
          break;
      sent+=bytes;
  } while (sent < total);

  /* receive the response */
  memset(response,0,sizeof(response));
  total = sizeof(response)-1;
  received = 0;
  do {
      bytes = read(sockfd,response+received,total-received);
      if (bytes < 0)
          DebugMessage(M64MSG_ERROR, "ERROR reading response from socket");
      if (bytes == 0)
          break;
      received+=bytes;
  } while (received < total);

  if (received == total)
      DebugMessage(M64MSG_ERROR, "ERROR storing complete response from socket");

  DebugMessage(M64MSG_INFO, response);

  close(sockfd);
}
