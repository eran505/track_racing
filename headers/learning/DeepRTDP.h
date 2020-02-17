//
// Created by ERANHER on 22.1.2020.
//

#ifndef TRACK_RACING_DEEPRTDP_H
#define TRACK_RACING_DEEPRTDP_H

#include <utility>
#include "learning/ReplayBuffer/ReplayBuffer.hpp"
#include "Policy/Policy.hpp"
#include "neuralNet.h"
#include "FeatureGen.h"

class DeepRTDP : public Policy{
    // Rewards
    double collReward=10;double goalReward=-10;double wallReward=-11;
    int ctrState=0;
    neuralNet* nNet;
    int ctrRandom;

    FeatureGen* featuerConv= nullptr;
    vector<feature*> fNextState;
    feature* fStateCurrFeaturesQ;
    feature* fAction;
    vector<float>* vecProbabilities;
    vector<float>* vecRewards;
    ReplayBuffer *myReplayBuffer;



    template<typename KeyType, typename ValueType>
    std::pair<KeyType,ValueType> get_max( const std::unordered_map<KeyType,ValueType>& x );
    Point* getRandomlyAction(vector<int>* intVect);
    vector<int>* getMaxActionId(State* s);
    void bellmanUpdate(State *s,Point& actionP);
    void rec_update(State *s,int index, double acc_probablity);
    bool applyAction(State *s, const string &id, Point &action, int max_speed);
    double rewardState(State *s);
    void initBuffers();
public:

    DeepRTDP(string namePolicy, int maxSpeedAgent,int seed,string idAgent);
    ~DeepRTDP() override { delete this->nNet; }
    void setNet(neuralNet* myNet){this->nNet=myNet;}
    void reset_policy() override;
    void policy_data() const override;
    const vector<float >* TransitionAction(State *s) override ;
    Point get_action(State *s) override;


};

DeepRTDP::DeepRTDP(string namePolicy, int maxSpeedAgent,int seed,string agentID):Policy(std::move(namePolicy),maxSpeedAgent,
        agentID){
    nNet = new neuralNet();
    ctrRandom=seed;
    this->featuerConv=new FeatureGen(agentID);
    this->myReplayBuffer=new ReplayBuffer(30,10);
}

void DeepRTDP::policy_data() const {

}

double DeepRTDP::rewardState(State *s)
{
    if (s->isGoal())
        return this->goalReward;
    if (this->is_wall)
        return wallReward;
    auto res = s->is_collusion(this->id_agent);
    if (!res.empty())
        return this->collReward;
    return 0;
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
    Point actionI = *getRandomlyAction(argMaxList);
    //del
    cout<<actionI.to_str()<<endl;
    delete argMaxList;

    //update
    bellmanUpdate(s,actionI);
    cout<<actionI.to_str()<<endl;

    return actionI;
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
    auto vecState = this->featuerConv->getFeaturesS(s);
    this->nNet->predictValue(vecState);
    return nullptr;
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

void DeepRTDP::bellmanUpdate(State *s, Point& actionP){
    // init the containers
    this->initBuffers();

    int indexTran = -1;
    this->fStateCurrFeaturesQ=featuerConv->getFeaturesS(s);
    this->fAction=actionP.getFeature();
    // copy state
    auto stateCur = new State(*s);

    this->applyActionToState(stateCur, &actionP);


    this->rec_update(stateCur,indexTran,1);

    //insert to buffer
    this->myReplayBuffer->addBuffer(vecProbabilities,vecRewards,this->fNextState,fAction,fStateCurrFeaturesQ);
}

void DeepRTDP::initBuffers() {
    this->fNextState.clear();
    this->vecProbabilities = new vector<float>();
    this->vecRewards = new vector<float>();
}
void DeepRTDP::rec_update(State *s,int index, double acc_probablity) {
    if (index+1==this->tran.size()) {
        //get the state features
        auto f = this->featuerConv->getFeaturesS(s);
        //append reward state
        vecRewards->push_back(this->rewardState(s));
        // add probability
        vecProbabilities->push_back(acc_probablity);
        // add reward
        this->fNextState.push_back(f);
        return;
    }
    auto old_state = State(*s);
    index++;
    auto res = tran[index]->TransitionAction(s);
    // waring need to del the res (Pointer)
    for (int i = 0; i < res->size(); ++i)
    {
        auto pos = this->hashActionMap->find(res->operator[](i));
        auto action = pos->second;
        this->applyAction(s, tran[index]->id_agent, *action, tran[index]->max_speed);
        this->rec_update(s,index,acc_probablity*res->operator[](++i));
        s->assignment(old_state,tran[index]->GetId());
    }
}

bool DeepRTDP::applyAction(State *s, const string &id, Point &action, int max_speed)
{
    return s->applyAction(id, action, max_speed);
}
#endif //TRACK_RACING_DEEPRTDP_H
