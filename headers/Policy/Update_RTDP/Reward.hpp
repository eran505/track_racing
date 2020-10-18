//
// Created by eranhe on 26/07/2020.
//

#ifndef TRACK_RACING_REWARD_HPP
#define TRACK_RACING_REWARD_HPP
#include "util_game.hpp"

struct Rewards {
public:
    double CollReward = 1000;
    double GoalReward = -500;
    double WallReward = -1000;
    double Step_reward = 0;
    double discountF=0.987;
    static Rewards getRewards()
    {
        return Rewards();
    }

};





#endif //TRACK_RACING_REWARD_HPP
