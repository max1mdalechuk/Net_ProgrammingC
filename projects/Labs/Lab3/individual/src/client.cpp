#include <stdio.h>
#include <string.h>
#include "common_net.h"
#include "../include/own_protocol.h"

#pragma warning(disable : 4996)

#define DEFAULT_PORT 5553
#define SERVER_IP "127.0.0.1"

void client()
{
    struct sockaddr_in saddr = {};
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    saddr.sin_port = htons(DEFAULT_PORT);

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(s, (sockaddr*)&saddr, sizeof(saddr)) != 0)
    {
        printf("Can't connect to server\n");
        return;
    }

    printf("Connected! Max array size is %d.\n", MAX_ARRAY_SIZE);

    while (1)
    {
        ClientRequest req;
        memset(&req, 0, sizeof(req));

        printf("\nEnter count of numbers (0 to exit): ");
        if (scanf("%d", &req.count) != 1 || req.count <= 0)
            break;

        if (req.count > MAX_ARRAY_SIZE)
        {
            printf("Error: Max size is %d\n", MAX_ARRAY_SIZE);
            continue;
        }

        printf("Enter %d numbers: ", req.count);
        for (int i = 0; i < req.count; i++)
        {
            scanf("%d", &req.data[i]);
        }

        if (send(s, (char*)&req, sizeof(ClientRequest), 0) <= 0)
            break;

        ServerResponse res;
        memset(&res, 0, sizeof(res));

        if (recv(s, (char*)&res, sizeof(ServerResponse), 0) <= 0)
            break;

        printf("\n>>> Server Reply:\n");
        if (res.isError)
        {
            printf("[FAILED]: %s\n", res.buf);
        }
        else
        {
            printf("[OK]: Histogram:\n%s\n", res.buf);
        }
    }
    closesocket(s);
}

int main()
{
    common_init_handler();
    client();
    common_exit_handler();
    return 0;
}