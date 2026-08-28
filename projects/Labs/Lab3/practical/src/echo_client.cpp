#include "common_net.h"
#include <iostream>
#include <string>

#define SERVER_IP "127.0.0.1"
#define PORT 5553

int main(int argc, char *argv[])
{
    common_init_handler();
    atexit(common_exit_handler);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        printf("Error: Socket creation failed\n");
        return -1;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    const char *ip = (argc >= 2) ? argv[1] : SERVER_IP;
    serverAddr.sin_addr.s_addr = inet_addr(ip);

    printf("Connecting to %s:%d...\n", ip, PORT);

    if (connect(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) != 0)
    {
        printf("Error: Connection failed\n");
        closesocket(sock);
        return -1;
    }

    printf("Connected! Type text and press ENTER. Type 'exit' to quit.\n");
    printf("--------------------------------------------------------\n");

    char buffer[1024] = {};

    while (true)
    {
        std::string user_input;
        std::cout << "You> ";

        std::getline(std::cin, user_input);

        if (user_input == "exit")
        {
            break;
        }
        if (user_input.empty())
            continue;

        int sendResult = send(sock, user_input.c_str(), user_input.length(), 0);
        if (sendResult == SOCKET_ERROR)
        {
            printf("Error: Send failed\n");
            break;
        }

        int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0)
        {
            buffer[bytesReceived] = '\0'; 
            printf("%s\n", buffer);       
        }
        else if (bytesReceived == 0)
        {
            printf("Server closed connection.\n");
            break;
        }
        else
        {
            printf("Recv failed.\n");
            break;
        }
    }

    closesocket(sock);
    return 0;
}