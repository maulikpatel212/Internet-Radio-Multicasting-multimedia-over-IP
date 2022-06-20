/* 
Computer Networks Internet Multicasting Radio over IP
Group- 13
Jinil chandarana - AU1940121
Maulikkumar Bhalani - AU1940206
Shrey Patel - AU1940110
Keyur Nagar - AU1940207
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>

#define data_port 7000
#define BUF_SIZE 4096
#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

int main(int argc, char *argv[])
{
    struct sockaddr_in sin;
    struct sockaddr_storage client_addr;
    char clientIP[INET_ADDRSTRLEN]; /* For IPv4 addresses */
    socklen_t client_addr_len;
    char buf[BUF_SIZE];
    int len;
    int sock;
    char *mc_addr;
    char *eptr;
    struct hostent *hp;
    int buffer_size = 8000;
    char buffer[buffer_size];

    /* Declarations for file(s) to be sent */
    char filename[50];

    /* For inserting delays, use nanosleep() struct timespec */
    struct timespec time1, time2;
    time1.tv_sec = 0;
    time1.tv_nsec = 0;
    strcpy(filename, "random.mp4");

    /* getting mc addr from argument */
    mc_addr = argv[1];
    buffer_size = 8000;
    time1.tv_nsec = strtol("64000", &eptr, 10);

    /* getting host info from ip */
    hp = gethostbyname(mc_addr);
    if (!hp)
    {
        fprintf(stderr, "server: unknown mc_addr %s\n", mc_addr);
        exit(1);
    }
    
    /* Create a socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("server: socket");
        exit(1);
    }

    /* build address data structure and bind to all local addresses*/
    memset((char *)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
    sin.sin_port = htons(data_port);

    /* binding socket */
    if ((bind(sock, (struct sockaddr *)&sin, sizeof(sin))) < 0)
    {
        perror("server: bind");
        exit(1);
    }
    else
    {
        /* Add code to parse IPv6 addresses */
        inet_ntop(AF_INET, &(sin.sin_addr), clientIP, INET_ADDRSTRLEN);
        printf("Server is listening at address %s:%d\n", clientIP, data_port);
    }

    printf("Client needs to send \"GET\" to receive the file %s\n", argv[1]);

    client_addr_len = sizeof(client_addr);

    /* time info structure */
    struct timeval tv1, tv2;

    /* Receive messages from clients*/
    while (len = recvfrom(sock, buf, sizeof(buf), 0,
                          (struct sockaddr *)&client_addr, &client_addr_len))
    {

        inet_ntop(client_addr.ss_family, &(((struct sockaddr_in *)&client_addr)->sin_addr),
                  clientIP, INET_ADDRSTRLEN);

        printf("Server got message from %s: %s [%d bytes]\n", clientIP, buf, len);

        /* Add code to send file if the incoming message is GET */
        char *video[5];

        /* Array of video names to stream */
        video[0] = "streamable_vid5.mp4";
        video[1] = "streamable_vid1.mp4";
        video[2] = "vid1.mp4";
        video[3] = "vid3.mp4";

        if (strcmp(buf, "GET\n") == 0)
        {
            while (1)
            {
                for (int i = 0; i < 2; i++) /* playing vids in sequence */
                {
                    
                    FILE *file;
                    file = fopen(video[i], "rb");

                    if (file == NULL)
                    {
                        fprintf(stderr, "Error opening input file\n");
                        fclose(file);
                        exit(1);
                    }

                    fseek(file, 0, SEEK_END);
                    int f_size = ftell(file); //size of file

                    long int read_size = 0;
                    int curr = 0;
                    int lastsize = 0;
                    
                    /* getiing time */
                    gettimeofday(&tv1, NULL); 

                    /* sending the file according to its size to stop appending itself */
                    while ((buffer_size * curr) < f_size)
                    {
                        fseek(file, (buffer_size * curr), SEEK_SET);
                        read_size = fread(buffer, 1, buffer_size, file);

                        /* sending frames to reciever */
                        if (sendto(sock, buffer, read_size, 0, (struct sockaddr *)&client_addr, client_addr_len) == -1)
                        {
                            printf("sento() error\n");
                            fclose(file);
                            exit(1);
                        }

                        /* desplaying sending datablock info */
                        printf("Sending datablock of size %ld from %ld - %ld\n", read_size, max(curr - 1, 0L) * buffer_size, ((max(curr - 1, 0L) * buffer_size) + read_size));
                        curr += 1;
                        lastsize = read_size;
                        memset(buffer, 0, buffer_size);
                        nanosleep(&time1, &time2);
                    }

                    printf("Sent %d datablocks of %d bytes each and 1 datablock with %d bytes.\n", curr - 1, buffer_size, lastsize);

                    /* getting datarate */
                    gettimeofday(&tv2, NULL);
                    float time_spent = (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (double)(tv2.tv_sec - tv1.tv_sec);
                    printf("Time required to send the file = %f seconds.\n", time_spent);
                    printf("Datarate = %f bytes/sec.\n", (double)(f_size / time_spent));

                    /* bye ot close connection */
                    strcpy(buffer, "BYE");
                    if (sendto(sock, buffer, buffer_size, 0, (struct sockaddr *)&client_addr, client_addr_len) == -1)
                    {
                        fclose(file);
                        exit(1);
                    }
                    fclose(file);
                }

                memset(buf, 0, sizeof(buf));
            }
        }
    }
}
