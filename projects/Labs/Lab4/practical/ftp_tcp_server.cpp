#include "common_net.h"
#include "../../Lab3/practical/include/thread_wrappers.h"
#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>

#define PORT 2121

Mutex consoleMutex;

std::string urlDecode(std::string str)
{
    std::string ret;
    char ch;
    int i, ii;
    for (i = 0; i < str.length(); i++)
    {
        if (str[i] == '%')
        {
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        }
        else
        {
            ret += str[i];
        }
    }
    return ret;
}

class FtpWorker : public Thread
{
    SOCKET m_socket;
    struct sockaddr_in m_addr;

public:
    FtpWorker(SOCKET s, struct sockaddr_in addr) : m_socket(s), m_addr(addr) {}

    void Run() override
    {
        char buffer[4096];
        int ret = recv(m_socket, buffer, sizeof(buffer) - 1, 0);

        if (ret > 0)
        {
            buffer[ret] = '\0';
            std::string request(buffer);
            char *client_ip = inet_ntoa(m_addr.sin_addr);

            std::string path = "/";
            size_t getPos = request.find("GET ");
            if (getPos != std::string::npos)
            {
                size_t startPath = getPos + 4;
                size_t endPath = request.find(" ", startPath);
                if (endPath != std::string::npos)
                {
                    path = request.substr(startPath, endPath - startPath);
                }
            }

            path = urlDecode(path);

            consoleMutex.Lock();
            printf("[FTP-Server] %s requested path: %s\n", client_ip, path.c_str());
            consoleMutex.Unlock();

            if (path == "/" || path == "")
            {
                std::string html = "<html><head><title>Мій FTP Сервер</title><meta charset='utf-8'></head>";
                html += "<body style='font-family: Arial; padding: 20px;'>";
                html += "<h2>Список файлів у директорії сервера:</h2><ul>";

                WIN32_FIND_DATAA findFileData;
                HANDLE hFind = FindFirstFileA("*", &findFileData);

                if (hFind != INVALID_HANDLE_VALUE)
                {
                    do
                    {
                        std::string fileName = findFileData.cFileName;
                        if (fileName != "." && fileName != "..")
                        {
                            html += "<li><a href='/" + fileName + "'>" + fileName + "</a></li>";
                        }
                    } while (FindNextFileA(hFind, &findFileData) != 0);
                    FindClose(hFind);
                }

                html += "</ul></body></html>";

                std::string response = "HTTP/1.1 200 OK\r\n";
                response += "Content-Type: text/html; charset=utf-8\r\n";
                response += "Content-Length: " + std::to_string(html.length()) + "\r\n";
                response += "Connection: close\r\n\r\n";
                response += html;

                send(m_socket, response.c_str(), response.length(), 0);
            }
            else
            {

                std::string filename = path.substr(1);

                std::ifstream file(filename, std::ios::binary);

                if (file.is_open())
                {
                    file.seekg(0, std::ios::end);
                    size_t fileSize = file.tellg();
                    file.seekg(0, std::ios::beg);

                    std::string headers = "HTTP/1.1 200 OK\r\n";
                    headers += "Content-Type: application/octet-stream\r\n";
                    headers += "Content-Disposition: attachment; filename=\"" + filename + "\"\r\n";
                    headers += "Content-Length: " + std::to_string(fileSize) + "\r\n";
                    headers += "Connection: close\r\n\r\n";

                    send(m_socket, headers.c_str(), headers.length(), 0);

                    char fileBuf[4096];
                    while (file.read(fileBuf, sizeof(fileBuf)) || file.gcount() > 0)
                    {
                        send(m_socket, fileBuf, file.gcount(), 0);
                    }
                    file.close();

                    consoleMutex.Lock();
                    printf(" -> File '%s' successfully sent.\n", filename.c_str());
                    consoleMutex.Unlock();
                }
                else
                {
                    std::string not_found = "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found";
                    send(m_socket, not_found.c_str(), not_found.length(), 0);

                    consoleMutex.Lock();
                    printf(" -> Error: File '%s' not found.\n", filename.c_str());
                    consoleMutex.Unlock();
                }
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

    printf("Advanced FTP-like Server started!\n");
    printf("Open your browser and go to: http://127.0.0.1:%d\n", PORT);

    while (true)
    {
        sockaddr_in clientAddr = {};
        int len = sizeof(clientAddr);
        SOCKET clientSock = accept(listenSock, (sockaddr *)&clientAddr, &len);

        if (clientSock != INVALID_SOCKET)
        {
            FtpWorker *worker = new FtpWorker(clientSock, clientAddr);
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