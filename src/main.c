/*
  raincoat - displays hourly weather forecasts
 
  Copyright (C) 2024 Shadman Saquib Abir 

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include "apikey.h"
#include "argsparse.h"
#include "dataparse.h"

int main(int argc, char *argv[])
{
    if (argc > 3) {
        fprintf(stderr, "Usage: %s [OPTION] [LOCATION]\n", argv[0]);
        return 1;
    }

    char *node = "api.weatherapi.com";
    char *service = "80";
    struct addrinfo hints;
    struct addrinfo *res, *resptr;
    int sockfd;
    char *request_fmt = "GET /v1/forecast.json?key=%s&q=%s&days=%s&aqi=no&alerts=no HTTP/1.0\r\n"
        "HOST:api.weatherapi.com\r\n\r\n";
    char request[1024];
    char *response;

    char days[2];
    char location[41];
    strcpy(days, "1");
    strcpy(location, "23.7527,90.3899");

    if (argc > 1) {
        int args_return = args_parse(argc, argv, location, days);
        if (args_return == 1) {
            return 0;
        }
        if (args_return == 2) {
            return 1;
        }
        if (args_return == 3) {
            return 4;
        }
    }

    sprintf(request, request_fmt, API_KEY, location, days);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_ADDRCONFIG;

    int server_error = getaddrinfo(node, service, &hints, &res);

    if (server_error) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(server_error));
        return 2;
    }

    for (resptr = res; resptr != NULL; resptr = resptr->ai_next) {
        sockfd = socket(resptr->ai_family, resptr->ai_socktype, resptr->ai_protocol);

        if (sockfd == -1) {
            continue;
        }

        if (connect(sockfd, resptr->ai_addr, resptr->ai_addrlen) != -1) {
            break;
        }

        close(sockfd);
    }

    freeaddrinfo(res);

    if (resptr == NULL) {
        fprintf(stderr, "Could not connect!\n");
        return 2;
    }

    struct timeval timeout;
    timeout.tv_sec = 15;
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("Failed to set timeout for receive operation\n");
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("Failed to set timeout for send operation\n");
    }

    int total = strlen(request);
    int sent = 0;
    int bytes;

    while (sent < total) {
        bytes = write(sockfd, request+sent, total-sent);

        if (bytes == -1) {
            perror("Error while sending request");
            close(sockfd);
            return 2;
        }

        if (!bytes) {
            break;
        }

        sent += bytes;
    }

    int buffer_size = atoi(days)*20480;
    response = (char *) calloc(buffer_size, sizeof(char));
    
    if (response == NULL) {
        perror("Error while allocating memory");
        close(sockfd);
        return 4;
    }
    
    total = buffer_size-1;
    int received = 0;

    while (received < total) {
        bytes = read(sockfd, response+received, total-received);

        if (bytes == -1) {
            perror("Error while receiving response");
            close(sockfd);
            free(response);
            return 2;
        }

        if (!bytes) {
            break;
        }

        received += bytes;
    }

    close(sockfd);

    if (received >= total) {
        fprintf(stderr, "The response buffer ran out of memory while receiving response from server\n");
        free(response);
        return 2;
    }

    char status[100];
    memset(status, 0, 100*sizeof(char));

    for (int i = 0; i < 99; i++) {
        if (response[i] == '\r' && response[i+1] == '\n') {
            break;
        }

        status[i] = response[i];
    }

    if (strcmp(status, "HTTP/1.1 200 OK") && 
       strcmp(status, "HTTP/1.1 400 Bad Request") && 
       strcmp(status, "HTTP/1.1 401 Unauthorized") && 
       strcmp(status, "HTTP/1.1 403 Forbidden")) {
        fprintf(stderr, "%s\n", status);
        free(response);
        return 2;
    }
    
    int body_start = 0;

    for (int i = 0; i < received-4; i++) {
        if (response[i] == '\r' && response[i+1] == '\n' && response[i+2] == '\r' && response[i+3] == '\n') {
            body_start = i + 4;
            break;
        }
    }

    char *response_body;
    response_body = response + body_start;

    int data_return = print_data(response_body);
    free(response);

    if (data_return == 1) {
        return 3;
    }

    return 0;
}
