#include "search.h"

void softmax(float* input, size_t size) {
    int i;
    double m, sum, constant;

    m = -INFINITY;
    for (i = 0; i < size; ++i) {
        if (m < input[i]) {
            m = input[i];
        }
    }

    sum = 0.0;
    for (i = 0; i < size; ++i) {
        sum += exp(input[i] - m);
    }

    constant = m + log(sum);
    for (i = 0; i < size; ++i) {
        input[i] = exp(input[i] - constant);
    }

}

Search::Search() {

}

void Search::set_root_node(Node* node) {
    root = node;
}

Node* Search::get_root_node() {
    return root;
}

void Search::run_iteration(History& history) {
    std::vector<Node*> leaf_nodes;
    trajectoryBuffer.clear();
    add_leaf_node(history, leaf_nodes);

    InputPlanes planes = EncodePositionForNN(history);
    auto out = nn.forward(planes);
    float* policy = out[0];
    float* wdl = out[1];
    wdl[1] = 0.0f;

    softmax(policy, 1858);
    softmax(wdl, 3);

    std::vector<Move> actions;
    std::vector<float> priors; 

    if (history.last().turn()) {
        MoveList<BLACK> list(history.last());
        for (Move m : list) {
            actions.push_back(m);
            auto it = std::find(policy_index.begin(), policy_index.end(), mirror_uci(m.uci()));
            priors.push_back(policy[it - policy_index.begin()]);
        };
    }
    else {
        MoveList<WHITE> list(history.last());
        for (Move m : list) {
            actions.push_back(m);
            auto it = std::find(policy_index.begin(), policy_index.end(), m.uci());
            priors.push_back(policy[it - policy_index.begin()]);
        }
    }

    float value = wdl[0] - wdl[2];
    if (actions.empty()) {
        if (history.last().checkers) {
            value = -1.0f;
        }
        else {
            value = 0.0f; 
        }
    }

    expand_leaf_node(leaf_nodes.back(), actions, priors);
    backup_leaf_node(history, -value);    
}

void Search::add_leaf_node(History& history, std::vector<Node*>& leaf_nodes) {
    Node* curr = root;
    while (true) {
        trajectoryBuffer.emplace_back(curr);
        if (!curr->get_expanded()) {
            break;
        }
        Node* best_child = curr->get_best_child();
        best_child->apply_virtual_loss(1.0f); 
        curr = best_child;
        Move action = curr->get_action();
        history.push(action); 
    }
    if (!curr->is_added()) {
        curr->set_added(true);
    }
    leaf_nodes.emplace_back(curr);
}

void Search::expand_leaf_node(Node* leaf, std::vector<Move> actions, std::vector<float> priors) {
    if (!leaf->get_expanded()) {
        size_t num_children = actions.size();
        for (size_t i = 0; i < num_children; i++) {
            std::shared_ptr<Node> child = std::make_shared<Node>();
            leaf->add_child(child, actions[i], priors[i]);
        }
        if (!actions.empty()) {
            leaf->set_expanded(true);
        }
    }
}

void Search::backup_leaf_node(History& history, float value) {
    for (auto it = trajectoryBuffer.rbegin(); it != trajectoryBuffer.rend(); ++it) {
        Node* node = *it;
        node->update_terminal(value);
        value = -value;

        if (node != root) {
            history.pop();
            node->undo_virtual_loss(1.0f); 
        }
    }
}

void run_search_thread(Search* t, History& history) {
    Node* root = new Node; 
    t->set_root_node(root);
    for (int i = 0; i < 1000; i++) {
        t->run_iteration(history);
    }

    auto children = root->get_children(); 
    int most_visits = 0; 
    std::shared_ptr<Node> most_visited; 
    
    for (size_t i = 0; i < children.size(); i++) {
        if (children[i]->get_visits() > most_visits) {
            most_visits = children[i]->get_visits();
            most_visited = children[i]; 
        }
    }
    std::cout << "bestmove " << most_visited->get_action() << std::endl;
}