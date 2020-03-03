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
#include <iostream>
#include <regex>
#include <stack>
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
    unsigned int updateCtr;

public:
    Learner(bool isDoubleNetwork, int sizeFeatuersIn, int batchSize);
    ~Learner();
    int predictValue(vector<float> *state);
    void updateNet(const ReplayBuffer *buffer);
    void loadStateDict();
    static bool isEmpty(at::Tensor x);
    static void LoadStateDict(torch::nn::Module& module,
                       const std::string& file_name,
                       const std::string& ignore_name_regex);
    static void SaveStateDict(const torch::nn::Module& module,
                                const std::string& file_name);

private:
    Tensor calcQtraget(const ReplayBuffer *buffer,int index);
};

Learner::Learner(bool isDoubleNetwork,int sizeFeatuersIn,int batchSize): batchSizeEntries(batchSize),
evalNet(new neuralNet(sizeFeatuersIn)),updateCtr(0),targetNet(evalNet),isDoubleNet(isDoubleNetwork)
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
//        cout<<"QMaxValues.sizes() \n"<<QMaxValues<<endl;
//        cout<<"probList.sizes()\n"<<probList<<endl;
//        cout<<"isNotEndState.sizes()\n "<<isNotEndState<<endl;
//        cout<<"rewardVec.sizes()\n"<<rewardVec<<endl;
        // Q*=r+dis_factor*T(s,a,s)*V(s')

        auto qTensor = QMaxValues*probList*discountedFactor*isNotEndState+rewardVec;
        cout<<"resTensor:\t"<<qTensor<<endl;

        auto valueCurState = this->evalNet->getActionStateValue(buffer->stateS[entryIndx],buffer->aAction[entryIndx]);

        cout<<"valueCurState;\t"<<valueCurState<<endl;

        this->evalNet->learn(valueCurState,qTensor);
        updateCtr++;
        if (updateCtr%1000==0)
        {
            auto* ptrNN = (torch::nn::Module*)this->evalNet;
            SaveStateDict(*ptrNN,"/home/eranhe/car_model/nn/nn.tr");
        }
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

void Learner::loadStateDict() {

}
void Learner::SaveStateDict(const torch::nn::Module& module,
                   const std::string& file_name) {
    torch::serialize::OutputArchive archive;
    auto params = module.named_parameters(true /*recurse*/);
    auto buffers = module.named_buffers(true /*recurse*/);
    for (const auto& val : params) {
        if (!Learner::isEmpty(val.value())) {
            archive.write(val.key(), val.value());
        }
    }
    for (const auto& val : buffers) {
        if (!Learner::isEmpty(val.value())) {
            archive.write(val.key(), val.value(), /*is_buffer*/ true);
        }
    }
    archive.save_to(file_name);
}

void Learner::LoadStateDict(torch::nn::Module& module,
                   const std::string& file_name,
                   const std::string& ignore_name_regex) {
    torch::serialize::InputArchive archive;
    archive.load_from(file_name);
    torch::NoGradGuard no_grad;
    std::regex re(ignore_name_regex);
    std::smatch m;
    auto params = module.named_parameters(true /*recurse*/);
    auto buffers = module.named_buffers(true /*recurse*/);
    for (auto& val : params) {
        if (!std::regex_match(val.key(), m, re)) {
            archive.read(val.key(), val.value());
        }
    }
    for (auto& val : buffers) {
        if (!std::regex_match(val.key(), m, re)) {
            archive.read(val.key(), val.value(), /*is_buffer*/ true);
        }
    }
}
bool Learner::isEmpty(at::Tensor x) {
    if (x.defined() && x.dim() > 0 && x.size(0) != 0 && x.numel() > 0)
        return false;
    else
        return true;
}
#endif //TRACK_RACING_LEARNER_H
