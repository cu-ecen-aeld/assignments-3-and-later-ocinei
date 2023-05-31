// include libraries used for sockets creation 
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <netdb.h>
#include <syslog.h>
#include <string.h>
#include <arpa/inet.h>

// define a socket routine 
int socketroutine() {
    // print creating socket
    printf("Creating socket\n");
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        printf("Socket creation failed\n");
        return 1;
    }
    // print creating socket address
    printf("Creating socket address\n");
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length;
    client_address_length = sizeof(client_address);

    server_address.sin_family = PF_INET;
    server_address.sin_port = htons(9000);
    // print binding socket
    int bind_status = bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));
    if(bind_status < 0)
    {
        printf("Bind failed\n");
        return 1;
    }
    // print listening socket
    printf("Listening socket\n");
    int listen_status = listen(socket_fd, 10);
    if(listen_status < 0)
    {
        printf("Listen failed\n");
        return 1;
    }

    int accept_status = accept(socket_fd, (struct sockaddr *)&client_address, &client_address_length);
    if(accept_status < 0)
    {
        printf("Accept failed\n");
        return 1;
    }

    char client_ipaddr[INET_ADDRSTRLEN];
    if(inet_ntop(PF_INET, &(client_address.sin_addr), client_ipaddr, INET_ADDRSTRLEN) == NULL)
    {
        printf("Failed to get ip address of peers\n");
        return 1;
    }

    openlog("aesdsocket", LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Accepted connection from %s", client_ipaddr);
    printf("Accepted connection from %s", client_ipaddr);

    FILE *fp = fopen("/var/tmp/aesdsocketdata", "w+");
    if(fp == NULL)
    {
        syslog(LOG_ERR, "Failed to open file: %s", strerror(errno));
        return 1;
    }

    char buffer[100];
    while(strstr(buffer, "\n") == NULL)
    {
        int receive_status = recv(accept_status, buffer, sizeof(buffer), 0);
        if(receive_status < 0)
        {
            printf("Receive failed\n");
            return 1;
        }
        fwrite(buffer, 1, sizeof(buffer), fp);
    }

    fclose(fp);

    fp = fopen("/var/tmp/aesdsocketdata", "r");
    while(fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        int send_status = send(accept_status, buffer, sizeof(buffer), 0);
        if(send_status < 0)
        {
            printf("Send failed\n");
            return 1;
        }
    }

    fclose(fp);

    close(socket_fd);
    syslog(LOG_INFO, "Closed connection from %s", inet_ntoa(client_address.sin_addr));
    closelog();

    return 0 ;
}

void signal_handler(int signum)
{
    if(signum == SIGINT || signum == SIGTERM)
    {
        printf("aesdsocketdata file is removed\n");
        syslog(LOG_INFO, "Caught signal, exiting");
        remove("/var/tmp/aesdsocketdata");
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    // while there is no SIGINT or SIGTERM signal, call the socket routine
    socketroutine();
    return 0;
}