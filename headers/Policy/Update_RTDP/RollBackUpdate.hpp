//
// Created by eranhe on 27/07/2020.
//

#ifndef TRACK_RACING_ROLLBACKUPDATE_HPP
#define TRACK_RACING_ROLLBACKUPDATE_HPP
#include "State.hpp"
class RollBackUpdater_classic{

    std::shared_ptr<vector<pair<State,pair<u_int64_t,int>>>> stack_roll_back = nullptr;

public:

    RollBackUpdater_classic()
    {
    }

};


#endif //TRACK_RACING_ROLLBACKUPDATE_HPP
