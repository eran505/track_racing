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
#include "ReplayBuffer/SumTree.hpp"
#include "FeatureGen.h"
#include "ReplayBuffer/prioritizedExperienceReplay.hpp"
class DeepRTDP : public Policy{


// Rewards
    float collReward=1;float goalReward=-1;float wallReward=-1;
    int ctrState=0;
    bool isEndBool=false;
    float discountFactor=0.987;
    int ctrRandom;
    float heuristicID;
    unsigned int preTrainNet;
    FeatureGen* featuerConv= nullptr;
    vector<feature*> fNextState;
    feature* fStateCurrFeaturesQ;
    short fAction;
    vector<float>* vecProbabilities;
    vector<float>* vecRewards;
    vector<short> isNotEnd;
    //ReplayBuffer *myReplayBuffer;
    Learner *dqn;
    bool heuristicFunc;
    bool preTrainNetBool;
    int ctrPreTrainNet=1000;

    //debug
    vector<State*> nextStateH;
    vector<float> nextStateHProb;

    template<typename KeyType, typename ValueType>
    std::pair<KeyType,ValueType> get_max( const std::unordered_map<KeyType,ValueType>& x );
    Point* getRandomlyAction(vector<int> intVect);
    int getMaxActionId(State* s);
    void bellmanUpdate(State *s,Point& actionP);
    void rec_update(State *s,int index, double acc_probablity);
    bool applyAction(State *s, const string &id, Point &action, int max_speed);
    double rewardState(State *s,bool isEnd);
    void initBuffers();
    float recH(State *s,int index, float acc_probablity,int lookup);
    vector<float>* heuristicFuncImpl(State *state);
    experienceTuple* constructExperience();
public:

    DeepRTDP(string namePolicy, int maxSpeedAgent, int seed, const string &agentID, int goal_numbers, string &home,
             float IDHuer);
    ~DeepRTDP() override
    {
        delete this->dqn;
        delete this->featuerConv;
    }
    void reset_policy() override;
    void policy_data() const override;
    const vector<float >* TransitionAction(State *s) override ;
    Point get_action(State *s) override;
    void setPreTraining();
    vector<float>* getYTrue(State *s);
    vector<float>* computeH(State *s,bool isLookUp);
    void getHeuristicValue(State *s, int index, float accProbablity);
    float compute_h(State *pState);
    float lookHead(State *s);
    vector<float> *searchLook(State *s);
};

DeepRTDP::DeepRTDP(string namePolicy, int maxSpeedAgent,int seed,const string& agentID,int goal_numbers,string &home,float IDHuer=0):Policy(std::move(namePolicy),maxSpeedAgent,
        agentID,home),ctrRandom(seed),featuerConv(new FeatureGen(agentID,goal_numbers,this->max_speed)),heuristicID(IDHuer){
    this->dqn=new Learner(true,this->featuerConv->getFeatureVecSize(),25,
            discountFactor,this->home, true);
    this->dqn->epslionGreedy= true;
    this->setPreTraining();
    this->featuerConv->set_string_home(this->home);

}

void DeepRTDP::policy_data() const {
}



void DeepRTDP::getHeuristicValue(State *s, int index, float accProbablity){
    //cout<<s->to_string_state()<<endl;
    float valueH;
    if (index == 0 )
    {
        nextStateH.push_back(new State(*s));
        nextStateHProb.push_back(accProbablity);
        return;
    }
    auto oldState = State(*s);
    index++;
    auto res = tran[index]->TransitionAction(s);
    // waring need to del the res (pointer)
    for (int i = 0; i < res->size(); ++i)
    {
        auto pos = this->hashActionMap->find(res->operator[](i));
        auto action = pos->second;
        this->applyAction(s, tran[index]->id_agent, *action, tran[index]->max_speed);
        this->getHeuristicValue(s, index, accProbablity * res->operator[](++i));
        s->assignment(oldState, tran[index]->GetId());
    }
    delete(res);
}

vector<float>* DeepRTDP::computeH(State *s,bool isLookUp = false){
    size_t sizeAction= this->hashActionMap->size();
    auto* vecH = new vector<float>(sizeAction);
    auto oldState = new State(*s);
    for( const auto &item_action : *this->hashActionMap)
    {
        // apply action state and let the envirmont to roll and check the reward/pos
        Point *actionCur = item_action.second;
        float val;
        bool isWall =s->applyAction(this->id_agent, *actionCur, max_speed);
        if (isWall)
            val = this->wallReward;
        else{
            if(!isLookUp)
                val = this->compute_h(s);
            else
                val = this->recH(s,0,1,1);
        }
        s->assignment(*oldState,this->id_agent);
        vecH->operator[](item_action.first)=val;
    }
    delete(oldState);
    return vecH;
}

