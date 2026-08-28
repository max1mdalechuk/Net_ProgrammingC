#include "common_net.h"
#include "socket_wrapper.h"

#define SERVER_PORT 5556
#define SERVER_IP "127.0.0.1"

int main(int argc, char *argv[])
{
    common_init_handler();
    atexit(common_exit_handler);

    UdpSocket clientSock;
    if (!clientSock.IsValid())
        return -1;

    std::string ip = (argc >= 2) ? argv[1] : SERVER_IP;
    Address serverAddr(ip, SERVER_PORT);

    while (true)
    {
        int count;
        printf("\nEnter count of numbers (0 to exit): ");
        if (scanf("%d", &count) != 1)
            break;
        if (count == 0)
            break;

        std::vector<int> numbers(count);
        printf("Enter %d numbers: ", count);
        for (int i = 0; i < count; ++i)
        {
            scanf("%d", &numbers[i]);
        }

        clientSock.Send(&count, sizeof(int), serverAddr);

        clientSock.Send(numbers.data(), count * sizeof(int), serverAddr);
        printf("Sent %d numbers to %s:%d\n", count, ip.c_str(), SERVER_PORT);

        
        char buf[256];
        Address temp;
        if (clientSock.Recv(buf, sizeof(buf), temp) > 0) {
            printf("Server replied: %s\n", buf);
        }
        
    }

    return 0;
}