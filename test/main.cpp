/**********************************************************************************
*  MIT License                                                                    *
*                                                                                 *
*  Copyright (c) 2020 Vahan Aghajanyan <vahancho@gmail.com>                       *
*                                                                                 *
*  Permission is hereby granted, free of charge, to any person obtaining a copy   *
*  of this software and associated documentation files (the "Software"), to deal  *
*  in the Software without restriction, including without limitation the rights   *
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      *
*  copies of the Software, and to permit persons to whom the Software is          *
*  furnished to do so, subject to the following conditions:                       *
*                                                                                 *
*  The above copyright notice and this permission notice shall be included in all *
*  copies or substantial portions of the Software.                                *
*                                                                                 *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       *
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  *
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  *
*  SOFTWARE.                                                                      *
***********************************************************************************/

#include "graphene.h"

#include <gtest/gtest.h>

struct Node
{
    int m_x;
    int m_y;

    bool operator<(const Node &other) const
    {
        if (m_x < other.m_x) {
            return true;
        } else if (m_x == other.m_x) {
            return m_y < other.m_y;
        }
        return false;
    }
};

TEST(General, Constructor)
{
    Graphene<int> graph;
}

TEST(General, AddEdge)
{
    Graphene<int> graph;
    graph.addEdge(1, 2);

    EXPECT_EQ(graph.size(), 1);
    EXPECT_EQ(graph.order(), 2);
}

TEST(General, AddEdgeUndirected)
{
    Graphene<int, GraphType::Undirected> graph;
    graph.addEdge(1, 2);

    EXPECT_EQ(graph.size(), 2);
    EXPECT_EQ(graph.order(), 2);
}

TEST(General, AddNode)
{
    Graphene<int> graph;
    graph.addNode(42);

    EXPECT_EQ(graph.size(), 0);
    EXPECT_EQ(graph.order(), 1);

    graph.addNode(0);
    EXPECT_EQ(graph.size(), 0);
    EXPECT_EQ(graph.order(), 2);

    graph.addEdge(0, 42);
    EXPECT_EQ(graph.size(), 1);
    EXPECT_EQ(graph.order(), 2);
}

TEST(General, ComplexNode)
{
    Graphene<Node> graph;
    const auto n = Node{-1, -1};
    graph.addNode(n);

    graph.addNode({0, 0});
    graph.addEdge({0, 0}, {1, 1});

    EXPECT_EQ(graph.size(), 1);
    EXPECT_EQ(graph.order(), 3);
    EXPECT_EQ(graph.nodeDegree({0, 0}), 1);
    EXPECT_EQ(graph.nodeDegree({1, 1}), 0);
    EXPECT_EQ(graph.adjacent({0, 0}, {1, 1}), true);
    EXPECT_EQ(graph.adjacent({1, 1}, {0, 0}), false);
}

TEST(General, ShortestPath)
{
    //
    // 1--2--5--8
    //  \     \/
    //   10---6---7
    //
    Graphene<int> graph;

    auto weightFunction = [](int x, int y) -> int {
        return std::abs(x - y);
    };

    // Non existent nodes
    auto path = graph.shortestPath(1, 2, weightFunction);
    EXPECT_EQ(path.size(), 0);

    graph.addEdge(1, 2);
    graph.addEdge(2, 5);
    graph.addEdge(5, 6);
    graph.addEdge(5, 8);
    graph.addEdge(8, 6);
    graph.addEdge(1, 10);
    graph.addEdge(10, 6);
    graph.addEdge(6, 7);

    // Non existent nodes
    path = graph.shortestPath(1, 222, weightFunction);
    EXPECT_EQ(path.size(), 0);

    EXPECT_EQ(graph.size(), 8);
    EXPECT_EQ(graph.order(), 7);

    path = graph.shortestPath(1, 6, weightFunction);
    EXPECT_EQ(path.size(), 4);
    EXPECT_EQ(path[0], 1);
    EXPECT_EQ(path[1], 2);
    EXPECT_EQ(path[2], 5);
    EXPECT_EQ(path[3], 6);

    graph.addNode(42);
    path = graph.shortestPath(1, 42, weightFunction);
    EXPECT_EQ(path.size(), 0); // There is no path from 1 to 42
}

TEST(General, ShortestPaths)
{
    //
    // 1--2--5--8
    //  \     \/
    //   10---6---7
    //
    Graphene<int> graph;

    auto weightFunction = [] (int x, int y) -> int {
        return std::abs(x - y);
    };

    graph.addEdge(1, 2);
    graph.addEdge(2, 5);
    graph.addEdge(5, 6);
    graph.addEdge(5, 8);
    graph.addEdge(8, 6);
    graph.addEdge(1, 10);
    graph.addEdge(10, 6);
    graph.addEdge(6, 7);

    EXPECT_EQ(graph.size(), 8);
    EXPECT_EQ(graph.order(), 7);

    auto paths = graph.shortestPaths(1, weightFunction);

    // All nodes should be connected
    EXPECT_EQ(paths.size(), graph.order());

    // 1->1
    EXPECT_EQ(paths[0].size(), 1);
    EXPECT_EQ(paths[0][0], 1);

    // 1->2
    EXPECT_EQ(paths[1].size(), 2);
    EXPECT_EQ(paths[1][0], 1);
    EXPECT_EQ(paths[1][1], 2);

    // 1->5
    EXPECT_EQ(paths[2].size(), 3);
    EXPECT_EQ(paths[2][0], 1);
    EXPECT_EQ(paths[2][1], 2);
    EXPECT_EQ(paths[2][2], 5);

    // 1->6
    EXPECT_EQ(paths[3].size(), 4);
    EXPECT_EQ(paths[3][0], 1);
    EXPECT_EQ(paths[3][1], 2);
    EXPECT_EQ(paths[3][2], 5);
    EXPECT_EQ(paths[3][3], 6);

    // 1->7
    EXPECT_EQ(paths[4].size(), 5);
    EXPECT_EQ(paths[4][0], 1);
    EXPECT_EQ(paths[4][1], 2);
    EXPECT_EQ(paths[4][2], 5);
    EXPECT_EQ(paths[4][3], 6);
    EXPECT_EQ(paths[4][4], 7);

    // 1->8
    EXPECT_EQ(paths[5].size(), 4);
    EXPECT_EQ(paths[5][0], 1);
    EXPECT_EQ(paths[5][1], 2);
    EXPECT_EQ(paths[5][2], 5);
    EXPECT_EQ(paths[5][3], 8);

    // 1->10
    EXPECT_EQ(paths[6].size(), 2);
    EXPECT_EQ(paths[6][0], 1);
    EXPECT_EQ(paths[6][1], 10);
}


int main(int argc, char**argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
