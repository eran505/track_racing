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
void print_tensor_size(const torch::Tensor& x);
class Learner{
    float discountedFactor;
    neuralNet* evalNet;   // this Net chooses the best action (Policy-net)
    neuralNet* targetNet; // this Net does the T(s,a,s') calculation
    unsigned int batchSizeEntries;
    bool isDoubleNet;
    bool configNet;
    unsigned int ctrUpdate=0;

    unsigned int ctrEval=0;
    unsigned int updateEvery=100;
    unsigned int siwchNetEvery;
    unsigned int updateCtr;
    string home;

public:
    void updateNetWork();
    void uploadNet();
    void preTrainNet(vector<float>* state,vector<float> *targetY);
    Learner(bool isDoubleNetwork, int sizeFeatuersIn, int batchSize,float discounterF,string &home,bool uploadNet);
    ~Learner();
    int predictValue(vector<float> *state,bool isRandom);
    void updateNet(const ReplayBuffer *buffer);
    void loadStateDict(const torch::nn::Module& moduleEval,
            torch::nn::Module& moduleTarget, const std::string& ignore_name_regex);
    static bool isEmpty(at::Tensor x);
    static void LoadStateDict(torch::nn::Module& module,
                       const std::string& file_name,
                       const std::string& ignore_name_regex);
    static void SaveStateDict(const torch::nn::Module& module,
                                const std::string& file_name);

private:
    Tensor calcQtraget(const ReplayBuffer *buffer,int index);

};

Learner::Learner(bool isDoubleNetwork,int sizeFeatuersIn,int batchSize,float discounterF,string &home,bool isUploadNet):
batchSizeEntries(batchSize),evalNet(new neuralNet(sizeFeatuersIn)),updateCtr(0),
targetNet(evalNet),home(home),isDoubleNet(isDoubleNetwork),discountedFactor(discounterF)
,siwchNetEvery(50)
{
    configNet= false;
    if (isDoubleNetwork)
        this->targetNet = new neuralNet(sizeFeatuersIn);
    if (isUploadNet)
    {
        uploadNet();
    }

}

int Learner::predictValue(vector<float> *state, bool isRandom=false) {
    ctrEval++;
    if (isRandom)
        return range_random(0,26);
    ArrayRef<float> xx = *state;
    auto Sstate = torch::tensor(xx);
    return this->evalNet->evalArgMaxQ(Sstate);
}
void Learner::updateNet(const ReplayBuffer *buffer) {
    if (!buffer->isSufficientAmountExperience() || configNet)
        return;
    ctrUpdate++;
    if (ctrUpdate%updateEvery!=0)
        return;
    //random samples
    unordered_set<int> entries;

    buffer->sampleEntries(batchSizeEntries,entries);
    for (const auto entryIndx:entries)
    {
        for (const auto val : *buffer->rRewardNextStates[entryIndx])
        {
            if (val>0)
                cout<<endl;
        }
        float sum_of_elems;
        vector<float> expReward;
        auto QMaxValues = this->calcQtraget(buffer,entryIndx);
        auto probList = torch::tensor(*buffer->pProbabilityNextStates[entryIndx]);
        auto isNotEndState = torch::tensor(*buffer->isEndStateNot[entryIndx]);
        auto rewardVec  = torch::tensor(*buffer->rRewardNextStates[entryIndx]);
//        cout<<"QMaxValues.sizes() \n"<<QMaxValues<<endl;
//        cout<<"probList.sizes()\n"<<probList<<endl;
//        cout<<"isNotEndState.sizes()\n "<<isNotEndState<<endl;
//        cout<<"rewardVec.sizes()\n"<<rewardVec<<endl;
        // Q*=r+dis_factor*T(s,a,s)*V(s')

        auto qTensor = QMaxValues*probList*discountedFactor*isNotEndState+rewardVec;
//        cout<<"bellman:\t"<<qTensor<<endl;

        auto valueCurState = this->evalNet->getActionStateValue(buffer->stateS[entryIndx],-1);
        auto actionIndex = buffer->aAction[entryIndx];
        auto QTargetNext = valueCurState.clone().detach();
        //cout <<"evalNet: "<< valueCurState[int(actionIndex)]<<endl;

        QTargetNext[int(actionIndex)]=qTensor.item().toFloat();
        this->evalNet->learn(valueCurState,QTargetNext);

    }
    updateCtr++;
    if (updateCtr%siwchNetEvery==0 and isDoubleNet)
    {
        this->updateNetWork();
    }




}

void Learner::uploadNet()
{
    cout<<"**** UploadNet ****"<<endl;
    auto model_save_path=this->home+"/car_model/nn/upload_nn.pt";
    auto* pModule = (torch::nn::Module*)this->evalNet;
    LoadStateDict(*pModule, model_save_path, "none");
    if (!isDoubleNet)
        return;
    pModule = (torch::nn::Module*)this->targetNet;
    LoadStateDict(*pModule, model_save_path, "none");

}
void Learner::updateNetWork()
{
    updateCtr=0;
    cout<<"updateNetWork"<<endl;
    auto model_save_path=this->home+"/car_model/nn/nn.pt";
    auto* pModule = (torch::nn::Module*)this->evalNet;
    SaveStateDict(*pModule, model_save_path);
    pModule = (torch::nn::Module*)this->targetNet;
    LoadStateDict(*pModule, model_save_path, "none");
    // Save the model
    // torch::save(pModule, model_save_path);
    // Load the model
    // torch::load(model, model_save_path);
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

void Learner::loadStateDict(const torch::nn::Module& moduleEval,torch::nn::Module& moduleTarget,
                            const std::string& ignore_name_regex) {
    //TODO: impl this function using the load and save function.
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

void Learner::preTrainNet(vector<float>* state,vector<float> *targetY) {

    auto predictY = this->evalNet->getActionStateValue(state,-1);
    ArrayRef<float> xx = *targetY;
    auto hTarget = torch::tensor(xx);
//    cout << "hTarget: "<<hTarget << endl;
//    cout << "predictY: " <<predictY<<endl;
    this->evalNet->learn(predictY, hTarget);

}
void print_tensor_size(const torch::Tensor& x) {
    std::cout << "[";
    for (size_t i = 0; i != x.dim() - 1; ++i) {
        std::cout << x.size(i) << " ";
    }
    std::cout << x.size(-1) << "]";
}

void print_script_module(const torch::jit::script::Module& module, size_t spaces) {
    for (const auto& sub_module : module.named_children()) {
        if (!sub_module.name.empty()) {
            std::cout << std::string(spaces, ' ') << sub_module.value.type()->name().value().name()
                      << " " << sub_module.name << "\n";
        }

        print_script_module(sub_module.value, spaces + 2);
    }
}
#endif //TRACK_RACING_LEARNER_H
