//
// Created by eranhe on 26/07/2020.
//

#ifndef TRACK_RACING_REWARD_HPP
#define TRACK_RACING_REWARD_HPP
#include "util_game.hpp"

typedef double valueType;
struct Rewards {
public:
    valueType CollReward = 1000;
    valueType GoalReward = -500;
    valueType WallReward = -1000;
    valueType Step_reward = 0;
    valueType discountF=0.987;//0.987;
    static Rewards getRewards()
    {
        return Rewards();
    }



};





#endif //TRACK_RACING_REWARD_HPP
