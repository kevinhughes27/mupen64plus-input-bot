#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "plugin.h"
#include "controller.h"

#include "json.h"
#include "json_tokener.h"

int socket_connect(char *host, int portno) {
    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd;

    /* create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) DebugMessage(M64MSG_ERROR, "ERROR opening socket");

    /* lookup the ip address */
    server = gethostbyname(host);
    if (server == NULL) DebugMessage(M64MSG_ERROR, "ERROR, no such host '%s'", host);

    /* fill in the structure */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        DebugMessage(M64MSG_INFO, "ERROR connecting to %s (%s) on port %d, please start bot server.", host, inet_ntoa(serv_addr.sin_addr), portno);
        return -1;
    }

    return sockfd;
}

void clear_controller(int Control) {
    controller[Control].buttons.R_DPAD = 0;
    controller[Control].buttons.L_DPAD = 0;
    controller[Control].buttons.D_DPAD = 0;
    controller[Control].buttons.U_DPAD = 0;
    controller[Control].buttons.START_BUTTON = 0;
    controller[Control].buttons.Z_TRIG = 0;
    controller[Control].buttons.B_BUTTON = 0;
    controller[Control].buttons.A_BUTTON = 0;
    controller[Control].buttons.R_CBUTTON = 0;
    controller[Control].buttons.L_CBUTTON = 0;
    controller[Control].buttons.D_CBUTTON = 0;
    controller[Control].buttons.U_CBUTTON = 0;
    controller[Control].buttons.R_TRIG = 0;
    controller[Control].buttons.L_TRIG = 0;
    controller[Control].buttons.X_AXIS = 0;
    controller[Control].buttons.Y_AXIS = 0;
}

void read_controller(int Control) {
    int sockfd = socket_connect(controller[Control].host, controller[Control].port);

    if (sockfd == -1) {
        clear_controller(Control);
        return;
    }

    int bytes, sent, received, total;
    char message[1024], response[4096]; // allocate more space than required.
    sprintf(message, "GET /%d HTTP/1.0\r\n\r\n", Control);

    /* send the request */
    total = strlen(message);
    sent = 0;
    do {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0)
            DebugMessage(M64MSG_ERROR, "ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent += bytes;
    } while (sent < total);

    /* receive the response */
    memset(response, 0, sizeof(response));
    total = sizeof(response) - 1;
    received = 0;
    do {
        bytes = read(sockfd, response + received, total - received);
        if (bytes < 0)
            DebugMessage(M64MSG_ERROR, "ERROR reading response from socket");
        if (bytes == 0)
            break;
        received += bytes;
    } while (received < total);

    if (received == total)
        DebugMessage(M64MSG_ERROR, "ERROR storing complete response from socket");

/* print the response */
#ifdef _DEBUG
    DebugMessage(M64MSG_INFO, response);
#endif

    /* parse the http response */
    char *body = strtok(response, "\n");
    for (int i = 0; i < 5; i++)
        body = strtok(NULL, "\n");

    /* parse the body of the response */
    json_object *jsonObj = json_tokener_parse(body);
    
/* print the object */
#ifdef _DEBUG
    DebugMessage(M64MSG_INFO, json_object_to_json_string(jsonObj));
#endif

    controller[Control].buttons.R_DPAD = 
        json_object_get_int(json_object_object_get(jsonObj, "R_DPAD"));
    controller[Control].buttons.L_DPAD = 
        json_object_get_int(json_object_object_get(jsonObj, "L_DPAD"));
    controller[Control].buttons.D_DPAD = 
        json_object_get_int(json_object_object_get(jsonObj, "D_DPAD"));
    controller[Control].buttons.U_DPAD = 
        json_object_get_int(json_object_object_get(jsonObj, "U_DPAD"));
    controller[Control].buttons.START_BUTTON = 
        json_object_get_int(json_object_object_get(jsonObj, "START_BUTTON"));
    controller[Control].buttons.Z_TRIG = 
        json_object_get_int(json_object_object_get(jsonObj, "Z_TRIG"));
    controller[Control].buttons.B_BUTTON = 
        json_object_get_int(json_object_object_get(jsonObj, "B_BUTTON"));
    controller[Control].buttons.A_BUTTON = 
        json_object_get_int(json_object_object_get(jsonObj, "A_BUTTON"));
    controller[Control].buttons.R_CBUTTON = 
        json_object_get_int(json_object_object_get(jsonObj, "R_CBUTTON"));
    controller[Control].buttons.L_CBUTTON = 
        json_object_get_int(json_object_object_get(jsonObj, "L_CBUTTON"));
    controller[Control].buttons.D_CBUTTON = 
        json_object_get_int(json_object_object_get(jsonObj, "D_CBUTTON"));
    controller[Control].buttons.U_CBUTTON = 
        json_object_get_int(json_object_object_get(jsonObj, "U_CBUTTON"));
    controller[Control].buttons.R_TRIG = 
        json_object_get_int(json_object_object_get(jsonObj, "R_TRIG"));
    controller[Control].buttons.L_TRIG = 
        json_object_get_int(json_object_object_get(jsonObj, "L_TRIG"));
    controller[Control].buttons.X_AXIS = 
        json_object_get_int(json_object_object_get(jsonObj, "X_AXIS"));
    controller[Control].buttons.Y_AXIS = 
        json_object_get_int(json_object_object_get(jsonObj, "Y_AXIS"));

    // Mark the JSON object to be freed:
    json_object_put(jsonObj);
    
    close(sockfd);
}
