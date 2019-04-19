/****************************************************************************
 * @author: Jordan396 <https://github.com/Jordan396/tcp-cJSON>              *
 *                                                                          *
 * This file was heavily adapted from the source code in:                   *
 *   "TCP/IP Sockets in C: Practical Guide for Programmers"                 *
 *   by Michael J. Donahoo and Kenneth L. Calvert                           *
 *   <http://cs.baylor.edu/~donahoo/practical/CSockets/textcode.html>       *
 *                                                                          *
 *   You should have received a copy of the MIT License when cloning this   *
 *   repository. If not, see <https://opensource.org/licenses/MIT>.         *
 ****************************************************************************/

/**
  * @file client.c
  * @author Jordan396
  * @date 19 April 2019
  * @brief client.c sends a JSON payload to a server.
  *
  * This file is to be compiled and executed on the client side.
  * 
  * Code is documented according to GNOME and Doxygen standards.
  * <https://developer.gnome.org/programming-guidelines/stable/c-coding-style.html.en>
  * <http://www.doxygen.nl/manual/docblocks.html>
  */

#include "client.h"

/* Function prototypes */

int main(int argc, char *argv[])
{
  /* Socket variables */
  int sock;                       /* Socket descriptor */
  struct sockaddr_in servAddress; /* ttweet server address */
  unsigned short servPort;        /* ttweet server port */
  char *servIP;                   /* Server IP address (dotted quad) */

  /* User variables */
  int userNumber;
  char *userMessage;

  /* Variables to handle transfer of data over TCP */
  cJSON *jobjToSend; /* JSON payload to be sent */

  if (argc != 5) /* Test for correct number of arguments */
  {
    die_with_error("Command not recognized!\nUsage: $./client <ServerIP> <ServerPort> <Number> \"Message\"");
  }

  servIP = argv[1];         /* Server IP address (dotted quad) */
  servPort = atoi(argv[2]); /* Use given port, if any */
  userNumber = atoi(argv[3]);     /* Parse number */
  userMessage = argv[4];    /* Parse message */

  /* Create a reliable, stream socket using TCP */
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    die_with_error("socket() failed");

  /* Construct the server address structure */
  memset(&servAddress, 0, sizeof(servAddress));    /* Zero out structure */
  servAddress.sin_family = AF_INET;                /* Internet address family */
  servAddress.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
  servAddress.sin_port = htons(servPort);          /* Server port */

  /* Establish the connection to the ttweet server */
  if (connect(sock, (struct sockaddr *)&servAddress, sizeof(servAddress)) < 0)
    die_with_error("connect() failed");

  /* Send data to server */
  jobjToSend = cJSON_CreateObject();
  cJSON_AddItemToObject(jobjToSend, "userMessage", cJSON_CreateString(userMessage));
  cJSON_AddItemToObject(jobjToSend, "userNumber", cJSON_CreateNumber(userNumber));
  send_payload(sock, jobjToSend);
  cJSON_Delete(jobjToSend);

  printf("Payload sent!\n");
  exit(1);
}