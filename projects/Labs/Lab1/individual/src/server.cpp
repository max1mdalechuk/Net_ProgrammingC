#include "common_net.h"
#include "protocol.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>

#pragma warning(disable : 4996)

#define DEFAULT_PORT 5553
#define CONNECTION_QUEUE 100

void error_msg(const char* msg)
{
    printf("[ERROR]: %s\n", msg);
}

void handle_connection(SOCKET client_socket, struct sockaddr_in* addr)
{
    char* str_addr = inet_ntoa(addr->sin_addr);
    printf("Client connected from %s!\n", str_addr);

    while (1)
    {
        ClientRequest req;
        memset(&req, 0, sizeof(req));
        int ret = recv(client_socket, (char*)&req, sizeof(ClientRequest), 0);

        if (ret <= 0)
        {
            break;
        }

        printf("Received array size: %d\n", req.count);

        if (req.count <= 0 || req.count > MAX_ARRAY_SIZE)
        {
            continue;
        }

        ServerResponse res;
        memset(&res, 0, sizeof(res));

        int offset = 0;

        for (int i = 0; i < req.count; i++)
        {
            if (offset >= 4000) break;

            int val = req.data[i];
            offset += sprintf(res.buf + offset, "%4d | ", val);

            for (int j = 0; j < val; j++)
            {
                offset += sprintf(res.buf + offset, "*");
            }
            offset += sprintf(res.buf + offset, "\n");
        }

        send(client_socket, (char*)&res, sizeof(ServerResponse), 0);
    }

    closesocket(client_socket);
    printf("Client %s disconnected.\n\n", str_addr);
}

void server()
{
    short port = DEFAULT_PORT;
    struct sockaddr_in saddr = {};

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET)
    {
        error_msg("Can't create socket");
        return;
    }

    if (bind(s, (sockaddr*)&saddr, sizeof(saddr)) != 0)
    {
        error_msg("Can't bind socket to address");
        return;
    }

    if (listen(s, CONNECTION_QUEUE) != 0)
    {
        error_msg("Can't listen for connections");
        return;
    }

    printf("Server started on port %d. Waiting for arrays...\n", port);

    while (1)
    {
        sockaddr_in client_addr = {};
        int len = sizeof(client_addr);
        SOCKET new_socket = accept(s, (sockaddr*)&client_addr, &len);

        if (new_socket == INVALID_SOCKET)
        {
            error_msg("Can't accept connection");
            continue;
        }

        handle_connection(new_socket, &client_addr);
    }

    closesocket(s);
}

int main()
{
    common_init_handler();
    server();
    common_exit_handler();
    return 0;
}