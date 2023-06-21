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
}

TEST(General, ComplexNode)
{
    Graphene<Node> graph;
    graph.addNode({0, 0});
    graph.addEdge({0, 0}, {1, 1});

    EXPECT_EQ(graph.size(), 1);
    EXPECT_EQ(graph.order(), 2);
    EXPECT_EQ(graph.nodeDegree({0, 0}), 1);
    EXPECT_EQ(graph.nodeDegree({1, 1}), 0);
    EXPECT_EQ(graph.adjacent({0, 0}, {1, 1}), true);
    EXPECT_EQ(graph.adjacent({1, 1}, {0, 0}), false);
}

int main(int argc, char**argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
