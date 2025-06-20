/*
* Copyright (c) 2024 InterDigital R&D France
* Licensed under the License terms of 5GMAG software (the "License").
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at https://www.5g-mag.com/license .
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/

#pragma once

#include "matrix.h"
#include <functional>
#include <queue>

namespace iloj::math
{
namespace graph
{
using NodeId = std::size_t;
enum class LinkType
{
    Undirected,
    Directed
};
has_type_macro(weight_type);

template<typename T>
class Link
{
private:
    NodeId m_node;
    T m_weight;

public:
    Link(NodeId n, T w): m_node(n), m_weight(w) {}
    [[nodiscard]] auto node() const -> NodeId { return m_node; }
    [[nodiscard]] auto weight() const -> T { return m_weight; }
};

template<typename T>
class Edge
{
private:
    std::pair<NodeId, NodeId> m_nodes;
    T m_weight;

public:
    Edge(NodeId i, NodeId j, T w): m_nodes({i, j}), m_weight(w) {}
    [[nodiscard]] auto nodes() const -> const std::pair<NodeId, NodeId> & { return m_nodes; }
    [[nodiscard]] auto weight() const -> T { return m_weight; }
};

template<typename T>
class Base
{
public:
    using weight_type = T;

public:
    //! \brief  Return the number of nodes of the graph.
    [[nodiscard]] virtual auto getNumberOfNodes() const -> std::size_t = 0;
    //! \brief  Return the number of neighbours of node.
    [[nodiscard]] virtual auto getNeighbourhoodSize(NodeId node) const -> std::size_t = 0;
    //! \brief  Return the #id neighbour of node.
    [[nodiscard]] virtual auto getNeighbour(NodeId node, std::size_t id) const -> Link<T> = 0;
    //! \brief Returns an array containing the shortest distance from the source node to every nodes of the graph
    //! (Dijkstra's algorithm). An extra array is filled with the shortest path predecessor list.
    auto getShortestDistanceFrom(NodeId source, std::vector<NodeId> &predecessor) const -> std::vector<T>;
    //! \brief Returns the shortest distance from the source node until the given condition is satisfied (Dijkstra's
    //! algorithm). An extra array is filled with the shortest path predecessor list. An optional complementary A*
    //! heuristic may be provided as input.
    auto getShortestDistanceIf(
        NodeId source,
        const std::function<bool(NodeId)> &condition,
        std::vector<NodeId> &predecessor,
        const std::function<T(NodeId)> &heuristic = [](NodeId /*unused*/) { return 0.; }) const -> std::pair<NodeId, T>;
    //! \brief Returns the shortest distance from the source node to the specified target node (Dijkstra's algorithm).
    //! An extra array is filled with the shortest path predecessor list. An optional complementary A* heuristic may be
    //! provided as input.
    auto getShortestDistanceBetween(
        NodeId source,
        NodeId target,
        std::vector<NodeId> &predecessor,
        const std::function<T(NodeId)> &heuristic = [](NodeId /*unused*/) { return 0.; }) const -> T;
    //! \brief Returns all possible paths (without cycle) and associated total weight between src and target.
    auto getPathsBetween(NodeId src, NodeId target) const -> std::vector<std::pair<T, std::vector<NodeId>>>;
    //! \brief Region growing from seed while condition is satisfied. The optional callback functions onTrue and onFalse
    //! are called each time the condition is satisfied or not.
    void growWhile(
        NodeId seed,
        const std::function<bool(NodeId)> &condition,
        const std::function<void(NodeId)> &onTrue = [](NodeId /*unused*/) {},
        const std::function<void(NodeId)> &onFalse = [](NodeId /*unused*/) {}) const;
    //! \brief Returns an array containing the bottleneck distance from the source node to every nodes of the graph
    //! (modified Dijkstra's algorithm). An extra array is filled with the bottleneck path predecessor list.
    auto getBottleneckDistanceFrom(NodeId source, std::vector<NodeId> &predecessor) const -> std::vector<T>;
    //! \brief Returns an array containing the minimax distance from the source node to every nodes of the graph
    //! (modified Dijkstra's algorithm). An extra array is filled with the bottleneck path predecessor list.
    auto getMiniMaxDistanceFrom(NodeId source, std::vector<NodeId> &predecessor) const -> std::vector<T>;
};

//! \brief Given a source node, a target node and an array containing the list of predecessor to go from src to
//! target (typically returned by a Dijkstra algorithm), returns the path from src to target.
auto getPredecessorToPathRepresentation(const std::vector<NodeId> &predecessor, NodeId src, NodeId target)
    -> std::vector<NodeId>;

