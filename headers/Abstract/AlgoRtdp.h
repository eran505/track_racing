//
// Created by eranhe on 10/05/2020.
//

#ifndef TRACK_RACING_ALGORTDP_H
#define TRACK_RACING_ALGORTDP_H
#include "State.hpp"
#include "util_game.hpp"
#include "Trees/Tree.hpp"
struct abstractContainer{
    u_int16_t action; // The action that in the last state
    u_int64_t hashState; // Hash vale of state
    float valueV; // Expected sum of rewards
    float probability; // Transition probability
};

using namespace Trees;
typedef abstractContainer container;
class treeAbs : Tree<container> {
    unordered_map<u_int64_t,Node<container>*> fast_access;

};
#endif //TRACK_RACING_ALGORTDP_H
