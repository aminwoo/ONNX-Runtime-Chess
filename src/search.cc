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

void Search::run_iteration(std::vector<Position>& history) {
    std::vector<Node*> leafNodes;
    trajectoryBuffer.clear();
    add_leaf_node(history, leafNodes);

    InputPlanes planes = EncodePositionForNN(history);
    auto out = nn.forward(planes);
    float* policy = out[0];
    float* wdl = out[1];

    softmax(policy, 1858);
    softmax(wdl, 3);

    std::vector<Move> actions;
    std::vector<float> priors; 

    if (history.back().turn()) {
        MoveList<BLACK> list(history.back());
        for (Move m : list) {
            actions.push_back(m);
            auto it = std::find(policy_index.begin(), policy_index.end(), mirror_uci(m.uci()));
            priors.push_back(policy[it - policy_index.begin()]);
        };
    }
    else {
        MoveList<WHITE> list(history.back());
        for (Move m : list) {
            actions.push_back(m);
            auto it = std::find(policy_index.begin(), policy_index.end(), m.uci());
            priors.push_back(policy[it - policy_index.begin()]);
        }
    }

    float value = wdl[0] - wdl[2];
    if (actions.empty()) {
        value = -1;
    }
    //else if (history.back().is_draw()) {
    //    value = 0; 
    //}

    expand_leaf_node(leafNodes.back(), actions, priors);
    backup_leaf_node(history, value);    
}

void Search::add_leaf_node(std::vector<Position>& history, std::vector<Node*>& leafNodes) {
    Node* curr = root;
    while (true) {
        trajectoryBuffer.emplace_back(curr);
        if (!curr->get_expanded()) {
            break;
        }
        Node* bestChild = curr->get_best_child();
        curr = bestChild;
        Move action = curr->get_action();

        history.emplace_back(history.back());

        if (history.back().turn()) {
            history.back().play<BLACK>(action);
        }
        else {
            history.back().play<WHITE>(action);
        }
    }
    if (!curr->is_added()) {
        curr->set_added(true);
    }
    leafNodes.emplace_back(curr);
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

void Search::backup_leaf_node(std::vector<Position>& history, float value) {
    for (auto it = trajectoryBuffer.rbegin(); it != trajectoryBuffer.rend(); ++it) {
        Node* node = *it;
        node->update_terminal(value);
        value = -value;

        if (node != root) {
            history.pop_back();
        }
    }
}

void run_search_thread(Search* t, std::vector<Position>& history) {
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