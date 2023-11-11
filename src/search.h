#pragma once

#include "policy.h"
#include "node.h"
#include "network.h"

#include <math.h>

class Node;

class Search {

private:
    Network nn;
    Node* root;
    std::vector<Node*> trajectoryBuffer;

public:
    Search();

    Node* get_root_node();
    void set_root_node(Node* node);
    void add_leaf_node(History& history, std::vector<Node*>& leafNodes);
    void expand_leaf_node(Node* leaf, std::vector<Move> actions, std::vector<float> priors);
    void backup_leaf_node(History& history, float value);
    void run_iteration(History& history);
};

void run_search_thread(Search* t, History& history);