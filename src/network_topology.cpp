#include "network_topology.hpp"
#include <algorithm>
#include <functional>
#include <numeric>

NetworkTopology::NetworkTopology(int device_count, const std::vector<std::vector<int>> ports) : _device_count(device_count),
                                                                                                _open_ports_count(0), _free_dev(0), _free_port_num(0),
                                                                                                _ports(ports)
{
}

bool NetworkTopology::compatibleDevices(int dev1, int p1, int dev2, int p2) const
{
    if (dev1 == dev2)
    {
        return false;
    }

    if (p1 == static_cast<int>(Port::LEFT) && p2 == static_cast<int>(Port::RIGHT))
    {
        return true;
    }

    if (p1 == static_cast<int>(Port::RIGHT) && p2 == static_cast<int>(Port::LEFT))
    {
        return true;
    }

    if (p1 == static_cast<int>(Port::TOP) && p2 == static_cast<int>(Port::BOTTOM))
    {
        return true;
    }

    if (p1 == static_cast<int>(Port::BOTTOM) && p2 == static_cast<int>(Port::TOP))
    {
        return true;
    }

    return false;
}

bool NetworkTopology::checkDevicesFeasibility(const std::vector<std::tuple<int, int, int, int>> &connections) const
{
    std::vector<std::vector<int>> eq_x(_device_count), eq_y(_device_count);
    std::vector<std::pair<int, int>> lt_x, lt_y;
    for (auto &connection : connections)
    {
        int d1 = std::get<0>(connection), p1 = std::get<1>(connection), d2 = std::get<2>(connection), p2 = std::get<3>(connection);
        if (p1 == static_cast<int>(Port::LEFT) && p2 == static_cast<int>(Port::RIGHT))
        {
            lt_x.emplace_back(d1, d2);
            eq_y[d1].push_back(d2);
            eq_y[d2].push_back(d1);
        }
        else if (p1 == static_cast<int>(Port::RIGHT) && p2 == static_cast<int>(Port::LEFT))
        {
            lt_x.emplace_back(d2, d1);
            eq_y[d1].push_back(d2);
            eq_y[d2].push_back(d1);
        }
        else if (p1 == static_cast<int>(Port::TOP) && p2 == static_cast<int>(Port::BOTTOM))
        {
            lt_y.emplace_back(d2, d1);
            eq_x[d1].push_back(d2);
            eq_x[d2].push_back(d1);
        }
        else if (p1 == static_cast<int>(Port::BOTTOM) && p2 == static_cast<int>(Port::TOP))
        {
            lt_y.emplace_back(d1, d2);
            eq_x[d1].push_back(d2);
            eq_x[d2].push_back(d1);
        }
    }

    std::vector<int> parent_x(_device_count), parent_y(_device_count);

    iota(parent_x.begin(), parent_x.end(), 0);
    iota(parent_y.begin(), parent_y.end(), 0);

    std::function<int(std::vector<int> &, int)> find = [&](std::vector<int> &par, int x)
    {
        return par[x] == x ? x : par[x] = find(par, par[x]);
    };

    auto unite = [&](std::vector<int> &par, int a, int b)
    {
        a = find(par, a);
        b = find(par, b);

        if (a != b)
        {
            par[b] = a;
        }
    };

    for (int i = 0; i < _device_count; ++i)
    {
        for (const int j : eq_x[i])
        {
            unite(parent_x, i, j);
        }

        for (const int j : eq_y[i])
        {
            unite(parent_y, i, j);
        }
    }

    for (const auto &[a, b] : lt_x)
    {
        if (find(parent_x, a) == find(parent_x, b))
        {
            return false;
        }
    }

    for (const auto &[a, b] : lt_y)
    {
        if (find(parent_y, a) == find(parent_y, b))
        {
            return false;
        }
    }

    std::vector<std::vector<int>> graph_x(_device_count), graph_y(_device_count);

    for (const auto &[a, b] : lt_x)
    {
        int ra = find(parent_x, a), rb = find(parent_x, b);
        if (ra != rb)
        {
            graph_x[ra].push_back(rb);
        }
    }

    for (const auto &[a, b] : lt_y)
    {
        int ra = find(parent_y, a), rb = find(parent_y, b);
        if (ra != rb)
        {
            graph_y[ra].push_back(rb);
        }
    }

    auto hasCycle = [&](std::vector<std::vector<int>> &g, int n)
    {
        enum class State : char
        {
            UNVISITED = 0,
            VISITING = 1,
            VISITED = 2
        };

        std::vector<State> state(n, State::UNVISITED);
        std::function<bool(int)> dfs = [&](int u)
        {
            state[u] = State::VISITING;
            for (int v : g[u])
            {
                if (state[v] == State::VISITING)
                {
                    return true;
                }

                if (state[v] == State::UNVISITED && dfs(v))
                {
                    return true;
                }
            }

            state[u] = State::VISITED;
            return false;
        };

        for (int i = 0; i < n; ++i)
        {
            if (state[i] == State::UNVISITED && dfs(i))
            {
                return true;
            }
        }

        return false;
    };

    if (hasCycle(graph_x, _device_count) || hasCycle(graph_y, _device_count))
    {
        return false;
    }

    return true;
}

