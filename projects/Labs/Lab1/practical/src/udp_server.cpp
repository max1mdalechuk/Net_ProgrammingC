#include "common_net.h"
#include "socket_wrapper.h"

#define SERVER_PORT 5556

int main()
{
    common_init_handler();
    atexit(common_exit_handler);

    UdpSocket serverSock;
    if (!serverSock.IsValid())
        return -1;

    if (!serverSock.Bind(SERVER_PORT))
    {
        printf("Bind failed.\n");
        return -1;
    }

    printf("UDP Server started on port %d. Waiting for arrays...\n", SERVER_PORT);

    while (true)
    {
        Address sender;
        int count = 0;

        int ret = serverSock.Recv(&count, sizeof(int), sender);

        if (ret > 0)
        {
            printf("\n[From %s]: Incoming array size: %d\n", sender.getIP().c_str(), count);

            if (count > 0 && count < 1000)
            {
                std::vector<int> data(count);

                ret = serverSock.Recv(data.data(), count * sizeof(int), sender);

                if (ret > 0)
                {
                    printf("Data: ");
                    for (int num : data)
                    {
                        printf("%d ", num);
                    }
                    printf("\n");

                    char ack[] = "Received!";
                    serverSock.Send(ack, sizeof(ack), sender);
                }
            }
        }
    }

    return 0;
}