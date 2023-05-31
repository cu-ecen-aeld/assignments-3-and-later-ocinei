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


int socket_fd;

void signal_handler(int sig)
{
    if(sig == SIGINT || sig  == SIGTERM)
    {
        syslog(LOG_INFO, "Caught signal, exiting");
        close(socket_fd);
        printf("Closing socket\n");
        remove("/var/tmp/aesdsocketdata");
        printf("Removing file\n");
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    // run signal_handler function when SIGINT or SIGTERM is received
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // print creating socket
    printf("Creating socket\n");
    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        printf("Socket creation failed\n");
        return -1;
    }
    // print creating socket address
    printf("Creating socket address\n");
    struct sockaddr_in server_address, client_address;
    memset(&server_address, 0, sizeof(server_address));
    memset(&client_address, 0, sizeof(client_address));
    socklen_t client_address_length;
    client_address_length = sizeof(client_address);

    server_address.sin_family = PF_INET;
    server_address.sin_port = htons(9000);
    return -1;
    // print binding socket
    int bind_status = bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));
    if(bind_status < 0)
    {
        printf("Bind failed\n");
        return -1;
    }
    pid_t child_process;
    if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        child_process = fork();
        if (child_process < 0) {
            printf("Fork failed\n");
            return -1;
        }
        if (child_process > 0) {
            printf("Child process created\n");
                        //  print listening socket
            printf("Listening socket\n");
            int listen_status = listen(socket_fd, 10);
            if(listen_status < 0)
            {
                printf("Listen failed\n");
                return -1;
            }


            while(1) {

                int accept_status = accept(socket_fd, (struct sockaddr *)&client_address, &client_address_length);
                if(accept_status < 0)
                {
                    printf("Accept failed\n");
                    return -1;
                }
                char *client_ipaddr = (char *)malloc(INET_ADDRSTRLEN);
                if(inet_ntop(PF_INET, &(client_address.sin_addr), client_ipaddr, INET_ADDRSTRLEN) == NULL)
                {
                    printf("Failed to get ip address of peers\n");
                    return -1;
                }

                openlog("aesdsocket", LOG_PID|LOG_CONS, LOG_USER);
                syslog(LOG_INFO, "Accepted connection from %s", client_ipaddr);
                printf("Accepted connection from %s", client_ipaddr);

                FILE *fp = fopen("/var/tmp/aesdsocketdata", "a");
                if(fp == NULL)
                {
                    syslog(LOG_ERR, "Failed to open file: %s", strerror(errno));
                    return -1;
                }

                // make a very long very long buffer
                char buffer[30000];
                memset(buffer, 0, sizeof(buffer));
                ssize_t received_status = recv(accept_status, buffer, sizeof(buffer), 0);
                if (received_status == -1) {
                    perror("recv");
                } else {
                    buffer[received_status] = '\0';
                    printf("Received %ld bytes: %s\n", received_status, buffer);
                    size_t original_length = strlen(buffer);
                    size_t datalength = strcspn(buffer, "\n");
                    printf("Data length: %ld\n", datalength);
                    printf("Original length: %ld\n", original_length);
                    // append buffer data to file in new line
                    fwrite(&buffer[0], 1, datalength+1, fp);
                }
                fclose(fp);

                // reading from file, set a new buffer, get data to buffer and send; don't use fget
                FILE *fp1 = fopen("/var/tmp/aesdsocketdata", "r");
                if(fp1 == NULL)
                {
                    syslog(LOG_ERR, "Failed to open file: %s", strerror(errno));
                    return -1;
                }
                char buffer1[30000];
                memset(buffer1, 0, sizeof(buffer1));
                size_t read_status = fread(buffer1, 1, sizeof(buffer1), fp1);
                if(read_status == 0)
                {
                    syslog(LOG_ERR, "Failed to read file: %s", strerror(errno));
                    return -1;
                }
                fclose(fp1);
                // send data to client
                // if block for if read_status is very long 
                ssize_t send_status = send(accept_status, buffer1, read_status, 0);
                // print number of bytes sent 
                printf("Sent %ld bytes: ", send_status);
                if(send_status == -1)
                {
                    syslog(LOG_ERR, "Failed to send data: %s", strerror(errno));
                    return -1;
                }

                close(accept_status);
                // wait for 5 seconds
                sleep(5);
                syslog(LOG_INFO, "Closed connection from %s", inet_ntoa(client_address.sin_addr));
                closelog();
            }

            return 0;
        }
    }

    return 0;
}