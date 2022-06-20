/* 
Computer Networks Internet Multicasting Radio over IP
Group- 13
Jinil chandarana - AU1940121
Maulikkumar Bhalani - AU1940206
Shrey Patel - AU1940110
Keyur Nagar - AU1940207
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define BUF_SIZE 64000
#define max(a, b) \
    ({ _typeof_ (a) _a = (a); \
       _typeof_ (b) _b = (b); \
     _a > _b ? _a : _b; })

int main(int argc, char *argv[])
{
    struct hostent *hp;
    struct sockaddr_in sin;
    char *mc_addr;
    char buffer[BUF_SIZE];
    int sock;
    int len, done = 0;;
    char filename[50];
    int mc_port;

    /* getting multicast addr from argument */
    mc_addr = argv[1];
    /* seeting according port */
    if (strcmp(argv[2], "1") == 0)
    {
        mc_port = 5445;
    }
    else
    {
        mc_port = 7000;
    }

    strcpy(filename, "download.mp4");

    /* translate mc_addr name into peer's IP address */
    hp = gethostbyname(mc_addr);
    if (!hp)
    {
        fprintf(stderr, "client: unknown mc_addr: %s\n", mc_addr);
        exit(1);
    }
    else
        printf("mc_addr %s found!\n", argv[1]);

    /* build address data structure */
    memset((char *)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
    sin.sin_port = htons(mc_port);

    /* create socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("client: socket");
        exit(1);
    }

    printf("Client will get data from to %s:%d.\n", argv[1], mc_port);
    printf("enter 'GET' to start radio \n");

    /* getting input from client */
    fgets(buffer, sizeof(buffer), stdin);
    buffer[BUF_SIZE - 1] = '\0';
    len = strlen(buffer) + 1;

    /* send GET message to server */
    if (sendto(sock, buffer, len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        perror("Client: sendto()");
        return 0;
    }

    /* Add code to receive unlimited data and either display the data
       or if specified by the user, store it in the specified file. */

    if (strcmp(buffer, "GET\n") == 0)
    {
        int buffer_size = 64000;
        char buffer[buffer_size];

        FILE *file;
        file = fopen(filename, "wb");
        if (file == NULL)
        {
            fprintf(stderr, "Error opening output file\n");
            fclose(file);
            exit(1);
        }

        /* getting info od received blocks */
        long int rec_size = 0;
        socklen_t sin_len = sizeof(sin);
        int curr = 0,i = 0;
        int lastsize = 0;
        
        while (1)
        {
            /* if connected to a station then send data */
            if (done == 0)
            {
                memset(&buffer, 0, sizeof(buffer));

                /* recv data from station */
                len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&sin, &sin_len);

                if ((len) < 0)
                {
                    printf("Error in receiving\n");
                }
                else
                {
                    printf("%d Receiving  %d\n", i, len);
                    fwrite(buffer, 1, len, file); /* start writing data in file */
                }
                
            }
        }

        /* desplaying recved data info */
        printf("Got %d datablocks of %d bytes each and 1 datablock with %d bytes.\n", curr - 1, buffer_size, lastsize);
        fclose(file);
    }

    /* connection termination */
    printf("Client terminating....\n");
    close(sock);
    printf("Client terminated.\n");
}

