#include "node.h"

Node* Node::get_best_child() {
    Node* best_child = nullptr;
    float best_value = -std::numeric_limits<float>::infinity();
    float c = 2.15f;

    size_t numChildren = children.size();
    for (size_t i = 0; i < numChildren; i++) {
        float u = sqrt(visits) * children[i]->get_prior() / (1.0f + children[i]->get_visits());
        float q = children[i]->Q(); 
        float v = (1 + children[i]->get_visits() - children[i]->get_virtual_loss()) / (1 + children[i]->get_visits() + children[i]->get_virtual_loss()); 
        float val = q * v + c * u;
        if (val > best_value) {
            best_value = val;
            best_child = children[i].get();
        }
    }
    return best_child;
}