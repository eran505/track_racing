//
// Created by ise on 26.1.2020.
//

#ifndef TRACK_RACING_REPLAYBUFFER_HPP
#define TRACK_RACING_REPLAYBUFFER_HPP

#include "State.hpp"
#include "util_game.hpp"
typedef vector<double>* feature;
class ReplayBuffer{
    vector<feature>* aAction;
    vector<vector<feature>>* rRewardNextStates;
    vector<vector<feature>>* pProbabilityNextStates;
    vector<vector<feature>>* nNextStates;
    vector<feature>* stateS;
    unsigned int memSize;
    unsigned int featureSize;
    unsigned int ctrInd;
    int loopNumber;

public:
    ReplayBuffer(unsigned int _memSize, unsigned int _featureSize){
        this->memSize=_memSize;
        this->stateS=new vector<feature>(memSize);
        this->featureSize=_featureSize;
        this->aAction = new vector<feature>(memSize);
        this->rRewardNextStates=new vector<vector<feature>>(memSize);
        this->pProbabilityNextStates=new vector<vector<feature>>(memSize);
        this->nNextStates=new vector<vector<feature>>(memSize);
    }
    void addBuffer(vector<feature>& refProbability,
                   vector<feature>& refReward,vector<feature>& refNextSate,
                   feature actionVec,feature stateVec){


        addItemOne(this->aAction,actionVec);
        addItemOne(this->stateS,stateVec);

        addItems(this->rRewardNextStates,refReward);
        addItems(this->pProbabilityNextStates,refProbability);
        addItems(this->rRewardNextStates,refNextSate);
        ctrInd++;
        if (ctrInd>=this->memSize)
        {
            ctrInd=0;
            loopNumber++;
        }
    }
    void addItems(vector<vector<feature>>* vecMem, vector<feature>& newItem )
    {
        auto vecTPos = vecMem->begin()+ctrInd;
        auto vecPtrOld = vecMem->operator[](ctrInd);
        for (auto item:vecPtrOld)
            delete item;
        vecMem->insert(vecTPos,newItem);
    }
    void addItemOne(vector<feature>* vecMem, feature newItem )
    {
        auto itPos = vecMem->begin() + ctrInd;
        auto ptrOld = vecMem->operator[](ctrInd);
        delete ptrOld;
        vecMem->insert(itPos,newItem);
    }



};


#endif //TRACK_RACING_REPLAYBUFFER_HPP