bool NetworkTopology::checkDevicesConnectivity(const std::vector<std::pair<int, int>> &edges) const
{
    if (_device_count == 0)
    {
        return true;
    }

    std::vector<std::vector<int>> adj(_device_count);

    for (const auto &e : edges)
    {
        int u = e.first, v = e.second;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    std::vector<bool> vis(_device_count, false);

    std::function<void(int)> dfs = [&](int u)
    {
        vis[u] = true;
        for (int v : adj[u])
        {
            if (!vis[v])
            {
                dfs(v);
            }
        }
    };

    dfs(0);

    for (int i = 0; i < _device_count; ++i)
    {
        if (!vis[i])
        {
            return false;
        }
    }

    return true;
}

bool NetworkTopology::backtrack(int idx, std::vector<int> &match_to, std::vector<bool> &used_port, int free_port_idx)
{
    while (idx < _open_ports_count && (used_port[idx] || idx == free_port_idx))
    {
        ++idx;
    }

    if (idx == _open_ports_count)
    {
        std::vector<std::pair<int, int>> edges;
        std::vector<std::tuple<int, int, int, int>> connections;
        for (int i = 0; i < _open_ports_count; ++i)
        {
            if (i == free_port_idx)
            {
                continue;
            }

            int j = match_to[i];

            if (i < j)
            {
                int dev1 = _open_ports[i].first, p1 = _open_ports[i].second;
                int dev2 = _open_ports[j].first, p2 = _open_ports[j].second;
                edges.emplace_back(dev1, dev2);
                connections.emplace_back(dev1, p1, dev2, p2);
            }
        }
        if (checkDevicesConnectivity(edges) && checkDevicesFeasibility(connections))
        {
            _solution_edges = std::move(connections);
            _free_dev = _open_ports[free_port_idx].first;
            _free_port_num = _open_ports[free_port_idx].second;
            return true;
        }

        return false;
    }
    for (int j = idx + 1; j < _open_ports_count; ++j)
    {
        if (used_port[j] || j == free_port_idx)
        {
            continue;
        }

        int dev1 = _open_ports[idx].first, p1 = _open_ports[idx].second;
        int dev2 = _open_ports[j].first, p2 = _open_ports[j].second;
        if (!compatibleDevices(dev1, p1, dev2, p2))
        {
            continue;
        }

        used_port[idx] = used_port[j] = true;
        match_to[idx] = j;
        match_to[j] = idx;

        if (backtrack(idx + 1, match_to, used_port, free_port_idx))
        {
            return true;
        }

        used_port[idx] = used_port[j] = false;
    }

    return false;
}

bool NetworkTopology::solve()
{
    if (!checkInputData())
    {
        return false;
    }

    _solution_edges.clear();
    if (_open_ports_count % 2 == 0)
    {
        return false;
    }

    for (int free_port_idx = 0; free_port_idx < _open_ports_count; ++free_port_idx)
    {
        int cnt0 = 0, cnt2 = 0, cnt1 = 0, cnt3 = 0;
        for (int i = 0; i < _open_ports_count; ++i)
        {
            if (i == free_port_idx)
                continue;
            int p = _open_ports[i].second;
            if (p == static_cast<int>(Port::LEFT))
                ++cnt0;
            else if (p == static_cast<int>(Port::RIGHT))
                ++cnt2;
            else if (p == static_cast<int>(Port::TOP))
                ++cnt1;
            else if (p == static_cast<int>(Port::BOTTOM))
                ++cnt3;
        }
        if (cnt0 != cnt2 || cnt1 != cnt3)
            continue;

        std::vector<int> match_to(_open_ports_count, -1);
        std::vector<bool> used_port(_open_ports_count, false);
        if (backtrack(0, match_to, used_port, free_port_idx))
        {
            return true;
        }
    }
    return false;
}

std::vector<std::string> NetworkTopology::getResultTopology() const
{
    if (_solution_edges.empty())
        return {};
    std::vector<std::vector<std::string>> result_topology(_device_count, std::vector<std::string>(PORTS_PER_DEVICE, "0"));
    for (const auto &connection : _solution_edges)
    {
        int d1 = std::get<0>(connection), p1 = std::get<1>(connection);
        int d2 = std::get<2>(connection), p2 = std::get<3>(connection);
        result_topology[d1][p1] = "S" + std::to_string(d2) + "." + std::to_string(p2);
        result_topology[d2][p2] = "S" + std::to_string(d1) + "." + std::to_string(p1);
    }
    result_topology[_free_dev][_free_port_num] = "M0";
    std::vector<std::string> result(_device_count);
    for (int i = 0; i < _device_count; ++i)
    {
        std::string line = std::to_string(i) + ":";
        for (int p = 0; p < PORTS_PER_DEVICE; ++p)
        {
            if (_ports[i][p] == 0)
            {
                line += " 0";
            }
            else
            {
                line += " " + result_topology[i][p];
            }
        }

        result[i] = line;
    }

    return result;
}

bool NetworkTopology::checkInputData()
{
    if (_device_count <= 0)
    {
        return false;
    }

    for (int i = 0; i < _device_count; ++i)
    {
        if (_ports[i].size() != PORTS_PER_DEVICE)
        {
            _open_ports.clear();
            return false;
        }

        for (int p = 0; p < PORTS_PER_DEVICE; ++p)
        {
            if (_ports[i][p] == 1)
            {
                _open_ports.emplace_back(i, p);
            }
        }
    }

    _open_ports_count = _open_ports.size();

    return true;
}