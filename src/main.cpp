#include "network_topology.hpp"
#include <iostream>

int main()
{
    constexpr int ports_count = 4;

    int device_count;
    std::cin >> device_count;

    std::vector<std::vector<int>> ports(device_count, std::vector<int>(ports_count));
    for (int i = 0; i < device_count; ++i)
    {
        for (int p = 0; p < ports_count; ++p)
        {
            std::cin >> ports[i][p];
        }
    }

    NetworkTopology network_topology(device_count, ports);

    if (!network_topology.solve())
    {
        std::cout << "invalid" << std::endl;
    }
    else
    {
        const auto res_topology = network_topology.getResultTopology();
        for (const auto &device_info : res_topology)
        {
            std::cout << device_info << std::endl;
        }
    }

    return 0;
}