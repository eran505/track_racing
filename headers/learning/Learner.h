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
#include "ReplayBuffer/prioritizedExperienceReplay.hpp"
using namespace torch;
/*
 *   Q*(s,a) = reward + (is_done*(-1))*discount_factor*(T(s,a,s')V(s'))
 *   loss (Q*-Q(s,a))
 *   if(fix time update)
 *      targetNet <-- evalNet
 * */
void print_tensor_size(const torch::Tensor& x);
class Learner{
    double discountedFactor;
    neuralNet* evalNet;   // this Net chooses the best action (Policy-net)
    neuralNet* targetNet; // this Net does the T(s,a,s') calculation
    unsigned int batchSizeEntries;
    bool isDoubleNet;
    bool configNet;
    double curError;
    unsigned int ctrUpdate=0;

    prioritizedExperienceReplay* buffer;
    unordered_map<unsigned long,vector<vector<double>>> netData;
    unsigned int ctrEval=0;
    unsigned int updateEvery=20;
    unsigned int siwchNetEvery;
    unsigned int updateCtr;

    string home;
    dictionary dictInfo= nullptr;
public:
    void setInfoDict(dictionary ptrD) {dictInfo=ptrD;}
    bool epslionGreedy= false;
    void updateNetWork();
    void uploadNet();
    void preTrainNet(vector<double>* state,vector<double> *targetY);
    Learner(bool isDoubleNetwork, int sizeFeatuersIn, int batchSize,double discounterF,string &home,bool uploadNet);
    ~Learner();
    int predictValue(vector<double> *state,bool isRandom);
    void updateNet();
    void insertToBuffer(experienceTuple *Item , double errorItem);
    void loadStateDict(const torch::nn::Module& moduleEval,
            torch::nn::Module& moduleTarget, const std::string& ignore_name_regex);
    static bool isEmpty(at::Tensor x);
    static void LoadStateDict(torch::nn::Module& module,
                       const std::string& file_name,
                       const std::string& ignore_name_regex);
    static void SaveStateDict(const torch::nn::Module& module,
                                const std::string& file_name);
    double computerError(experienceTuple *exp,bool learn);
    static unsigned long hashValueMe(vector<double> &vec);


    private:
    Tensor calcQtraget(const experienceTuple *exp);
    void toCsvDict(string &pathFile, unordered_map<unsigned long,vector<vector<double>>> &netData);


};

Learner::Learner(bool isDoubleNetwork,int sizeFeatuersIn,int batchSize,double discounterF,string &home,bool isUploadNet):
batchSizeEntries(batchSize),evalNet(new neuralNet(sizeFeatuersIn)),updateCtr(0),
targetNet(evalNet),home(home),isDoubleNet(isDoubleNetwork),discountedFactor(discounterF)
,siwchNetEvery(400)
{
    configNet= false;
    if (isDoubleNetwork)
        this->targetNet = new neuralNet(sizeFeatuersIn);
    if (isUploadNet)
    {
        uploadNet();
    }
    buffer = new prioritizedExperienceReplay(1000,batchSize*2); //4000 == bug !!!
}

int Learner::predictValue(vector<double> *state, bool isRandom=false) {
    ctrEval++;
    if (isRandom)
        return range_random(0,26);

    if (epslionGreedy)
            if (range_random(0,9)==0)
            return range_random(0,26);

    ArrayRef<double> xx = *state;
    auto Sstate = torch::tensor(xx);
    auto tensorVector = this->evalNet->evalArgMaxQ(Sstate);
    vector<double > debugVec;
/** ----------------- debug -----------------*/
    if (ctrEval%500==0 or ctrEval%500==1 or ctrEval%500==2)
    {
        auto hashID = hashValueMe(*state);
//        cout<<"state:\t";
//        for (const auto item :*state) cout<<","<<item;
//        cout<<"\nhashID:\t"<<hashID<<endl;
        for (int i = 0; i < 27; ++i)
            debugVec.push_back(tensorVector[i].item().toFloat());
//        auto vecK = getTopK(3,debugVec);
//        for(int k=0;k<vecK.size();k=k+2)
//            cout<<"ID: "<<hashID<<"\t[ "<<vecK[k]<<" ] = "<<vecK[k+1]<<endl;
//        cout<<"---------"<<endl;
        netData[hashID].push_back(debugVec);
    }
    if (ctrEval%4000==0)
    {
        auto pathP = this->home+"/car_model/debug/";
        toCsvDict(pathP,this->netData);
    }
/** ----------------- debug -----------------*/

    auto res = tensorVector.argmax(0);

    return res.item().toInt();
}

