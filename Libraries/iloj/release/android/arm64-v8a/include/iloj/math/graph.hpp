/*
* Copyright (c) 2025 InterDigital CE Patent Holdings SASU
* Licensed under the License terms of 5GMAG software (the "License").
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at https://www.5g-mag.com/license .
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/

namespace iloj::math
{
namespace graph
{
template<typename T>
auto Base<T>::getShortestDistanceFrom(NodeId source, std::vector<NodeId> &predecessor) const -> std::vector<T>
{
    std::size_t N = this->getNumberOfNodes();
    std::vector<T> distance(N, -1.);
    std::vector<NodeId> Q;

    predecessor.resize(N);
    std::fill(predecessor.begin(), predecessor.end(), N);

    distance[source] = 0.;

    Q.push_back(source);
    predecessor[source] = source;

    while (!Q.empty())
    {
        NodeId current = Q.front();

        std::pop_heap(
            Q.begin(), Q.end(), [&distance](NodeId id1, NodeId id2) { return (distance[id2] < distance[id1]); });
        Q.pop_back();

        for (std::size_t i = 0; i < this->getNeighbourhoodSize(current); i++)
        {
            auto l = this->getNeighbour(current, i);
            NodeId other = l.node();

            if (other == current)
            {
                continue;
            }

            T alt = distance[current] + l.weight();

            if (distance[other] < 0.)
            {
                distance[other] = alt;
                predecessor[other] = current;

                Q.push_back(other);
                std::push_heap(Q.begin(), Q.end(), [&distance](NodeId id1, NodeId id2) {
                    return (distance[id2] < distance[id1]);
                });
            }
            else if (alt < distance[other])
            {
                distance[other] = alt;
                predecessor[other] = current;
            }
        }
    }

    return distance;
}

template<typename T>
auto Base<T>::getShortestDistanceIf(NodeId source,
                                    const std::function<bool(NodeId)> &condition,
                                    std::vector<NodeId> &predecessor,
                                    const std::function<T(NodeId)> &heuristic) const -> std::pair<NodeId, T>
{
    NodeId N = this->getNumberOfNodes();
    std::vector<T> distance(N, -1);
    std::vector<T> score(N, -1);
    std::vector<NodeId> Q;

    predecessor.resize(N);
    std::fill(predecessor.begin(), predecessor.end(), N);

    distance[source] = 0.;
    score[source] = heuristic(source);

    Q.push_back(source);
    predecessor[source] = source;

    while (!Q.empty())
    {
        NodeId current = Q.front();

        if (condition(current))
        {
            return std::pair<NodeId, T>(current, distance[current]);
        }

        std::pop_heap(Q.begin(), Q.end(), [&score](NodeId id1, NodeId id2) { return (score[id2] < score[id1]); });
        Q.pop_back();

        for (NodeId i = 0; i < this->getNeighbourhoodSize(current); i++)
        {
            auto l = this->getNeighbour(current, i);
            NodeId other = l.node();

            if (other == current)
            {
                continue;
            }

            T alt = distance[current] + l.weight();

            if (distance[other] < 0)
            {
                distance[other] = alt;
                score[other] = alt + heuristic(other);

                predecessor[other] = current;

                Q.push_back(other);
                std::push_heap(
                    Q.begin(), Q.end(), [&score](NodeId id1, NodeId id2) { return (score[id2] < score[id1]); });
            }
            else if (alt < distance[other])
            {
                distance[other] = alt;
                score[other] = alt + heuristic(other);

                predecessor[other] = current;
            }
        }
    }

    return std::pair<NodeId, T>(source, -1);
}

template<typename T>
auto Base<T>::getShortestDistanceBetween(NodeId source,
                                         NodeId target,
                                         std::vector<NodeId> &predecessor,
                                         const std::function<T(NodeId)> &heuristic) const -> T
{
    return getShortestDistanceIf(
               source, [target](NodeId nodeId) { return (nodeId == target); }, predecessor, heuristic)
        .second;
}

template<typename T>
auto Base<T>::getPathsBetween(NodeId src, NodeId target) const -> std::vector<std::pair<T, std::vector<NodeId>>>
{
    NodeId N = this->getNumberOfNodes();
    std::vector<std::pair<T, std::vector<NodeId>>> out;
    std::queue<NodeId> Q_id;
    std::queue<T> Q_weight;
    std::queue<std::vector<NodeId>> Q_predecessor;

    Q_id.push(src);

    Q_weight.push(0);

    Q_predecessor.push(std::vector<NodeId>(N, N + 1));
    Q_predecessor.front()[src] = src;

    while (!Q_id.empty())
    {
        NodeId current = Q_id.front();

        if (current != target)
        {
            for (std::size_t i = 0; i < this->getNeighbourhoodSize(current); i++)
            {
                auto l = this->getNeighbour(current, i);
                NodeId other = l.node();

                if (other == current)
                {
                    continue;
                }
                if (Q_predecessor.front()[other] == (N + 1))
                {
                    Q_id.push(other);

                    Q_weight.push(Q_weight.front() + l.weight());

                    Q_predecessor.push(Q_predecessor.front());
                    Q_predecessor.back()[other] = current;
                }
            }
        }
        else
        {
            out.push_back(std::make_pair(Q_weight.front(),
                                         getPredecessorToPathRepresentation(Q_predecessor.front(), src, target)));
        }

        Q_id.pop();
        Q_weight.pop();
        Q_predecessor.pop();
    }

    return out;
}

template<typename T>
void Base<T>::growWhile(NodeId seed,
                        const std::function<bool(NodeId)> &condition,
                        const std::function<void(NodeId)> &onTrue,
                        const std::function<void(NodeId)> &onFalse) const
{
    std::size_t N = this->getNumberOfNodes();
    std::vector<std::uint8_t> visited(N, 0);
    std::queue<NodeId> Q;

    Q.push(seed);

    while (!Q.empty())
    {
        NodeId current = Q.front();

        visited[current] = 1;

        if (condition(current))
        {
            onTrue(current);

            for (std::size_t i = 0; i < this->getNeighbourhoodSize(current); i++)
            {
                auto l = this->getNeighbour(current, i);
                NodeId other = l.node();

                if (current == other)
                {
                    continue;
                }
                if (visited[other] == 0U)
                {
                    Q.push(other);
                }
            }
        }
        else
        {
            onFalse(current);
        }

        Q.pop();
    }
}

template<typename T>
auto Base<T>::getBottleneckDistanceFrom(NodeId source, std::vector<NodeId> &predecessor) const -> std::vector<T>
{
    std::size_t N = this->getNumberOfNodes();
    std::vector<T> distance(N, -1.);
    std::vector<NodeId> Q;

    predecessor.resize(N);
    std::fill(predecessor.begin(), predecessor.end(), N);

    distance[source] = std::numeric_limits<T>::max();

    Q.push_back(source);
    predecessor[source] = source;

    while (!Q.empty())
    {
        NodeId current = Q.front();

        std::pop_heap(
            Q.begin(), Q.end(), [&distance](NodeId id1, NodeId id2) { return (distance[id2] < distance[id1]); });
        Q.pop_back();

        for (std::size_t i = 0; i < this->getNeighbourhoodSize(current); i++)
        {
            auto l = this->getNeighbour(current, i);
            NodeId other = l.node();

            if (other == current)
            {
                continue;
            }

            T alt = std::min(distance[current], l.weight());

            if (distance[other] < 0.)
            {
                distance[other] = alt;
                predecessor[other] = current;

                Q.push_back(other);
                std::push_heap(Q.begin(), Q.end(), [&distance](NodeId id1, NodeId id2) {
                    return (distance[id2] < distance[id1]);
                });
            }
            else if (distance[other] < alt)
            {
                distance[other] = alt;
                predecessor[other] = current;
            }
        }
    }

    return distance;
}

template<typename T>
auto Base<T>::getMiniMaxDistanceFrom(NodeId source, std::vector<NodeId> &predecessor) const -> std::vector<T>
{
    std::size_t N = this->getNumberOfNodes();
    std::vector<T> distance(N, -1.);
    std::vector<NodeId> Q;

    predecessor.resize(N);
    std::fill(predecessor.begin(), predecessor.end(), N);

    distance[source] = 0.;

    Q.push_back(source);
    predecessor[source] = source;

    while (!Q.empty())
    {
        NodeId current = Q.front();

        std::pop_heap(
            Q.begin(), Q.end(), [&distance](NodeId id1, NodeId id2) { return (distance[id1] < distance[id2]); });
        Q.pop_back();

        for (std::size_t i = 0; i < this->getNeighbourhoodSize(current); i++)
        {
            auto l = this->getNeighbour(current, i);
            NodeId other = l.node();

            if (other == current)
            {
                continue;
            }

            T alt = std::max(distance[current], l.weight());

            if (distance[other] < 0.)
            {
                distance[other] = alt;
                predecessor[other] = current;

                Q.push_back(other);
                std::push_heap(Q.begin(), Q.end(), [&distance](NodeId id1, NodeId id2) {
                    return (distance[id1] < distance[id2]);
                });
            }
            else if (alt < distance[other])
            {
                distance[other] = alt;
                predecessor[other] = current;
            }
        }
    }

    return distance;
}

template<typename G>
auto getKruskalMinimumSpanningTree(const G &g) -> BuiltIn::Sparse<typename G::weight_type>
{
    std::size_t N = g.getNumberOfNodes();
    BuiltIn::Sparse<typename G::weight_type> h(N);
    UnionFind uf(N);
    std::vector<Edge<typename G::weight_type>> edges;
    std::size_t E = 0;

    for (std::size_t i = 0; i < g.getNumberOfNodes(); i++)
    {
        for (std::size_t j = 0; j < g.getNeighbourhoodSize(i); j++)
        {
            auto l = g.getNeighbour(i, j);

            if (i < l.node())
            {
                edges.push_back(Edge<typename G::weight_type>(i, l.node(), l.weight()));
            }
        }
    }

    std::make_heap(edges.begin(),
                   edges.end(),
                   [](const Edge<typename G::weight_type> &e1, const Edge<typename G::weight_type> &e2) {
                       return (e2.weight() < e1.weight());
                   });

    while (!edges.empty())
    {
        const Edge<typename G::weight_type> &e = edges.front();

        if (uf.find(e.nodes().first) != uf.find(e.nodes().second))
        {
            h.addEdge(e, LinkType::Undirected);
            uf.merge(e.nodes().first, e.nodes().second);
            if ((N - 1) < ++E)
            {
                break;
            }
        }

        std::pop_heap(edges.begin(),
                      edges.end(),
                      [](const Edge<typename G::weight_type> &e1, const Edge<typename G::weight_type> &e2) {
                          return (e2.weight() < e1.weight());
                      });
        edges.pop_back();
    }

    return h;
}

} // namespace graph

} // namespace iloj::math
