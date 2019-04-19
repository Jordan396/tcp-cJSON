/****************************************************************************
 * @author: Jordan396 <https://github.com/Jordan396/trivial-twitter-v2>     *
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
  * @file ttweetsrv.c
  * @author Jordan396
  * @date 13 April 2019
  * @brief ttweetser handles upload/download requests from ttweetcli clients.
  *
  * This file is to be compiled and executed on the server side. For an overview of 
  * what this program does, visit <https://github.com/Jordan396/trivial-twitter-v2>.
  * 
  * Code is documented according to GNOME and Doxygen standards.
  * <https://developer.gnome.org/programming-guidelines/stable/c-coding-style.html.en>
  * <http://www.doxygen.nl/manual/docblocks.html>
  * 
  * ttweetsrc creates a persistent connection to a ttweetcli client. 
  * A new child process is created for each connection via fork().
  * The parent process ensures that only a maximum of MAX_CONC_CONN
  * connections are running concurrently at any time.
  * 

  */

#include "server.h"

/* Function prototypes */

/* functions to handle connections */
void handle_tcp_client(int clntSocket); /* TCP client handling function */

int main(int argc, char *argv[])
{
  int servSock;                    /* Socket descriptor for server */
  int clntSock;                    /* Socket descriptor for client */
  struct sockaddr_in echoServAddr; /* Local address */
  struct sockaddr_in echoClntAddr; /* Client address */
  unsigned short echoServPort;     /* Server port */
  unsigned int clntLen;            /* Length of client address data structure */

  if (argc != 2) /* Test for correct number of arguments */
  {
    fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
    exit(1);
  }

  echoServPort = atoi(argv[1]); /* First arg:  local port */

  /* Create socket for incoming connections */
  if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    die_with_error("socket() failed");

  /* Construct local address structure */
  memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
  echoServAddr.sin_family = AF_INET;                /* Internet address family */
  echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
  echoServAddr.sin_port = htons(echoServPort);      /* Local port */

  /* Bind to the local address */
  if (bind(servSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
    die_with_error("bind() failed");

  /* Mark the socket so it will listen for incoming connections */
  if (listen(servSock, MAX_PENDING) < 0)
    die_with_error("listen() failed");

  for (;;) /* Run forever */
  {
    /* Set the size of the in-out parameter */
    clntLen = sizeof(echoClntAddr);

    /* Wait for a client to connect */
    if ((clntSock = accept(servSock, (struct sockaddr *)&echoClntAddr,
                           &clntLen)) < 0)
      die_with_error("accept() failed");

    /* clntSock is connected to a client! */

    printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

    handle_tcp_client(clntSock);
  }
  /* NOT REACHED */
}

/** \copydoc handle_tcp_client */
void handle_tcp_client(int clntSocket)
{
  char objReceived[MAX_RESP_SIZE]; /* String to store response */
  int recvMsgSize;                /* Size of received message */

  cJSON *jobjReceived = cJSON_CreateObject();
  receive_response(clntSocket, objReceived);
  printf("Raw response: %s\n", objReceived);
  jobjReceived = cJSON_Parse(objReceived);
  printf("userNumber: %d\n", cJSON_GetObjectItemCaseSensitive(jobjReceived, "userNumber")->valueint);
  printf("userMessage: %s\n", cJSON_GetObjectItemCaseSensitive(jobjReceived, "userMessage")->valuestring);
  cJSON_Delete(jobjReceived);

  close(clntSocket); /* Close client socket */
}
