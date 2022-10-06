#pragma once
#include <unordered_map>
#include <unordered_set>
#include "utils.hpp"
#include <vector>
#include <limits>
#include <queue>
#include <map>

namespace game {

    template<typename Type> class Node {
    public:
        using Edge = std::pair<Node*, double>;
        using Edges = std::unordered_map<Node*, double>;
    protected:
        Edges edges;
        Type* value;
    public:
        Node(const Type* initValue) : value{initValue} {}
        ~Node() {
            for (std::pair<Node*, double>& node : this->edges) node.first->unlink(this, false);
        }

        void link(Node* other, double weight = 1.f, bool undirected = true) {
            this->edges.insert({other, weight});
            if (undirected) other->link(this, weight, false);
        }

        void unlink(Node* other, bool undirected = true) {
            this->edges.erase(other);
            if (undirected) other->unlink(this, false);
        }

        Edges::iterator begin() {
            return this->edges.begin();
        }

        Edges::iterator end() {
            return this->edges.end();
        }

        double weight(Node* other) {
            Edges::iterator i = this->edges.find(other);
            if (i == this->edges.end()) return std::numeric_limits<double>::max();
            else return i->second;
        }
    };

    template<typename Type> class Graph {
    public:
        using Nodes = std::unordered_set<Node<Type>*>;
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

        const std::vector<std::vector<Node*>>& aStar(Node* origin, const std::vector<Node*>& targets) {
            std::vector<std::vector<Node*>> paths;
            std::priority_queue<IPoint> openSet;
            std::map<Node*, double>
                cameFrom,
                gScore,
                fScore;

            gScore[origin] = 0;
            fScore[origin] = 0;
        }
    };
};
