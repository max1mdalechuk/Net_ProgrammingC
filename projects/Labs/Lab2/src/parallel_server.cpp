#include <limits.h>
#include "common_net.h"
#include "../include/protocol.h"

#define DEFAULT_PORT 5553

CRITICAL_SECTION consoleLock;
    
DWORD WINAPI ClientHandler(LPVOID lpParam)
{
    SOCKET client_socket = (SOCKET)lpParam;

    sockaddr_in addr;
    int addr_len = sizeof(addr);
    getpeername(client_socket, (sockaddr *)&addr, &addr_len);
    char *str_ip = inet_ntoa(addr.sin_addr);
    int port = ntohs(addr.sin_port);

    EnterCriticalSection(&consoleLock);
    printf("[Thread %lu] Client connected from %s:%d\n", GetCurrentThreadId(), str_ip, port);
    LeaveCriticalSection(&consoleLock);

    while (1)
    {
        ClientRequest req;
        memset(&req, 0, sizeof(req));

        int ret = recv(client_socket, (char *)&req, sizeof(ClientRequest), 0);
        if (ret <= 0)
            break;

        EnterCriticalSection(&consoleLock);
        printf("[%s:%d] Received array of %d elements.\n", str_ip, port, req.count);
        LeaveCriticalSection(&consoleLock);

        ServerResponse res;
        memset(&res, 0, sizeof(res));

        if (req.count <= 0 || req.count > MAX_ARRAY_SIZE)
        {
            res.isError = true;
            sprintf(res.message, "Error: Invalid count (1-%d).", MAX_ARRAY_SIZE);
            send(client_socket, (char *)&res, sizeof(ServerResponse), 0);
            continue;
        }

        int min_val = INT_MAX;
        int max_val = INT_MIN;
        long long sum = 0;

        for (int i = 0; i < req.count; i++)
        {
            int val = req.data[i];
            if (val < min_val)
            {
                min_val = val;
            }

            if (val > max_val)
            {
                max_val = val;
            }
            sum += val;
        }

        res.isError = false;
        res.min = min_val;
        res.max = max_val;
        res.avg = (double)sum / req.count;
        strcpy(res.message, "Success");

        send(client_socket, (char *)&res, sizeof(ServerResponse), 0);
    }

    closesocket(client_socket);

    EnterCriticalSection(&consoleLock);
    printf("[Thread %lu] Client %s:%d disconnected.\n", GetCurrentThreadId(), str_ip, port);
    LeaveCriticalSection(&consoleLock);

    return 0;
}

void server()
{
    short port = DEFAULT_PORT;
    struct sockaddr_in saddr = {};
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(port);

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    bind(s, (sockaddr *)&saddr, sizeof(saddr));
    listen(s, 100);

    InitializeCriticalSection(&consoleLock);

    printf("Parallel Server started on port %d.\n", port);

    while (1)
    {
        sockaddr_in client_addr = {};
        int len = sizeof(client_addr);

        SOCKET new_socket = accept(s, (sockaddr *)&client_addr, &len);
        if (new_socket == INVALID_SOCKET)
            continue;

        HANDLE hThread = CreateThread(
            NULL,
            0,
            ClientHandler,
            (LPVOID)new_socket,
            0,
            NULL);

        if (hThread)
        {
            CloseHandle(hThread);
        }
        else
        {
            closesocket(new_socket);
        }
    }

    DeleteCriticalSection(&consoleLock);
    closesocket(s);
}

int main()
{
    common_init_handler();
    server();
    common_exit_handler();
    return 0;
}