#include "common_net.h"
#include "../../Lab1/practical/include/socket_wrapper.h"
#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>

#define PORT 2121

int main()
{
    common_init_handler();
    UdpSocket serverSock;

    if (!serverSock.Bind(PORT))
    {
        printf("Failed to bind port %d\n", PORT);
        return -1;
    }

    printf("UDP File Server started on port %d\n", PORT);
    printf("Waiting for UDP clients...\n");

    while (true)
    {
        char buffer[1024];
        Address clientAddr;

        int ret = serverSock.Recv(buffer, sizeof(buffer) - 1, clientAddr);
        if (ret > 0)
        {
            buffer[ret] = '\0';
            std::string req(buffer);
            printf("\n[From %s]: %s\n", clientAddr.getIP().c_str(), req.c_str());

            if (req == "LIST")
            {
                std::string list = "--- Server directory ---\n";
                WIN32_FIND_DATAA findData;
                HANDLE hFind = FindFirstFileA("*", &findData);
                if (hFind != INVALID_HANDLE_VALUE)
                {
                    do
                    {
                        if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0)
                        {
                            list += findData.cFileName;
                            list += "\n";
                        }
                    } while (FindNextFileA(hFind, &findData));
                    FindClose(hFind);
                }
                serverSock.Send(list.c_str(), list.length(), clientAddr);
            }
            else if (req.substr(0, 4) == "GET ")
            {
                std::string filename = req.substr(4);
                std::ifstream file(filename, std::ios::binary);

                if (file.is_open())
                {
                    file.seekg(0, std::ios::end);
                    long filesize = file.tellg();
                    file.seekg(0, std::ios::beg);

                    std::string sizeMsg = "SIZE " + std::to_string(filesize);
                    serverSock.Send(sizeMsg.c_str(), sizeMsg.length(), clientAddr);

                    char ack[10];
                    serverSock.Recv(ack, sizeof(ack), clientAddr);

                    char fileBuf[1024];
                    while (file.read(fileBuf, sizeof(fileBuf)) || file.gcount() > 0)
                    {
                        serverSock.Send(fileBuf, file.gcount(), clientAddr);
                        serverSock.Recv(ack, sizeof(ack), clientAddr);
                    }
                    printf("File '%s' successfully sent over UDP.\n", filename.c_str());
                    file.close();
                }
                else
                {
                    std::string errMsg = "ERROR File not found";
                    serverSock.Send(errMsg.c_str(), errMsg.length(), clientAddr);
                }
            }
        }
    }
    return 0;
}