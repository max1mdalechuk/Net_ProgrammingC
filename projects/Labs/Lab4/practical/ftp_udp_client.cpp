#include "common_net.h"
#include "../../Lab1/practical/include/socket_wrapper.h"
#include <iostream>
#include <string>
#include <fstream>

#define SERVER_PORT 2121
#define SERVER_IP "127.0.0.1"

int main()
{
    common_init_handler();
    UdpSocket clientSock;
    Address serverAddr(SERVER_IP, SERVER_PORT);

    printf("UDP File Client started.\n");

    while (true)
    {
        printf("\n====================================\n");
        printf("Commands: LIST, GET <filename>, EXIT\n");
        printf("You> ");

        std::string cmd;
        std::getline(std::cin, cmd);

        if (cmd == "EXIT")
            break;
        if (cmd.empty())
            continue;

        clientSock.Send(cmd.c_str(), cmd.length(), serverAddr);

        if (cmd == "LIST")
        {
            char buf[4096];
            Address sender;
            int ret = clientSock.Recv(buf, sizeof(buf) - 1, sender);
            if (ret > 0)
            {
                buf[ret] = '\0';
                printf("\n%s\n", buf);
            }
        }
        else if (cmd.substr(0, 4) == "GET ")
        {
            char buf[1024];
            Address sender;

            int ret = clientSock.Recv(buf, sizeof(buf) - 1, sender);
            if (ret > 0)
            {
                buf[ret] = '\0';
                std::string resp(buf);

                if (resp.substr(0, 5) == "SIZE ")
                {
                    long filesize = std::stol(resp.substr(5));
                    printf("Downloading file of size %ld bytes...\n", filesize);

                    std::string filename = "downloaded_" + cmd.substr(4);
                    std::ofstream outfile(filename, std::ios::binary);

                    clientSock.Send("OK", 2, serverAddr);

                    long received = 0;
                    while (received < filesize)
                    {
                        ret = clientSock.Recv(buf, sizeof(buf), sender);
                        if (ret > 0)
                        {
                            outfile.write(buf, ret);
                            received += ret;

                            clientSock.Send("ACK", 3, serverAddr);
                        }
                    }
                    outfile.close();
                    printf("Success! File saved as '%s'\n", filename.c_str());
                }
                else
                {
                    printf("Server reply: %s\n", buf);
                }
            }
        }
        else
        {
            printf("Unknown command syntax.\n");
        }
    }
    return 0;
}