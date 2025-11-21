#include "proxy.h"

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

int main(int argc, char **argv)
{
    /* Check command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int proxySocket, clientSocket, serverSocket;
    struct sockaddr_in proxy_address;
    int proxyPort = atoi(argv[1]);


    proxySocket = Socket(AF_INET, SOCK_STREAM, 0);
    
    memset(proxy_address.sin_zero, 0, sizeof(proxy_address.sin_zero));  //set all bytes to zero before use. prevents unexpected behavior from uninitialiazed memory
    proxy_address.sin_family = AF_INET;
    proxy_address.sin_port = htons(proxyPort);
    proxy_address.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(proxySocket,(struct sockaddr *) & proxy_address, sizeof(proxy_address));
    Listen(proxySocket, SOMAXCONN);
    printf("Waiting for a connection on port: %d...\n", proxyPort);
    fflush(stdout);
   
    ////accept client connection
    //clientSocket = Accept(proxySocket, NULL, NULL);

    ////create server socket
    //serverSocket = Socket(AF_INET, SOCK_STREAM, 0);

    printf("%s", user_agent_hdr);
    close(proxySocket);
    /*close(clientSocket);
    close(clientSocket);*/
    return 0;
}
