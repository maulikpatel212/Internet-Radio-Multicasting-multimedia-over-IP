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
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

/* site info structure */
struct site_info
{
  uint8_t site_name_size;
  char site_name[20];
  uint8_t site_desc_size;
  char site_desc[100];
  uint8_t radio_station_count;
};

/* station info structure */
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

  char *ip = "127.0.0.1";
  int port = 9011;

  int sock;
  struct sockaddr_in addr;
  socklen_t addr_size;
  char buffer[1024];
  char choice[1024];

  /* Creating socket */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror("Socket connection error");
    exit(1);
  }

  /* Building addr data structure */
  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = inet_addr(ip);

  /* Connecting socket To Server */
  connect(sock, (struct sockaddr *)&addr, sizeof(addr));

  /* Reseting info structures */
  bzero(&station1, sizeof(station1));
  bzero(&station2, sizeof(station2));
  bzero(&site1, sizeof(site1));
  bzero(&site2, sizeof(site2));

  /* Receive site info for station 1 */
  recv(sock, &(site1), sizeof(site1) + 1, 0);
  
  /* displaying site info for station 1 */
  printf("\n-------------------------------------------------------\n");
  printf("Station 1 site info\n");
  printf("\nSite name: %s", site1.site_name);
  printf("\nSite description: %s\n", site1.site_desc);

  /* Receive station info for station 1 */
  recv(sock, &(station1), sizeof(station1) + 1, 0);
  
  /* displaying station info for station 1 */
  printf("info port        : %d\n", station1.info_port);
  printf("Station Number   : %d\n", station1.radio_station_number);
  printf("Station name     : %s\n", station1.radio_station_name);
  printf("Multicast Address: %s\n", station1.multicast_address);
  printf("Data port        : %d\n", station1.data_port);
  printf("Bit rate         : %d kb/s\n", station1.bit_rate);

  /* Receive site info for station 2 */
  recv(sock, &(site2), sizeof(site2) + 1, 0);

  /* displaying site info for station 2 */  
  printf("\n-------------------------------------------------------\n");
  printf("Station 2 site info\n");
  printf("\nSite name: %s", site2.site_name);
  printf("\nSite description: %s\n", site2.site_desc);

  /* Receive station info for station 2 */
  recv(sock, &(station2), sizeof(station2) + 1, 0);

  /* displaying station info for station 2 */  
  printf("info port        : %d\n", station2.info_port);
  printf("Station Number   : %d\n", station2.radio_station_number);
  printf("Station name     : %s\n", station2.radio_station_name);
  printf("Multicast Address: %s\n", station2.multicast_address);
  printf("Data port        : %d\n", station2.data_port);
  printf("Bit rate         : %d kb/s", station2.bit_rate);
  printf("\n-------------------------------------------------------\n");

  printf("\n Enter your choice:");
  scanf("\n%s", &choice);

  write(sock, choice, sizeof(choice));

  if (strcmp(choice, "1") == 0)
  {
    system("gcc receiver.c -o receiver");  // innitiating receiver as a UDP client
    char string1[200] = "sudo ./receiver ";
    strcat(string1, "127.0.0.1 1");
    system(string1);
  }
  else
  {
    system("gcc receiver.c -o receiver");  // innitiating receiver as a UDP client
    char string1[200] = "sudo ./receiver ";
    strcat(string1, "127.0.0.1 2");
    system(string1);
  }
  return 0;
}
