#pragma once
#include <unordered_map>

namespace game {
    template<typename Type> class Node {
    protected:
        std::unordered_map<Node*, double> edges;
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
    };

    class Graph {
    protected:
    public:
    };
};
