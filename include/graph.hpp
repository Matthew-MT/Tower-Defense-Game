#pragma once
#include <unordered_map>
#include <vector>

namespace game {

    template<typename Type> class Node {
    public:
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
    };

    template<typename Type> class Graph {
    protected:
        std::vector<Node<Type>*> nodes;
    public:
    };
};
