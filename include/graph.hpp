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
        ~GraphNode() {
            for (std::pair<GraphNode<Type>*, double>& node : this->edges) node.first->unlink(this, false);
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

        Nodes::insert_return_type insert(Node* node) {
            return this->nodes.insert(node);
        }

        Nodes::size_type erase(Node* node) {
            return this->nodes.erase(node);
        }

        const std::vector<Node*>& aStar(
            Node* origin,
            Node* target,
            const std::function<double(Node* a, Node* b)>& heuristic
        ) {
            using queue_type = std::pair<double, Node*>;
            std::function<bool(const queue_type& a, const queue_type& b)> compare = [&](const queue_type& a, const queue_type& b) -> bool {
                return a.first < b.first;
            };
            std::vector<std::vector<Node*>> paths;
            std::priority_queue<
                queue_type,
                std::vector<queue_type>,
                decltype(&compare)
            > openSet(compare);
            openSet.push({0, origin});
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
                    }
                }
            }
        }
    };
};
