#include <gtest/gtest.h>
#include "network_topology.hpp"

TEST(NetworkTopologyTest, Example1_Correct)
{
    const std::vector<std::vector<int>> ports = {
        {1, 0, 1, 1},
        {1, 0, 0, 0},
        {0, 1, 0, 0}};

    NetworkTopology nt(3, ports);
    EXPECT_TRUE(nt.solve());

    const std::vector<std::string> expected_result = {
        "0: M0 0 S1.0 S2.1",
        "1: S0.2 0 0 0",
        "2: 0 S0.3 0 0"};

    const auto result = nt.getResultTopology();

    EXPECT_TRUE(expected_result == result);
}

TEST(NetworkTopologyTest, Example2_InCorrect)
{
    std::vector<std::vector<int>> ports = {
        {1, 0, 1, 1},
        {1, 0, 0, 1},
        {0, 1, 1, 0}};
    NetworkTopology nt(3, ports);
    EXPECT_FALSE(nt.solve());
}

TEST(NetworkTopologyTest, Example3_Correct)
{
    const std::vector<std::vector<int>> ports = {
        {1, 0, 1, 0},
        {1, 0, 0, 0},
        {1, 0, 1, 0}};

    NetworkTopology nt(3, ports);
    EXPECT_TRUE(nt.solve());

    const std::vector<std::string> expected_result = {
        "0: M0 0 S2.0 0",
        "1: S2.2 0 0 0",
        "2: S0.2 0 S1.0 0"};

    const auto result = nt.getResultTopology();

    EXPECT_TRUE(expected_result == result);
}

TEST(NetworkTopologyTest, OddPortsButImpossible)
{
    std::vector<std::vector<int>> ports = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}};
    NetworkTopology nt(4, ports);
    EXPECT_FALSE(nt.solve());
}

TEST(NetworkTopologyTest, SquareCycle)
{
    std::vector<std::vector<int>> ports = {
        {1, 0, 1, 1},
        {1, 0, 0, 1},
        {1, 1, 0, 0},
        {0, 1, 1, 0}};
    NetworkTopology nt(4, ports);
    EXPECT_TRUE(nt.solve());

    const std::vector<std::string> expected_result = {
        "0: M0 0 S1.0 S3.1",
        "1 : S0 .2 0 0 S2 .1",
        "2 : S3 .2 S1 .3 0 0",
        "3 : 0 S0 .3 S2 .0 0"};

    const auto result = nt.getResultTopology();
}

TEST(NetworkTopologyTest, EvenOpenPorts)
{
    std::vector<std::vector<int>> ports = {
        {1, 0, 0, 0},
        {0, 1, 0, 0}};
    NetworkTopology nt(2, ports);
    EXPECT_FALSE(nt.solve());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}