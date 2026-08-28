#include "common_net.h"
#include "../../Lab3/practical/include/thread_wrappers.h"
#include <iostream>
#include <fstream>
#include <string>

#define PORT 8080

Mutex consoleMutex;

class HttpWorker : public Thread
{
    SOCKET m_socket;
    struct sockaddr_in m_addr;

public:
    HttpWorker(SOCKET s, struct sockaddr_in addr) : m_socket(s), m_addr(addr) {}

    void Run() override
    {
        char buffer[4096];
        int ret = recv(m_socket, buffer, sizeof(buffer) - 1, 0);

        if (ret > 0)
        {
            buffer[ret] = '\0';
            char *client_ip = inet_ntoa(m_addr.sin_addr);

            consoleMutex.Lock();
            printf("\n====== HTTP REQUEST FROM %s ======\n", client_ip);
            printf("%s\n", buffer);
            printf("=========================================\n");
            consoleMutex.Unlock();

            std::ifstream file("index.html", std::ios::binary);
            if (file.is_open())
            {
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

                std::string response = "HTTP/1.1 200 OK\r\n";
                response += "Content-Type: text/html; charset=utf-8\r\n";
                response += "Content-Length: " + std::to_string(content.length()) + "\r\n";
                response += "Connection: close\r\n\r\n";
                response += content;

                send(m_socket, response.c_str(), response.length(), 0);
            }
            else
            {
                std::string not_found = "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found";
                send(m_socket, not_found.c_str(), not_found.length(), 0);
            }
        }
        closesocket(m_socket);
        delete this;
    }
};

int main()
{
    common_init_handler();
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr));
    listen(listenSock, 10);

    printf("HTTP Web Server started!\n");
    printf("Open your browser and go to: http://127.0.0.1:%d\n", PORT);

    while (true)
    {
        sockaddr_in clientAddr = {};
        int len = sizeof(clientAddr);
        SOCKET clientSock = accept(listenSock, (sockaddr *)&clientAddr, &len);

        if (clientSock != INVALID_SOCKET)
        {
            HttpWorker *worker = new HttpWorker(clientSock, clientAddr);
            if (worker->Start())
            {
                worker->Detach();
            }
            else
            {
                delete worker;
                closesocket(clientSock);
            }
        }
    }
    closesocket(listenSock);
    common_exit_handler();
    return 0;
}