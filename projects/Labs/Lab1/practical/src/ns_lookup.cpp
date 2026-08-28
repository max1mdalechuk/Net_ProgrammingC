#include "socket_wrapper.h"

int main(int argc, char *argv[])
{
    common_init_handler();
    atexit(common_exit_handler);

    std::string domain;

    if (argc >= 2)
    {
        domain = argv[1];
    }
    else
    {
        printf("Enter domain name: ");
        char buf[256];
        scanf("%s", buf);
        domain = buf;
    }

    printf("Resolving %s...\n", domain.c_str());

    std::vector<std::string> ips;

    if (Address::Resolve(domain, ips))
    {
        for (const auto &ip : ips)
        {
            printf("Address: %s\n", ip.c_str());
        }
    }
    else
    {
        printf("Error: Could not resolve domain.\n");
    }

    return 0;
}