double DeepRTDP::rewardState(State *s,bool isEnd=true)
{
    isEndBool=false;
    short iNotEnd=1;
    double reward=0;
    if (s->isGoal()){
        isEndBool=true;
        iNotEnd=0;
        reward+=this->goalReward;
    }
    else if (this->is_wall){
        isEndBool=true;
        iNotEnd=0;
        reward+=this-> wallReward;
    }
    else {
        auto res = s->is_collusion(this->id_agent);
        if (!res.empty()) {
            isEndBool=true;
            iNotEnd = 0;
            reward += this->collReward;
        }
    }
    if (isEnd)
        this->isNotEnd.push_back(iNotEnd);
    return reward;
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
//    if(this->evalPolicy)
//        cout<<"ARGMAX="<<actionI.to_str()<<endl;

    if (this->preTrainNetBool){
        this->dqn->preTrainNet(this->fStateCurrFeaturesQ,this->getYTrue(s));
        ctrPreTrainNet--;
        cout<<"ctrPreTrainNet="<<ctrPreTrainNet<<endl;
        if (ctrPreTrainNet==0)
        {
            this->preTrainNetBool=false;
            this->dqn->updateNetWork();
            cout<<" -- END - preTrainNet -- \n"<<endl;
        }

    }
    else {
        if(!this->evalPolicy)
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
   // cout<<"Cur State: "<<s->to_string_state()<<endl;
    unordered_map <int,double> QstateTable;
    this->fStateCurrFeaturesQ=featuerConv->getFeaturesS(s);
    this->dqn->epslionGreedy=!this->evalPolicy;
    auto entry = this->dqn->predictValue(this->fStateCurrFeaturesQ);
    if (preTrainNetBool and !this->heuristicFunc)
        entry = this->dqn->predictValue(this->fStateCurrFeaturesQ,true);
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

    auto expItem = this->constructExperience();

    auto errorItem = this->dqn->computerError(expItem,false);
    // insert to buffer
    this->dqn->insertToBuffer(expItem,errorItem);
    //this->myReplayBuffer->addBuffer(vecProbabilities,vecRewards,this->fNextState,fAction,fStateCurrFeaturesQ);
    //  this->prioritizedBuffer->add()
    this->dqn->updateNet();
    delete stateCur;

}

void DeepRTDP::initBuffers() {
    this->fNextState.clear();
    this->isNotEnd.clear();
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

vector<float> *DeepRTDP::heuristicFuncImpl(State *state) {
    //auto vecResultsH = computeH(state, true);
    auto vecResultsH = searchLook(state);
    return vecResultsH;
}

vector<float> *DeepRTDP::getYTrue(State *s) {
    auto vecState = this->fStateCurrFeaturesQ;
    vector<float> *ptrVec;
    if (this->heuristicFunc){
        ptrVec = this->heuristicFuncImpl(s);
    } else{
        int sizeVec = int(pow(3.0,Point::D_point::D));
        ptrVec = new vector<float>();
        for (int i = 0; i < sizeVec; ++i) {
            ptrVec->push_back(this->heuristicID);
        }
    }
    return ptrVec;
}
/*
 *
 * Q(s,a) = roll_out Q(s,a)
 * */
float DeepRTDP::recH(State *s,int index, float acc_probablity,int lookup) {
    float valueH;
    //cout<<s->to_string_state()<<endl;
    if (index % tran.size() == 0 ) lookup--;

    if (lookup==0){
        auto actionValues = this->computeH(s);
        float max = *max_element(actionValues->begin(), actionValues->end());
        return max*acc_probablity;
    }
    auto oldState = State(*s);
    index++;
    auto res = tran[index]->TransitionAction(s);
    // waring need to del the res (pointer)
    for (int i = 0; i < res->size(); ++i)
    {
        auto pos = this->hashActionMap->find(res->operator[](i));
        auto action = pos->second;
        this->applyAction(s, tran[index]->id_agent, *action, tran[index]->max_speed);
        valueH+=this->recH(s,index,acc_probablity*res->operator[](++i),lookup);
        s->assignment(oldState, tran[index]->GetId());
    }
    delete(res);
    return valueH;
}



float DeepRTDP::compute_h(State *pState) {
    char team = this->id_agent[1];
    auto my_pos = pState->get_position(this->id_agent);
    vector<Point> vec_pos;
    pState->getAllPosOpponent(vec_pos,team);
    double min = pState->g_grid->getSizeIntGrid();
    double posA = -1;
    for (int i = 0; i < vec_pos.size(); ++i) {
        auto res = getMaxDistance(vec_pos[i],my_pos);
        if (min>res)
        {
            min=res;
        }
    }
    int max_speed=-1;
    for (Policy* itemPolicy:this->tran) {
        max_speed = itemPolicy->max_speed;
    }
    min=min-max_speed;
    min = std::max(0.0,min);
    //min=min/double(this->my_policy->max_speed);
    auto res = this->collReward*pow(discountFactor,min);
    //debug
    //cout<<"h(<"<<s->to_string_state()<<")="<<res<<endl;
    return res;
}
experienceTuple* DeepRTDP::constructExperience()
{
    return new experienceTuple(fAction,fStateCurrFeaturesQ,isNotEnd,vecProbabilities,
                               vecRewards,fNextState);
}



vector<float>* DeepRTDP::searchLook(State *s){
    auto *res= new vector<float>(this->hashActionMap->size());
    for (const auto &itemAction: *this->hashActionMap){
        auto stateNew = new State(*s);
        auto action = itemAction.second;
        bool wall = this->applyAction(stateNew, this->id_agent, *action, this->max_speed);
        if (wall)
            res->operator[](itemAction.first)=this->wallReward;
        else
            res->operator[](itemAction.first)=lookHead(stateNew);
        delete(stateNew);
    }
    return res;
}

float DeepRTDP::lookHead(State *s)
{
    float finalReward=0;
    queue<State*> queStack;
    queue<float> queStackProbability;
    queStack.push(new State(*s)); //inserts element at the end
    queStackProbability.push(1); //inserts element at the end
    while(!queStack.empty()) {

        auto sizeQ = queStack.size();
        int k = 0;
        while (k < sizeQ) {
            State *ptrCurState = queStack.front(); //access the first element
            auto stateProbability = queStackProbability.front();
            queStack.pop(); //removes the first element
            queStackProbability.pop();



            auto r = rewardState(ptrCurState, false);
            if (isEndBool) {
                finalReward += (float(r) * stateProbability);
                delete ptrCurState;
            } else {
                queStack.push(ptrCurState);
                queStackProbability.push(stateProbability);
            }
            k++;

        }

        for (const auto &item : tran) {
            auto size = queStack.size();
            int j = 0;
            while (j < size) {
                State *ptrCurState = queStack.front(); //access the first element
                auto stateProbability = queStackProbability.front();
                queStack.pop(); //removes the first element
                queStackProbability.pop();

                auto res = item->TransitionAction(ptrCurState);
                for (int i = 0; i < res->size(); ++i) {
                    auto newState = new State(*ptrCurState);
                    auto pos = this->hashActionMap->find(res->operator[](i));
                    auto action = pos->second;
                    float actionProb = res->operator[](++i);
                    this->applyAction(newState, item->id_agent, *action, item->max_speed);
                    queStack.push(newState);
                    queStackProbability.push(actionProb * stateProbability);
                }
                delete (ptrCurState);
                j++;
            }
        }
        sizeQ = queStack.size();
        k = 0;
        while (k < sizeQ) {
            State *sCur = queStack.front(); //access the first element
            auto stateProbability = queStackProbability.front();
            queStack.pop(); //removes the first element
            queStackProbability.pop();
            auto entryIdx = this->getMaxActionId(sCur);
            Point actionI = *getRandomlyAction({entryIdx});
            bool wall = this->applyAction(sCur, this->id_agent, actionI, this->max_speed);
            if (wall)
            {
                finalReward+=(stateProbability*this->wallReward);
                delete sCur;
            }else{
                queStack.push(sCur);
                queStackProbability.push(stateProbability);
            }
            ++k;
        }
    }
    return finalReward;
}

/**
 *
 * [] -> [] [] [] [] -->
 *
 * **/

#endif //TRACK_RACING_DEEPRTDP_H
