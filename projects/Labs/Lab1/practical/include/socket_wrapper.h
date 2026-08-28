#ifndef SOCKET_WRAPPER_H
#define SOCKET_WRAPPER_H

#include <vector>
#include <string>
#include "common_net.h"

// #pragma comment(lib, "ws2_32.lib")

class Address
{
    struct sockaddr_in m_addr;

public:
    Address()
    {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sin_family = AF_INET;
    }

    Address(const std::string &ip, short port)
    {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sin_family = AF_INET;
        m_addr.sin_port = htons(port);
        m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    }

    Address(struct sockaddr_in addr) : m_addr(addr) {}

    static bool Resolve(const std::string &domain, std::vector<std::string> &out_ips)
    {
        hostent *he = gethostbyname(domain.c_str());
        if (he == NULL)
            return false;

        for (char **pa = he->h_addr_list; *pa != NULL; ++pa)
        {
            in_addr addr;
            memcpy(&addr, *pa, he->h_length);
            out_ips.push_back(inet_ntoa(addr));
        }
        return true;
    }

    struct sockaddr *getStruct() const
    {
        return (struct sockaddr *)&m_addr;
    }

    int getSize() const
    {
        return sizeof(m_addr);
    }

    std::string getIP() const
    {
        return inet_ntoa(m_addr.sin_addr);
    }

    struct sockaddr_in *getRawPtr()
    {
        return &m_addr;
    }
};

class UdpSocket
{
    SOCKET m_socket;

public:
    UdpSocket()
    {
        m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_socket == INVALID_SOCKET)
        {
            printf("Socket creation failed\n");
        }
    }

    ~UdpSocket()
    {
        if (m_socket != INVALID_SOCKET)
        {
            closesocket(m_socket);
        }
    }

    bool Bind(short port)
    {
        struct sockaddr_in local_addr = {};
        local_addr.sin_family = AF_INET;
        local_addr.sin_port = htons(port);
        local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(m_socket, (struct sockaddr *)&local_addr, sizeof(local_addr)) == SOCKET_ERROR)
        {
            return false;
        }
        return true;
    }

    int Send(const void *data, int size, const Address &to)
    {
        return sendto(m_socket, (const char *)data, size, 0,
                      (struct sockaddr *)to.getStruct(), to.getSize());
    }

    int Recv(void *buffer, int size, Address &from)
    {
        socklen_t len = from.getSize();
        return recvfrom(m_socket, (char *)buffer, size, 0,
                        (struct sockaddr *)from.getRawPtr(), &len);
    }

    bool IsValid() const
    {
        return m_socket != INVALID_SOCKET;
    }
};

#endif