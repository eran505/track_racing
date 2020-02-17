//
// Created by ise on 26.1.2020.
//

#ifndef TRACK_RACING_REPLAYBUFFER_HPP
#define TRACK_RACING_REPLAYBUFFER_HPP

#include "State.hpp"
#include "util_game.hpp"
typedef vector<float> feature;
class ReplayBuffer{
public:

    vector<feature*> aAction;
    vector<feature*> rRewardNextStates;
    vector<feature*> pProbabilityNextStates;
    vector<vector<feature*>> nNextStates;
    vector<feature*> stateS;
    unsigned int memSize;
    unsigned int featureSize;
    unsigned int ctrInd;
    unsigned int loopNumber;

public:
    ReplayBuffer(unsigned int _memSize, unsigned int _featureSize): aAction(_memSize),rRewardNextStates(_memSize),
    pProbabilityNextStates(_memSize),nNextStates(_memSize),stateS(_memSize){
        ctrInd=0;
        loopNumber=0;
        this->memSize=_memSize;
        this->featureSize=_featureSize;
    }
    void addBuffer(feature* refProbability,
                   feature* refReward,vector<feature*>& refNextSate,
                   feature* actionVec,feature* stateVec){

        addItems(aAction,actionVec);
        addItems(this->stateS,stateVec);

        addItems(this->rRewardNextStates,refReward);
        addItems(this->pProbabilityNextStates,refProbability);
        addItems(this->nNextStates,refNextSate);
        ctrInd++;
        if (ctrInd>=this->memSize)
        {
            ctrInd=0;
            loopNumber++;
        }
    }
    void addItems(vector<vector<feature*>>& vecMem, vector<feature*>& newItem )
    {
        auto vecTPos = vecMem.begin()+ctrInd;
        auto vecPtrOld = vecMem.operator[](ctrInd);
        for (auto item:vecPtrOld)
            delete item;
        vecMem.insert(vecTPos,newItem);
    }
    void addItems(vector<feature*>& vecMem, feature* newItem ){
        auto vecTPos = vecMem.begin()+ctrInd;
        auto vecPtrOld = vecMem.operator[](ctrInd);
        delete vecPtrOld;
        vecMem.operator[](ctrInd)=newItem;
    }
    void addItemOne(vector<feature*>* vecMem, feature* newItem )
    {
        auto itPos = vecMem->begin() + ctrInd;
        auto ptrOld = vecMem->operator[](ctrInd);
        delete ptrOld;
        vecMem->insert(itPos,newItem);
    }
    void sampleEntries(int size,unordered_set<int> &hashList)
    {
        //unordered_set<int> hashList;
        if (size>=memSize)
            throw;
        int ctr=0;
        int lim = int(memSize)*2;
        while(hashList.size()<size && ctr++ < lim)
        {
            auto entry = range_random(0,int(memSize)-1);
            hashList.insert(entry);
        }
    }
    bool isSufficientAmountExperience()
    {
        if (this->loopNumber>0)
            return true;
        if (this->ctrInd>100)
            return true;
        return false;
    }

};


#endif //TRACK_RACING_REPLAYBUFFER_HPP