double Learner::computerError(experienceTuple *exp,bool learnPhase=true)
{
    ArrayRef<double> x;
    double sum_of_elems;
    vector<double> expReward;
    auto QMaxValues = this->calcQtraget(exp);
    x = *exp->ptrProbabilities;
    auto probList = torch::tensor(x);
    ArrayRef<short> y = exp->isEndStateNot;
    auto isNotEndState = torch::tensor(y);
    x = *exp->ptrRewards;
    auto rewardVec  = torch::tensor(x);
//    cout<<"QMaxValues.sizes() \n"<<QMaxValues<<endl;
//    cout<<"probList.sizes()\n"<<probList<<endl;
//    cout<<"isNotEndState.sizes()\n "<<isNotEndState<<endl;
//    cout<<"rewardVec.sizes()\n"<<rewardVec<<endl;
        /*  Q*=r+dis_factor*T(s,a,s)*V(s')  */

    auto qTensor = QMaxValues*probList*discountedFactor*isNotEndState+rewardVec;
    auto qTensorSum = torch::sum(qTensor);
//    cout<<"bellman:\t"<<qTensor<<endl;

    auto valueCurState = this->evalNet->getActionStateValue(exp->ptrState,-1);
    auto actionIndex = exp->aAction;
    auto QTargetNext = valueCurState.clone().detach();
//    cout <<"evalNet: "<< valueCurState[int(actionIndex)]<<endl;

    QTargetNext[int(actionIndex)]=qTensorSum.item().toFloat();

    auto errordouble = abs(valueCurState[int(actionIndex)].item().toFloat()-QTargetNext[int(actionIndex)].item().toFloat());
    if (learnPhase)
        this->evalNet->learn(valueCurState,QTargetNext);
    return errordouble;
}
void Learner::insertToBuffer(experienceTuple *item , double errorItem){
    this->buffer->add(errorItem,item);
}
void Learner::updateNet() {
    if (configNet || !this->buffer->readyToLearn())
        return;
    ctrUpdate++;
    if (ctrUpdate%updateEvery!=0)
        return;
    //sample

    this->buffer->sample(this->batchSizeEntries);
    //this->buffer->numPostiveReward();
    vector<double> errVec;
    for(auto & ptrItem : buffer->batchSampleData)
    {
//        if (ptrItem== nullptr)
//            continue;
        auto errorNew = this->computerError(ptrItem, true);
        errVec.push_back(errorNew);
    }
    buffer->updatePriorities(buffer->batchSampleIndex,errVec);
    updateCtr++;
    if (updateCtr%siwchNetEvery==0 and isDoubleNet)
    {
        this->updateNetWork();
    }
}

void Learner::uploadNet()
{
    cout<<"**** UploadNet ****"<<endl;
    auto model_save_path=this->home+"/car_model/nn/nn.pt";
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
    auto name ="nn_"+dictInfo->operator[]("ID")+".pt";
    auto model_save_path=this->home+"/car_model/nn/"+name;
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
    delete this->buffer;

}

Tensor Learner::calcQtraget(const experienceTuple *exp) {
    vector<double> nNextStateExpectedReward;
    auto x = torch::zeros({0,0});
    torch::Tensor Qtarget;
    bool isFirst=true;
    // This scope calc the expected value of the transition
    vector<at::Tensor> l ;
    for (const auto nNextItem :  exp->ptrNextStateVec) {
        ArrayRef<double> xx = *nNextItem;
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

void Learner::preTrainNet(vector<double>* state,vector<double> *targetY) {

    auto predictY = this->evalNet->getActionStateValue(state,-1);
    ArrayRef<double> xx = *targetY;
    auto hTarget = torch::tensor(xx);
//    cout << "hTarget: "<<hTarget << endl;
//    cout << "predictY: " <<predictY<<endl;
    this->evalNet->learn(predictY, hTarget);
    delete (targetY);

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


unsigned long Learner::hashValueMe(vector<double> &vec){

    unsigned long seed = vec.size();
    for(auto& i : vec) {
        seed ^= long(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

void Learner::toCsvDict(string &pathFile, unordered_map<unsigned long, vector<vector<double>>> &netData) {
    try {
        for (const auto &item : netData) {
            auto pathFileI = pathFile + std::to_string(item.first) + ".csv";
            csvfile csv(move(pathFileI), ",");

            for (const auto &vecI :item.second) {
                for (auto row:vecI) {
                    csv << row;
                }
                csv << endrow;
            }
        }
        netData.clear();
    }catch (const std::exception &ex) {std::cout << "Exception was thrown: " << ex.what() << std::endl;}

}



#endif //TRACK_RACING_LEARNER_H
