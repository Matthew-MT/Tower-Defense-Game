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
        using Edges = std::unordered_map<GraphNode<Type>*, double>;
    protected:
        Edges edges;
        Type* value;
    public:
        GraphNode(const Type* initValue) : value{initValue} {}
        GraphNode(Type initValue) : value{&initValue} {}
        ~GraphNode() {
            for (Edge& node : this->edges) node.first->unlink(this, false);
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

        Edges::iterator begin() {
            return this->edges.begin();
        }

        Edges::iterator end() {
            return this->edges.end();
        }

        double weight(GraphNode<Type>* other) {
            Edges::iterator i = this->edges.find(other);
            if (i == this->edges.end()) return std::numeric_limits<double>::max();
            else return i->second;
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

        Nodes::iterator begin() {
            return this->nodes.begin();
        }

        Nodes::iterator end() {
            return this->nodes.end();
        }

        Nodes::iterator find(Node* node) {
            return this->nodes.find(node);
        }

        Nodes::iterator find(const Type& value) {
            return std::find_if(this->begin(), this->end(), [&](const Node& node) -> bool {
                return value == node.getValue();
            });
        }

        Nodes::insert_return_type insert(const Type& value) {
            return this->nodes.insert(new Node(value));
        }

        Nodes::insert_return_type insert(Node* node) {
            return this->nodes.insert(node);
        }

        Nodes::size_type erase(Nodes::iterator i) {
            return this->nodes.erase(i);
        }

        Nodes::size_type erase(const Type& value) {
            return this->erase(this->find(value));
        }

        Nodes::size_type erase(Node* node) {
            return this->nodes.erase(node);
        }

        void clear() {
            for (Node* node : this->nodes) delete node;
            this->nodes.clear();
        }

        const std::vector<Node*>& aStar(
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
                decltype(&compare)
            > openSet(compare);
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
                if (current.second == target) {
                    std::vector<Node*> path{current};
                    std::map<Node*, Node*>::iterator index;
                    while ((index = cameFrom.find(current)) != cameFrom.end()) {
                        current = *index;
                        path.push_back(current);
                    }
                    return std::move(path);
                }

                openSet.pop();
                tracking.erase(current);
                for (const Node::Edge& edge : current) {
                    double tentative_gScore = gScore[current] + edge.second;
                    if (!gScore.find(edge.first) || tentative_gScore < gScore[edge.first]) {
                        cameFrom[edge.first] = current;
                        gScore[edge.first] = tentative_gScore;
                        fScore[edge.first] = tentative_gScore + heuristic(edge.first, target);
                        if (!tracking.find(edge.first)) {
                            openSet.push(edge.first);
                            tracking.insert(edge.first);
                        }
                    }
                }
            }

            return {};
        }
    };
};
