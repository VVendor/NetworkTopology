#pragma once
#include <vector>
#include <string>
#include <tuple>

class NetworkTopology
{
public:
    NetworkTopology(int device_count, const std::vector<std::vector<int>> &ports);

    bool solve();

    std::vector<std::string> getResultTopology() const;

private:
    enum class Port : int
    {
        LEFT = 0,
        TOP = 1,
        RIGHT = 2,
        BOTTOM = 3
    };

    static constexpr int PORTS_PER_DEVICE = 4;

    int _device_count;
    int _open_ports_count;
    int _free_dev, _free_port_num;
    std::vector<std::vector<int>> _ports;
    std::vector<std::pair<int, int>> _open_ports;
    std::vector<std::tuple<int, int, int, int>> _solution_edges;

    bool compatibleDevices(int dev1, int p1, int dev2, int p2) const;
    bool checkDevicesFeasibility(const std::vector<std::tuple<int, int, int, int>> &connectionss) const;
    bool checkDevicesConnectivity(const std::vector<std::pair<int, int>> &edges) const;
    bool backtrack(int idx, std::vector<int> &match_to, std::vector<bool> &used_port, int free_port_idx);
    bool checkInputData();
};