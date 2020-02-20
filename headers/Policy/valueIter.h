//
// Created by eranhe on 20/02/2020.
//

#ifndef TRACK_RACING_VALUEITER_H
#define TRACK_RACING_VALUEITER_H

#include <utility>

#include "Policy.hpp"

class valueIter: Policy{

    float *valueTable;

public:
    valueIter(string namePolicy, int maxSpeedAgent, string agentId);
    ~valueIter() override;

};

valueIter::~valueIter() = default;

valueIter::valueIter(string namePolicy, int maxSpeedAgent, string agentId) : Policy(std::move(namePolicy), maxSpeedAgent, agentId) {

}
void fillTabke(vector<string> opState)
{
    
}


#endif //TRACK_RACING_VALUEITER_H
