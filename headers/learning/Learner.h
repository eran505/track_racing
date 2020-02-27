//
// Created by ERANHER on 27.2.2020.
//

#ifndef TRACK_RACING_LEARNER_H
#define TRACK_RACING_LEARNER_H


#include "util_game.hpp"
#include <torch/torch.h>
#include <iostream>
#include "neuralNet.h"
#include <memory>
#include "ReplayBuffer/ReplayBuffer.hpp"
using namespace torch;
/*
 *   Q*(s,a) = reward + (is_done*(-1))*discount_factor*(T(s,a,s')V(s'))
 *   loss (Q*-Q(s,a))
 *   if(fix time update)
 *      targetNet <-- evalNet
 * */
class Learner{
    float discountedFactor = 0.985;
    neuralNet* evalNet;   // this Net chooses the best action (Policy-net)
    neuralNet* targetNet; // this Net does the T(s,a,s') calculation
    unsigned int batchSizeEntries;
    bool isDoubleNet;

public:
    Learner(bool isDoubleNetwork, int sizeFeatuersIn, int batchSize);
    ~Learner();
    int predictValue(vector<float> *state);
    void updateNet(const ReplayBuffer *buffer);

private:
    Tensor calcQtraget(const ReplayBuffer *buffer,int index);
};

Learner::Learner(bool isDoubleNetwork,int sizeFeatuersIn,int batchSize): batchSizeEntries(batchSize),
evalNet(new neuralNet(sizeFeatuersIn)),targetNet(evalNet),isDoubleNet(isDoubleNetwork)
{
    if (isDoubleNetwork)
        this->targetNet = new neuralNet(sizeFeatuersIn);
}

int Learner::predictValue(vector<float> *state) {
    ArrayRef<float> xx = *state;
    auto Sstate = torch::tensor(xx);
    return this->evalNet->evalArgMaxQ(Sstate);
}
void Learner::updateNet(const ReplayBuffer *buffer) {
    if (!buffer->isSufficientAmountExperience())
        return;

    //random samples
    unordered_set<int> entries;

    buffer->sampleEntries(batchSizeEntries,entries);
    for (const auto entryIndx:entries)
    {
        float sum_of_elems;
        vector<float> expReward;
        auto QMaxValues = this->calcQtraget(buffer,entryIndx);
        auto probList = torch::tensor(*buffer->pProbabilityNextStates[entryIndx]);
        auto isNotEndState = torch::tensor(buffer->isEndStateNot[entryIndx]);
        auto rewardVec = torch::tensor(*buffer->rRewardNextStates[entryIndx]);
        cout<<"QMaxValues.sizes() \n"<<QMaxValues<<endl;
        cout<<"probList.sizes()\n"<<probList<<endl;
        cout<<"isNotEndState.sizes()\n "<<isNotEndState<<endl;
        cout<<"rewardVec.sizes()\n"<<rewardVec<<endl;
        // Q*=r+dis_factor*T(s,a,s)*V(s')


        std::for_each(expReward.begin(), expReward.end(), [&] (float n) {
            sum_of_elems += n;
        });
        auto valueCurState = this->evalNet->getActionStateValue(buffer->stateS[entryIndx],buffer->aAction[entryIndx]);
        //need to be tensor ->sum_of_elems
        auto sum_of_elemsTensoer = torch::tensor(sum_of_elems);
        //cout<<"sum_of_elemsTensoer = "<<sum_of_elemsTensoer<<endl;
        //cout<<"valueCurState = "<<valueCurState<<endl;

        this->evalNet->learn(valueCurState,sum_of_elemsTensoer);

    }
}

Learner::~Learner() {
    if (isDoubleNet)
        delete this->targetNet;
    delete this->evalNet;

}

Tensor Learner::calcQtraget(const ReplayBuffer *buffer, int index) {
    vector<float> nNextStateExpectedReward;
    auto x = torch::zeros({0,0});
    torch::Tensor Qtarget;
    bool isFirst=true;
    // This scope calc the expected value of the transition
    vector<at::Tensor> l ;
    for (const auto nNextItem :  buffer->nNextStates[index]) {
        ArrayRef<float> xx = *nNextItem;
        auto curTensor = torch::tensor(xx);
        l.push_back(curTensor);
    }
    Qtarget=torch::stack(l);

    Tensor resultsTensor = this->targetNet->getEvalMaxValue(Qtarget); // Does not accumulate gradient
    return resultsTensor;
}


#endif //TRACK_RACING_LEARNER_H
