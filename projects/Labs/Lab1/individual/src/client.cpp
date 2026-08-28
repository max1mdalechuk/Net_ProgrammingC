#include "common_net.h"
#include "protocol.h"

#pragma warning(disable : 4996)
// #pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT 5553

void error_msg(const char* msg)
{
    printf("[ERROR]: %s\n", msg);
}

void client()
{
    char host[256] = "127.0.0.1";
    short port = DEFAULT_PORT;

    struct sockaddr_in saddr = {};
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr(host);
    saddr.sin_port = htons(port);

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET)
    {
        error_msg("Can't create socket");
        return;
    }

    if (connect(s, (sockaddr*)&saddr, sizeof(saddr)) != 0)
    {
        error_msg("Can't connect to server");
        return;
    }

    printf("Connected to server!\n");
    printf("Enter '0' as array size to exit.\n\n");

    while (1)
    {
        ClientRequest req;

        printf("\nEnter the size of array (0 to exit): ");

        if (scanf("%d", &req.count) != 1)
        {
            while (getchar() != '\n');
            continue;
        }

        if (req.count <= 0)
        {
            break;
        }
        else if (req.count > MAX_ARRAY_SIZE)
        {
            printf("Error: Max size is %d\n", MAX_ARRAY_SIZE);
            continue;
        }

        printf("Enter %d numbers: ", req.count);
        for (int i = 0; i < req.count; i++)
        {
            scanf("%d", &req.data[i]);
        }

        int ret = send(s, (char*)&req, sizeof(ClientRequest), 0);
        if (ret <= 0)
        {
            error_msg("Send failed\n");
            break;
        }

        ServerResponse res;
        memset(&res, 0, sizeof(res));
        ret = recv(s, (char*)&res, sizeof(ServerResponse), 0);
        if (ret <= 0)
        {
            error_msg("Server disconnected\n");
            break;
        }


        printf("\n[Server Response - Histogram]\n");
        printf("%s", res.buf);
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