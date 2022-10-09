#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include "utils.hpp"
#include <vector>
#include <limits>
#include <queue>
#include <map>

namespace game {
    template<typename Type> class GraphNode {
    public:
        using Edge = std::pair<GraphNode<Type>*, double>;
        using ConstEdge = std::pair<GraphNode<Type>* const, double>;
        using Edges = std::unordered_map<GraphNode<Type>*, double>;
        using Neighbors = std::unordered_set<GraphNode<Type>*>;
    protected:
        Edges edges;
        Type* value;
    public:
        GraphNode(const Type* initValue) : value{initValue} {}
        GraphNode(const Type& initValue) : value{new Type(initValue)} {}
        ~GraphNode() {
            for (const Edge& node : this->edges) node.first->unlink(this, false);
        }

        bool operator == (const GraphNode<Type>& other) {
            return this->value == other.getValue();
        }

        bool operator != (const GraphNode<Type>& other) {
            return this->value != other.getValue();
        }

        void setValue(Type* value) {
            this->value = value;
        }

        Type* getValue() {
            return this->value;
        }

        void link(GraphNode<Type>* other, double weight = 1.f, bool undirected = true) {
            this->edges.insert({other, weight});
            if (undirected) other->link(this, weight, false);
        }

        void unlink(GraphNode<Type>* other, bool undirected = true) {
            this->edges.erase(other);
            if (undirected) other->unlink(this, false);
        }

        void unlinkAll() {
            while (this->edges.size() > 0) (*this->edges.begin()).first->unlink(this);
        }

        double weight(GraphNode<Type>* other) {
            typename Edges::iterator i = this->edges.find(other);
            if (i == this->edges.end()) return std::numeric_limits<double>::max();
            else return i->second;
        }

        typename Edges::iterator begin() {
            return this->edges.begin();
        }

        typename Edges::iterator end() {
            return this->edges.end();
        }

        Neighbors getNeighbors() {
            Neighbors neighbors;
            for (ConstEdge& edge : this->edges) neighbors.insert(edge.first);
            return std::move(neighbors);
        }
    };

    template<typename Type> class Graph {
    public:
        using Node = GraphNode<Type>;
        using Nodes = std::unordered_set<Node*>;
    protected:
        Nodes nodes;
    public:
        Graph() {}
        ~Graph() {}

        typename Nodes::iterator begin() {
            return this->nodes.begin();
        }

        typename Nodes::iterator end() {
            return this->nodes.end();
        }

        typename Nodes::size_type size() {
            return this->nodes.size();
        }

        typename Nodes::iterator find(Node* node) {
            return this->nodes.find(node);
        }

        typename Nodes::iterator find(const Type& value) {
            return std::find_if(this->begin(), this->end(), [&](Node* node) -> bool {
                return value == *(node->getValue());
            });
        }

        std::pair<typename Nodes::iterator, bool> insert(Node* node) {
            return this->nodes.insert(node);
        }

        std::pair<typename Nodes::iterator, bool> insert(const Type* value) {
            return this->insert(new Node(value));
        }

        std::pair<typename Nodes::iterator, bool> insert(const Type& value) {
            return this->insert(new Node(value));
        }

        typename Nodes::iterator erase(typename Nodes::iterator i) {
            if (i != this->nodes.end()) {
                Node* value = *i;
                if (i == this->find(value)) value->unlinkAll();
            }
            return this->nodes.erase(i);
        }

        typename Nodes::iterator erase(const Type& value) {
            return this->erase(this->find(value));
        }

        typename Nodes::size_type erase(Node* node) {
            typename Nodes::size_type
                sizeBefore = this->nodes.size(),
                sizeAfter = this->nodes.erase(node);
            if (sizeBefore > sizeAfter) node->unlinkAll();
            return sizeAfter;
        }

        void clear() {
            for (Node* node : this->nodes) node->unlinkAll();
            this->nodes.clear();
        }

        std::vector<Node*> aStar(
            Node* origin,
            Node* target,
            const std::function<double(Node* a, Node* b)>& heuristic
        ) {
            using queue_type = std::pair<double, Node*>;
            std::function<bool(const queue_type& a, const queue_type& b)> compare = [&](
                const queue_type& a,
                const queue_type& b
            ) -> bool {
                return a.first < b.first;
            };
            std::vector<std::vector<Node*>> paths;
            std::priority_queue<
                queue_type,
                std::vector<queue_type>,
                decltype(compare)
            > openSet{compare};
            openSet.push({0, origin});
            std::unordered_set<Node*>
                tracking;
            tracking.insert(origin);
            std::map<Node*, Node*>
                cameFrom;
            std::map<Node*, double>
                gScore,
                fScore;

            gScore[origin] = 0;
            fScore[origin] = heuristic(origin, target);

            while (!openSet.empty()) {
                Node* current = openSet.top().second;
                if (current == target) {
                    std::vector<Node*> path{current};
                    typename std::map<Node*, Node*>::iterator index;
                    while ((index = cameFrom.find(current)) != cameFrom.end()) {
                        current = (*index).second;
                        path.push_back(current);
                    }
                    return path;
                }

                openSet.pop();
                tracking.erase(current);
                for (const typename Node::Edge& edge : *current) {
                    double tentative_gScore = gScore[current] + edge.second;
                    if (gScore.find(edge.first) == gScore.end() || tentative_gScore < gScore[edge.first]) {
                        cameFrom[edge.first] = current;
                        gScore[edge.first] = tentative_gScore;
                        fScore[edge.first] = tentative_gScore + heuristic(edge.first, target);
                        if (tracking.find(edge.first) == tracking.end()) {
                            openSet.push({tentative_gScore, edge.first});
                            tracking.insert(edge.first);
                        }
                    }
                }
            }

            return {};
        }
    };
};
