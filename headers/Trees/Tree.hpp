//
// Created by ERANHER on 13.5.2020.
//

#ifndef TRACK_RACING_TREE_HPP
#define TRACK_RACING_TREE_HPP
#include "util_game.hpp"
namespace Trees {

    template <typename T>
    class Node{
        std::vector<Node*> children;
        Node* father;
        std::unique_ptr<T> data;

        Node(const Node *mFatehr){
            father=mFatehr;
            children=vector<Node*>(pow(3,Point::D_point::D));
        }
    };

    template <typename T>
    class Tree{
        Trees::Node<T> root;

    };
}
#endif //TRACK_RACING_TREE_HPP
