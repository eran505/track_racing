//
// Created by ERANHER on 22.1.2020.
//

#ifndef TRACK_RACING_DEEPRTDP_H
#define TRACK_RACING_DEEPRTDP_H

#include "Learner.h"
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
    float heuristicID;
    unsigned int preTrainNet;
    FeatureGen* featuerConv= nullptr;
    vector<feature*> fNextState;
    feature* fStateCurrFeaturesQ;
    unsigned int fAction;
    vector<float>* vecProbabilities;
    vector<float>* vecRewards;
    ReplayBuffer *myReplayBuffer;
    Learner *dqn;
    bool heuristicFunc;
    bool preTrainNetBool;
    int ctrPreTrainNet=1000;

    template<typename KeyType, typename ValueType>
    std::pair<KeyType,ValueType> get_max( const std::unordered_map<KeyType,ValueType>& x );
    Point* getRandomlyAction(vector<int> intVect);
    int getMaxActionId(State* s);
    void bellmanUpdate(State *s,Point& actionP);
    void rec_update(State *s,int index, double acc_probablity);
    bool applyAction(State *s, const string &id, Point &action, int max_speed);
    double rewardState(State *s);
    void initBuffers();
    vector<float>* heuristicFuncImpl(vector<float> *state);

public:

    DeepRTDP(string namePolicy, int maxSpeedAgent,int seed,const string& agentID,int goal_numbers,float IDHuer);
    ~DeepRTDP() override { delete this->nNet; }
    void setNet(neuralNet* myNet){this->nNet=myNet;}
    void reset_policy() override;
    void policy_data() const override;
    const vector<float >* TransitionAction(State *s) override ;
    Point get_action(State *s) override;
    void setPreTraining();
    vector<float>* getYTrue();

};

DeepRTDP::DeepRTDP(string namePolicy, int maxSpeedAgent,int seed,const string& agentID,int goal_numbers,float IDHuer=0):Policy(std::move(namePolicy),maxSpeedAgent,
        agentID),ctrRandom(seed),featuerConv(new FeatureGen(agentID,goal_numbers)),heuristicID(IDHuer),
        myReplayBuffer(new ReplayBuffer(30)){
    this->dqn=new Learner(false,this->featuerConv->getFeatureVecSize(),5);
    //nNet = new neuralNet(this->featuerConv->getFeatureVecSize());
    this->setPreTraining();

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
    auto entryIdx = this->getMaxActionId(s);

    //choose randomly one
    Point actionI = *getRandomlyAction({entryIdx});

    if (this->preTrainNet){
        this->dqn->preTrainNet(this->fStateCurrFeaturesQ,this->getYTrue());
        ctrPreTrainNet--;
        cout<<"ctrPreTrainNet="<<ctrPreTrainNet<<endl;
        if (ctrPreTrainNet==0)
        {
            this->preTrainNet=false;
            cout<<" -- END - preTrainNet -- \n"<<endl;
        }

    }
    else {
        bellmanUpdate(s,actionI);
    }
    return actionI;
}


template<typename KeyType, typename ValueType>
std::pair<KeyType,ValueType> DeepRTDP::get_max( const std::unordered_map<KeyType,ValueType>& x ) {
    using pairtype=std::pair<KeyType,ValueType>;
    return *std::max_element(x.begin(), x.end(), [] (const pairtype & p1, const pairtype & p2) {
        return p1.second < p2.second;
    });
}

int DeepRTDP::getMaxActionId(State *s) {
    unordered_map <int,double> QstateTable;
    this->fStateCurrFeaturesQ=featuerConv->getFeaturesS(s);
    auto entry = this->dqn->predictValue(this->fStateCurrFeaturesQ);
    return entry;
}




Point *DeepRTDP::getRandomlyAction(vector<int> intVect) {
    int argMax;
    int size = intVect.size();
    if (size>1)
    {
        argMax = intVect.operator[](ctrRandom%size);
        ctrRandom = ++ctrRandom%this->hashActionMap->size();

    } else
        argMax = intVect.operator[](0);
    auto pos = this->hashActionMap->find(argMax);
    if (pos == this->hashActionMap->end())
        throw std::invalid_argument( "function::get_argmx_action Error" );
    return pos->second;
}

void DeepRTDP::bellmanUpdate(State *s, Point& actionP){
    // init the containers
    this->initBuffers();

    int indexTran = -1;
    //this->fStateCurrFeaturesQ=featuerConv->getFeaturesS(s);
    this->fAction=actionP.hashMeAction(Point::actionMax);
    // copy state
    auto stateCur = new State(*s);

    this->applyActionToState(stateCur, &actionP);


    this->rec_update(stateCur,indexTran,1);
    //cout<<stateCur->to_string_state()<<endl;
    //insert to buffer
    this->myReplayBuffer->addBuffer(vecProbabilities,vecRewards,this->fNextState,fAction,fStateCurrFeaturesQ);
    this->dqn->updateNet(this->myReplayBuffer);
}

void DeepRTDP::initBuffers() {
    this->fNextState.clear();
    this->vecProbabilities = new vector<float>();
    this->vecRewards = new vector<float>();
}
void DeepRTDP::rec_update(State *s,int index, double acc_probablity) {
    //cout<<s->to_string_state()<<endl;
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

void DeepRTDP::setPreTraining() {
    if (this->heuristicID==0)
        preTrainNetBool=false;
    else
    {
        preTrainNetBool= true;
        if (this->heuristicID>0)
            heuristicFunc=false;
        else
            heuristicFunc= true;
    }

}

vector<float> *DeepRTDP::heuristicFuncImpl(vector<float> *state) {
    int sizeVec = int(pow(3.0,Point::D_point::D));
    auto* ptrVec = new vector<float>();
    for (int i = 0; i < sizeVec; ++i) {
        ptrVec->push_back(10.0);
    }
    return ptrVec;
}

vector<float> *DeepRTDP::getYTrue() {
    auto vecState = this->fStateCurrFeaturesQ;
    vector<float> *ptrVec;
    if (this->heuristicFunc){
        ptrVec = this->heuristicFuncImpl(vecState);
    } else{
        int sizeVec = int(pow(3.0,Point::D_point::D));
        ptrVec = new vector<float>();
        for (int i = 0; i < sizeVec; ++i) {
            ptrVec->push_back(this->heuristicID);
        }
    }
    return ptrVec;


}

#endif //TRACK_RACING_DEEPRTDP_H
