//
// Created by ERANHER on 22.1.2020.
//

#ifndef TRACK_RACING_DEEPRTDP_H
#define TRACK_RACING_DEEPRTDP_H

#include <utility>

#include "Policy/Policy.hpp"
#include "neuralNet.h"

class DeepRTDP : public Policy{
    // Rewards
    double collReward=10;double goalReward=-10;double wallReward=-11;
    int ctrState=0;
    neuralNet* nNet;
    int ctrRandom;

    template<typename KeyType, typename ValueType>
    std::pair<KeyType,ValueType> get_max( const std::unordered_map<KeyType,ValueType>& x );
    Point* getRandomlyAction(vector<int>* intVect);
    vector<int>* getMaxActionId(State* s);

public:

    DeepRTDP(string namePolicy, int maxSpeedAgent,int seed);
    ~DeepRTDP() override { delete this->nNet; }
    void setNet(neuralNet* myNet){this->nNet=myNet;}
    void reset_policy() override;
    void policy_data() const override;
    const vector<float >* TransitionAction(State *s) override ;
    Point get_action(State *s) override;

};

DeepRTDP::DeepRTDP(string namePolicy, int maxSpeedAgent,int seed):Policy(std::move(namePolicy),maxSpeedAgent){
    nNet = nullptr;
    ctrRandom=seed;
}

void DeepRTDP::policy_data() const {

}

const vector<float> *DeepRTDP::TransitionAction(State *s) {
    return nullptr;
}

void DeepRTDP::reset_policy() {
    Policy::reset_policy();
}

Point DeepRTDP::get_action(State *s) {
    auto argMaxList = this->getMaxActionId(s);

    //choose randomly one
    auto actionI = getRandomlyAction(argMaxList);
    //del
    delete argMaxList;

    return *actionI;
}


template<typename KeyType, typename ValueType>
std::pair<KeyType,ValueType> DeepRTDP::get_max( const std::unordered_map<KeyType,ValueType>& x ) {
    using pairtype=std::pair<KeyType,ValueType>;
    return *std::max_element(x.begin(), x.end(), [] (const pairtype & p1, const pairtype & p2) {
        return p1.second < p2.second;
    });
}

vector<int>* DeepRTDP::getMaxActionId(State *s) {
    unordered_map <int,double> QstateTable;
    auto max_val = this->collReward*-2;

    for (auto &itm : *this->hashActionMap) {
        auto expectedValue = this->nNet->getQvalue(s,itm.second);
        QstateTable.insert({itm.first,expectedValue})
    }
    auto val_max = this->get_max(QstateTable);
    auto* actionList=new vector<int>();

    for (auto &itm : QstateTable) {
        if (val_max.second==itm.second)
            actionList->push_back(itm.first);
    }
    return actionList;
}

Point *DeepRTDP::getRandomlyAction(vector<int> *intVect) {
    int argMax;
    int size = intVect->size();
    if (size>1)
    {
        argMax = intVect->operator[](ctrRandom%size);
        ctrRandom = ++ctrRandom%this->hashActionMap->size();

    } else
        argMax = intVect->operator[](0);
    auto pos = this->hashActionMap->find(argMax);
    if (pos == this->hashActionMap->end())
        throw std::invalid_argument( "function::get_argmx_action Error" );
    return pos->second;
}


#endif //TRACK_RACING_DEEPRTDP_H
