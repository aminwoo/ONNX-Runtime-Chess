#pragma once

#include "types.h"

#include <math.h>
#include <vector>


class Node {
private:
    std::vector<std::shared_ptr<Node>> children;
    Move action;
    float value_sum = -1.0f;
    float prior = 0.0f;
    int visits = 0;
    bool added = false;
    bool expanded = false;

public:
    Node() {};

    Node* get_best_child();

    void update_terminal(float value) {
        value_sum -= value;
        visits++;
    }

    void set_prior(float value) {
        prior = value; 
    }

    float get_prior() {
        return prior; 
    }

    bool is_added() {
        return added;
    }

    void set_added(bool value) {
        added = value;
    }

    void add_child(std::shared_ptr<Node> child, Move action, float prior) {
        child->set_action(action); 
        child->set_prior(prior); 
        child->set_action(action);
        children.emplace_back(child);
    }

    std::vector<std::shared_ptr<Node>> get_children() {
        return children;
    }

    bool get_expanded() {
        return expanded;
    }

    void set_expanded(bool value) {
        expanded = value;
    }

    Move get_action() {
        return action;
    }

    void set_action(Move value) {
        action = value;
    }

    int get_visits() {
        return visits;
    }

    void increment_visits() {
        visits++;
    }

    void decrement_visits() {
        visits--;
    }

    float Q() {
        return value_sum / (1.0f + visits);
    }
};
