#include "common_net.h"
#include "thread_wrappers.h"
#include <vector>
#include <string>

#define PORT 5553

Mutex consoleMutex;

class ClientWorker : public Thread
{
    SOCKET m_socket;
    struct sockaddr_in m_addr;

public:
    ClientWorker(SOCKET s, struct sockaddr_in addr) : m_socket(s), m_addr(addr) {}

    void Run() override
    {
        char *str_ip = inet_ntoa(m_addr.sin_addr);

        consoleMutex.Lock();
        printf("[Server] Client connected: %s\n", str_ip);
        consoleMutex.Unlock();

        char buffer[1024];

        while (true)
        {
            int ret = recv(m_socket, buffer, sizeof(buffer) - 1, 0);

            if (ret <= 0)
            {
                break;
            }

            buffer[ret] = '\0';

            consoleMutex.Lock();
            printf("[%s] Says: %s\n", str_ip, buffer);
            consoleMutex.Unlock();

            std::string reply = "Echo: " + std::string(buffer);
            send(m_socket, reply.c_str(), reply.length(), 0);
        }

        closesocket(m_socket);

        consoleMutex.Lock();
        printf("[Server] Client disconnected: %s\n", str_ip);
        consoleMutex.Unlock();

        delete this;
    }
};

int main()
{
    common_init_handler();
    atexit(common_exit_handler);

    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSock == INVALID_SOCKET)
        return -1;

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr)) != 0)
    {
        printf("Bind failed\n");
        return -1;
    }

    if (listen(listenSock, 10) != 0)
    {
        printf("Listen failed\n");
        return -1;
    }

    printf("Echo Server is listening on port %d...\n", PORT);

    while (true)
    {
        sockaddr_in clientAddr = {};
        int len = sizeof(clientAddr);
        SOCKET clientSock = accept(listenSock, (sockaddr *)&clientAddr, &len);

        if (clientSock != INVALID_SOCKET)
        {
            ClientWorker *worker = new ClientWorker(clientSock, clientAddr);

            if (worker->Start())
            {
                worker->Detach();
            }
            else
            {
                printf("Failed to create thread for client!\n");
                delete worker;
                closesocket(clientSock);
            }
        }
    }

    closesocket(listenSock);
    return 0;
}