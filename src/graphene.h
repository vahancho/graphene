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

#endif // !__GRAPHENE_H__

