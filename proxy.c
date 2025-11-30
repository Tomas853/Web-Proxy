#include "proxy.h"

int main(int argc, char **argv)
{
    /* Check command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int proxySocket, clientSocket;
    struct sockaddr_in proxy_address;
    int proxyPort = atoi(argv[1]);


    proxySocket = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    Setsockopt(proxySocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    memset(proxy_address.sin_zero, 0, sizeof(proxy_address.sin_zero));  //set all bytes to zero before use. prevents unexpected behavior from uninitialiazed memory
    proxy_address.sin_family = AF_INET;
    proxy_address.sin_port = htons(proxyPort);
    proxy_address.sin_addr.s_addr = htonl(INADDR_ANY);


    Bind(proxySocket,(struct sockaddr *) & proxy_address, sizeof(proxy_address));
    Listen(proxySocket, SOMAXCONN);
    printf("Waiting for a connection on port: %d...\n", proxyPort);
    fflush(stdout);
   
    while (1) {
        //accept client connection
        socklen_t proxy_addr_len = sizeof(proxy_address);
        clientSocket = Accept(proxySocket,(struct sockaddr*) &proxy_address, &proxy_addr_len);
        if (clientSocket < 0) continue;
        
        //handle client - proxy request
        char client_headers[8192];
        Metadata client_metadata;
        if (handle_client(clientSocket, &client_metadata, &client_headers) < 0) {
            close(clientSocket);
            continue;
        }

        //connect to server
        int serverSocket = handle_server(client_metadata.hostname, client_metadata.port);
        if (serverSocket < 0) {
            unix_error("Could not connect to server \n");
            continue;
        }

        char outgoing_req[8192];
        build_http_request(outgoing_req, client_metadata.hostname, client_metadata.path, client_headers);
        printf("==============Forwarding request=================: \n%s\n============================\n", outgoing_req);

        //send and receive 
        send_to_server(serverSocket, outgoing_req);
        relay_server_response(serverSocket, clientSocket);
    }


    printf("%s", user_agent_hdr);
    close(proxySocket);
    return 0;
}