namespace BuiltIn
{
//! \brief Regular built-in graph
template<typename T>
class Sparse: public Base<T>
{
public:
    using weight_type = T;

private:
    std::vector<std::vector<Link<T>>> m_link;

public:
    explicit Sparse(std::size_t nb_nodes = 0): m_link(nb_nodes) {}
    void addNode() { m_link.push_back(std::vector<Link<T>>()); }
    [[nodiscard]] auto getNumberOfNodes() const -> std::size_t override { return m_link.size(); }
    [[nodiscard]] auto getNeighbourhoodSize(NodeId node) const -> std::size_t override { return m_link[node].size(); }
    [[nodiscard]] auto getNeighbour(NodeId node, std::size_t id) const -> Link<T> override { return m_link[node][id]; }
    [[nodiscard]] auto getNeighbourhood(NodeId id) const -> const std::vector<Link<T>> & { return m_link[id]; }
    //! \brief Connect vertices node and other and specify the connection weight.
    void connect(NodeId node, NodeId other, T weight, LinkType type)
    {
        m_link[node].push_back(Link<T>(other, weight));
        if (type == LinkType::Undirected)
        {
            m_link[other].push_back(Link<T>(node, weight));
        }
    }
    //! \brief Add the specified edge.
    void addEdge(const Edge<T> &e, LinkType type) { connect(e.nodes().first, e.nodes().second, e.weight(), type); }
};

//! \brief Dense built-in graph
template<typename T>
class Dense: public Base<T>
{
public:
    using weight_type = T;

private:
    Mat<T> m_weight;

public:
    Dense(Mat<T> weight): m_weight(std::move(weight)) {}
    auto getWeightMatrix() const -> const Mat<T> & { return m_weight; }
    [[nodiscard]] auto getNumberOfNodes() const -> std::size_t override { return m_weight.m(); }
    [[nodiscard]] auto getNeighbourhoodSize(NodeId /*unused*/) const -> std::size_t override { return m_weight.m(); }
    auto getNeighbour(NodeId node, NodeId id) const -> Link<T> override { return Link<T>(id, m_weight(node, id)); }
    //! \brief Connect vertices node and other and specify the connection weight.
    void connect(NodeId node, NodeId other, T weight, LinkType type)
    {
        m_weight(node, other) = weight;
        if (type == LinkType::Undirected)
        {
            m_weight(other, node) = weight;
        }
    }
};

} // namespace BuiltIn

//! \brief Returns the Minimum Spanning Tree for an undirected input graph (Kruskal's algorithm).
template<typename G>
auto getKruskalMinimumSpanningTree(const G &g) -> BuiltIn::Sparse<typename G::weight_type>;

//! \brief Union find implementation
class UnionFind
{
private:
    class Set
    {
    private:
        std::size_t m_parent = 0;
        std::size_t m_rank = 0;

    public:
        Set(std::size_t id): m_parent(id) {}
        [[nodiscard]] auto getParent() const -> std::size_t { return m_parent; }
        void setParent(std::size_t parent) { m_parent = parent; }
        [[nodiscard]] auto getRank() const -> std::size_t { return m_rank; }
        void increaseRank() { m_rank++; }
    };

private:
    std::vector<Set> m_set;
    std::size_t m_nbGroup = 0;

public:
    UnionFind(std::size_t n = 0): m_nbGroup(n)
    {
        for (std::size_t id = 0; id < n; id++)
        {
            m_set.emplace_back(id);
        }
    }
    void push()
    {
        m_set.emplace_back(m_set.size());
        m_nbGroup++;
    }
    [[nodiscard]] auto nbElement() const -> std::size_t { return m_set.size(); }
    [[nodiscard]] auto nbGroup() const -> std::size_t { return m_nbGroup; }
    auto find(std::size_t x) -> std::size_t
    {
        if (m_set[x].getParent() != x)
        {
            m_set[x].setParent(find(m_set[x].getParent())); // NOLINT
        }

        return m_set[x].getParent();
    }
    void merge(std::size_t x, std::size_t y)
    {
        std::size_t xRoot = find(x);
        std::size_t yRoot = find(y);

        if (xRoot != yRoot)
        {
            if (m_set[xRoot].getRank() < m_set[yRoot].getRank())
            {
                m_set[xRoot].setParent(yRoot);
            }
            else if (m_set[yRoot].getRank() < m_set[xRoot].getRank())
            {
                m_set[yRoot].setParent(xRoot);
            }
            else
            {
                m_set[yRoot].setParent(xRoot);
                m_set[xRoot].increaseRank();
            }

            m_nbGroup--;
        }
    }
};

} // namespace graph

} // namespace iloj::math

//! \brief Send the graph g to the stream os.
template<typename G, std::enable_if_t<iloj::math::graph::has_weight_type<G>::value, int> = 0>
auto operator<<(std::ostream &os, const G &g) -> std::ostream &
{
    for (std::size_t i = 0; i < g.getNumberOfNodes(); i++)
    {
        os << "n" << i << " -> ";
        for (std::size_t j = 0; j < g.getNeighbourhoodSize(i); j++)
        {
            auto l = g.getNeighbour(i, j);
            os << "n" << l.node() << "[" << l.weight() << "] ";
        }
        if (i != (g.getNumberOfNodes() - 1))
        {
            os << "\n";
        }
    }

    return os;
}

#include "graph.hpp"
