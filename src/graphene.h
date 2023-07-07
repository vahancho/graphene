/**********************************************************************************
*  MIT License                                                                    *
*                                                                                 *
*  Copyright (c) 2023 Vahan Aghajanyan <vahancho@gmail.com>                       *
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

#ifndef __GRAPHENE_H__
#define __GRAPHENE_H__

#include <map>
#include <set>
#include <queue>
#include <vector>

enum class GraphType
{
    Directed,
    Undirected
};

//! Implements an abstract graph.
template<typename NodeType, GraphType GT = GraphType::Directed>
class Graphene
{
public:
    /// Adds new node.
    template<typename UR = NodeType>
    void addNode(UR && node);

    /// Adds an edge with the given \p tile and \p head
    template<typename UR = NodeType>
    void addEdge(UR && tile, UR && head);

    /// The order of a graph is its number of nodes
    size_t order() const;

    /// The size of a graph is its number of edges
    size_t size() const;

    /// The degree or valency of a vertex is the number of edges that are incident to it
    size_t nodeDegree(const NodeType &node) const;

    /// Two nodes \p x and \p y are adjacent if {x, y} is an edge
    bool adjacent(const NodeType &x, const NodeType &y) const;

    /// Returns the shortest path from the node \p from to the node \p to.
    /*!
        The function uses the Dijkstra algorithms for the shortest path between
        two nodes. The \p weightFunction is a custom function that returns weight
        that corresponds to two nodes (edge). For example, it can be a distance
        between two geometrical points.

        \param from The source node
        \param to The target node
        \param weightFunction A function that calculates a weight for an edge (between to nodes)
        \return A shortest path (list of nodes) between two nodes.
    */
    template <typename Func>
    std::vector<NodeType> shortestPath(const NodeType &from,
                                       const NodeType &to,
                                       Func weightFunction) const;

private:
    std::map<NodeType, std::set<NodeType>> m_adjacencyList;
};

////////////////////////////////////////////////////////////////////////////////
// Definition of the function templates
template<typename NodeType, GraphType GT>
template<typename UR>
void Graphene<NodeType, GT>::addNode(UR && node)
{
    m_adjacencyList[std::forward<UR>(node)];
}

template<typename NodeType, GraphType GT>
template<typename UR>
void Graphene<NodeType, GT>::addEdge(UR && tile, UR && head)
{
    auto &headNeighbours = m_adjacencyList[std::forward<UR>(head)];
    auto &tileNeighbours = m_adjacencyList[std::forward<UR>(tile)];

    // Link tile -> head
    tileNeighbours.emplace(std::forward<UR>(head));

    // C++17
    if constexpr (GT == GraphType::Undirected) {
        // Link head -> tile
        headNeighbours.emplace(std::forward<UR>(tile));
    }
}

template<typename NodeType, GraphType GT>
size_t Graphene<NodeType, GT>::order() const
{
    return m_adjacencyList.size();
}

template<typename NodeType, GraphType GT>
size_t Graphene<NodeType, GT>::size() const
{
    size_t r{0};
    for (const auto &i : m_adjacencyList) {
        r += i.second.size();
    }
    return r;
}

template<typename NodeType, GraphType GT>
size_t Graphene<NodeType, GT>::nodeDegree(const NodeType &node) const
{
    auto it = m_adjacencyList.find(node);
    if (it != m_adjacencyList.cend()) {
        return it->second.size();
    }
    return 0;
}

template<typename NodeType, GraphType GT>
bool Graphene<NodeType, GT>::adjacent(const NodeType &x, const NodeType &y) const
{
    auto it = m_adjacencyList.find(x);
    if (it != m_adjacencyList.cend()) {
        const auto &neibours = it->second;
        if (neibours.find(y) != neibours.cend()) {
            return true;
        }
    }
    return false;
}

template<typename NodeType, GraphType GT>
template <typename Func>
std::vector<NodeType> Graphene<NodeType, GT>::shortestPath(const NodeType &from,
                                                           const NodeType &to,
                                                           Func weight) const
{
    if (m_adjacencyList.find(from) == m_adjacencyList.cend() ||
        m_adjacencyList.find(to) == m_adjacencyList.cend()) {
        return {};
    }

    using WeightType = decltype(weight(from, to));
    using Pair = std::pair<WeightType, NodeType>;

    // A priority queue - the smallest element on top
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> queue;

    // The node's weight abstraction.
    class Weight
    {
    public:
        using Path = std::vector<NodeType>;

        Weight() = default;
        Weight(const NodeType &from) : m_path{from} {}
        bool infinite() const { return m_infinite; }
        void setWeight(WeightType weight) { m_weight= weight; m_infinite = false; }
        WeightType weight() const { return m_weight; }
        const Path &path() const { return m_path; }
        Path &path() { return m_path; }

    private:
        WeightType m_weight{};
        bool m_infinite{true};
        Path m_path;
    };

    std::map<NodeType, Weight> nodeWeights;

    // Initialize with the source node.
    queue.push({WeightType{}, from});
    nodeWeights.emplace(from, Weight{from});

    while (!queue.empty()) {
        // Make a copy!
        auto node = queue.top().second;
        queue.pop();
        const auto &nodeWeight = nodeWeights[node];

        auto it = m_adjacencyList.find(node);
        if (it != m_adjacencyList.cend()) {
            for (const auto &adjacent : it->second) {
                auto edgeWeight = weight(node, adjacent);

                // If there is shorted path to 'adjacent' through 'node'.
                auto &adjacentWeight = nodeWeights[adjacent];
                const auto totalWeight = nodeWeight.weight() + edgeWeight;

                if (adjacentWeight.infinite() || (adjacentWeight.weight() > totalWeight)) {
                    adjacentWeight.setWeight(totalWeight);

                    auto &adjacentPath = adjacentWeight.path();
                    auto &nodePath = nodeWeight.path();

                    adjacentPath.insert(adjacentPath.end(),
                                        nodePath.begin(),
                                        nodePath.end());
                    adjacentPath.emplace_back(adjacent);

                    queue.push({adjacentWeight.weight(), adjacent});
                }
            }
        }
    }

    return nodeWeights[to].path();

    /*
    printf("Node\t\tDistance\tPath\n");
    for (const auto &i : nodeWeights) {
        if (i.first == to) {
            const auto &path = i.second.path();
            ret = path;

            std::cout << i.first << "\t\t" << i.second.weight() << "\t\t";
            for (const auto &p : path) {
                std::cout << p << "-->";
            }
            std::cout << '\n';
            break;
        }
    }

    return ret;
    */
}

#endif // !__GRAPHENE_H__

