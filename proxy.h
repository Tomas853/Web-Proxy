#define _CRT_SECURE_NO_WARNINGS
#include "csapp.h"

/* You won't lose style points for including this long line in your code */
static const char* user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

typedef struct {
    char url[2048];
    char hostname[1024];
    char path[2048];
    int port;
} Metadata;

void parse_url(Metadata* client_metadata) {
    char temp[2048];
    strcpy(temp,client_metadata->url);

    client_metadata->port = 80;
    strcpy(client_metadata->path, "/");

    const char* p = temp;

    if (strncmp(p, "http://", 7) == 0) {
        p += 7;
    }

    const char* slash = strchr(p, '/');
    if (slash) {
        strcpy(client_metadata->path, slash);
    }
    else {
        strcpy(client_metadata->path, '/');
    }

    char host_n_port[1024];  //extract host and port(maybe)
    if (slash) {
        strncpy(host_n_port, p, slash - p); //copy line from hostname till the firs '/' or path
        host_n_port[slash - p] = '\0';
    }
    else {
        strcpy(host_n_port, p); //copy all line starting from host name
    }

    char* colon = strchr(host_n_port, ':');
    if (colon) {
        *colon = '\0';
        strcpy(client_metadata->hostname, host_n_port);

        client_metadata->port = atoi(colon + 1);
    }
    else {
        strcpy(client_metadata->hostname, host_n_port);
    }
}

int handle_client(int clientSocket, Metadata* client_metadata, char* headers_out) {
    rio_t rio;
    Rio_readinitb(&rio, clientSocket);

    char reqline[1024];
    char method[16], url[2048], version[32];
    char header[1024];

    if (Rio_readlineb(&rio, reqline, 1024) <= 0) return -1;
    sscanf(reqline, "%s %s %s", method, url, version);
    
    if (strcmp(method, "GET") != 0) {
        fprintf(stderr, "Only GET supported \n");
        return -1;
    }

    headers_out[0] = '\0';
    while (Rio_readlineb(&rio, header, 1024) > 0) {
        if (!strcmp(header, "\r\n")) break;
        strcat(headers_out, header);
    }

    strcpy(client_metadata->url, url);
    parse_url(client_metadata);

    return 0;
}

int handle_server(const char* hostname, int port) {
    int serverSocket;
    struct addrinfo hints, *res;
    char port_str[16];

    sprintf(port_str, "%d", port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    Getaddrinfo(hostname, port_str, &hints, &res);

    serverSocket = Socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (serverSocket < 0) {
        Freeaddrinfo(res);
        return -1;
    }

    Connect(serverSocket, res->ai_addr, res->ai_addrlen);

    Freeaddrinfo(res);
    return serverSocket;
}

void build_http_request(char* outbuf, const char* hostname, const char* path, const char* client_headers) {
    
    //put all in one string
    sprintf(outbuf,
        "GET %s HTTP/1.0\r\n"
        "Host: %s\r\n"
        "%s"
        "Connection: close\r\n"
        "Proxy-Connection: close\r\n",
        path, hostname, user_agent_hdr);

    strcat(outbuf, client_headers);
    strcat(outbuf, "\r\n");
}

//send request to server(from proxy to server)
//buffer safe send
void send_to_server(int serverSocket,char* req_buffer) {
    int sent = 0;
    int left = strlen(req_buffer);
    int n;

    while (left > 0) {
        n = send(serverSocket, req_buffer + sent, left, 0);
        if (n <= 0) break;
        sent += n;
        left -= n;
    }
}

//receive and relay/send-back response to client (from server back to proxy back to client)
//buffer safe
void relay_server_response(int serverSocket,int clientSocket) {
    char buf[8192];
    int n;

    while ((n = recv(serverSocket, buf, sizeof(buf), 0)) > 0) {
        int sent = 0;
        while (sent < n) {
            int s = send(clientSocket, buf + sent, n - sent, 0);
            if (s <= 0) break;
            sent += s;
        }
    }
}