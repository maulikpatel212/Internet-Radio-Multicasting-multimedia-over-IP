/* 
Computer Networks Internet Multicasting Radio over IP
Group- 13
Jinil chandarana - AU1940121
Maulikkumar Bhalani - AU1940206
Shrey Patel - AU1940110
Keyur Nagar - AU1940207
*/


// Main server which will provide the station-list to the client

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

// Structure of site info
struct site_info
{
  uint8_t site_name_size;
  char site_name[20];
  uint8_t site_desc_size;
  char site_desc[100];
  uint8_t radio_station_count;
};

// Structure of station info
struct station_info
{
  uint8_t radio_station_number;
  char radio_station_name[50];
  char multicast_address[32];
  uint16_t data_port;
  uint16_t info_port;
  uint32_t bit_rate;
};

int main()
{
  struct station_info station1, station2;
  struct site_info site1, site2;

  char *ip = "127.0.0.1"; // Ip on which server is running
  int port = 9011; // Port on TCP server is listening

  int n, server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;
  char buffer[1024];

  /* creating server socket */
  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0)
  {
    perror("Socket connection error");
    exit(1);
  }
  printf("\n TCP server socket created.\n");

  /* Building addr data structure */
  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  /* binding the server */
  if (n = (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0)
  {
    perror("Binding Error");
    exit(1);
  }

  /* listen */
  listen(server_sock, 5);

  while (1)
  {
    /* accept connection from client */
    addr_size = sizeof(client_addr);
    if((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size))<0)
    {
        perror(" client socket not accepted\n");
    }
    printf(" connection accepted\n");
    
    /*Declaration of site info for station 1 */
    bzero(&site1, sizeof(site1));
    strcpy(site1.site_name, "www.redmirchi.com");
    strcpy(site1.site_desc, "Frequency: 98.7 MHz");

    /*Declaration of station info for station 1 */
    bzero(&station1, sizeof(station1));
    station1.radio_station_number = 1;
    strcpy(station1.multicast_address, "230.192.13.1");
    station1.data_port = 5445; // port for streaming radio station1
    station1.info_port = 9010; // port fro getting info of station1
    station1.bit_rate = 1068;  // bit rate of station1 
    strcpy(station1.radio_station_name, "RED MIRCHI");

    /*Declaration of site info for station 2 */
    bzero(&site2, sizeof(site2));
    strcpy(site2.site_name, "www.bigfm.com");
    strcpy(site2.site_desc, "Frequency: 92.7 MHz");

    /*Declaration of station info for station 2 */
    bzero(&station2, sizeof(station2));
    station2.radio_station_number = 2;
    strcpy(station2.multicast_address, "239.192.13.2");
    station2.data_port = 7000;  // port for streaming radio station2
    station2.info_port = 9010;  // port fro getting info of station2
    station2.bit_rate = 936;    // bit rate of station2 
    strcpy(station2.radio_station_name, "Radio 92.5");

    /*Sending structure of site info */
    send(client_sock, &(site1), sizeof(site1) + 1, 0); /* sending site1 info */
    send(client_sock, &(station1), sizeof(station1) + 1, 0); /* sending station1 info */

    send(client_sock, &(site2), sizeof(site2) + 1, 0); /* sending site2 info */
    send(client_sock, &(station2), sizeof(station2) + 1, 0); /* sending station2 info */

    while (1)
    {
      /* reseting buffer to avoide data collision */  
      bzero(buffer, 1024);
      /* getting station choice from client */
      read(client_sock, buffer, sizeof(buffer));

      /* running station in request */
      if (strcmp(buffer, "1") == 0)
      {
        system("gcc station1.c -o station1");
        char string1[200] = "./station1 ";
        strcat(string1, "127.0.0.1");
        system(string1);
      }
     
      else
      {
        system("gcc station2.c -o station2");
        char string1[200] = "./station2 ";
        strcat(string1, "127.0.0.1");
        system(string1);
      }
    }
  }
  return 0;
}
