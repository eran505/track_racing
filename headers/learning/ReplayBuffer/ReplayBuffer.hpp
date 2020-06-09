//
// Created by ise on 26.1.2020.
//

#ifndef TRACK_RACING_REPLAYBUFFER_HPP
#define TRACK_RACING_REPLAYBUFFER_HPP

#include "State.hpp"
#include "util_game.hpp"
typedef vector<double> feature;
class ReplayBuffer{
public:

    vector<unsigned int> aAction;
    vector<feature*> rRewardNextStates;
    vector<feature*> pProbabilityNextStates;
    vector<vector<feature*>> nNextStates;
    vector<feature*> stateS;
    vector<feature*> isEndStateNot;
    unsigned int memSize;

    unsigned int ctrInd;
    unsigned int loopNumber;

public:
    explicit ReplayBuffer(unsigned int _memSize): aAction(_memSize),isEndStateNot(_memSize),rRewardNextStates(_memSize),
    pProbabilityNextStates(_memSize),nNextStates(_memSize),stateS(_memSize){
        ctrInd=0;
        loopNumber=0;
        this->memSize=_memSize;
    }

    void addBuffer(feature* refProbability,
            feature* refReward,vector<feature*>& refNextSate,
                   int actionVec,feature* stateVec){


        isNOTEndAddItems(*refReward);
        addItems(actionVec);
        addItems(this->stateS,stateVec);
        addItems(this->rRewardNextStates,refReward);
        addItems(this->pProbabilityNextStates,refProbability);
        addItems(this->nNextStates,refNextSate);
        ctrInd++;

        if (ctrInd==this->memSize)
        {
            ctrInd=0;
            loopNumber++;
        }
    }
    void addItems(int actionIndex)
    {
        aAction[ctrInd]=actionIndex;
    }
    void addItems(vector<vector<feature*>>& vecMem, vector<feature*>& newItem )
    {
        auto vecPtrOld = vecMem[ctrInd];
        for (auto item:vecPtrOld)
            delete item;
        vecMem[ctrInd]=newItem;
    }


    void isNOTEndAddItems(const feature& refReward)
    {
        auto* newItemIndx= new feature();
        for (auto const item: refReward)
        {
            if (item==0)
                newItemIndx->push_back(1);
            else
                newItemIndx->push_back(0);
        }
        auto vecPtrOld = this->isEndStateNot[ctrInd];
        delete vecPtrOld;
        this->isEndStateNot[ctrInd]=newItemIndx;
    }

    void addItems(vector<feature*>& vecMem, feature* newItem ){
        auto vecPtrOld = vecMem[ctrInd];
        delete vecPtrOld;
        vecMem[ctrInd]=newItem;
    }
    void addItemOne(vector<feature*>* vecMem, feature* newItem )
    {
        auto itPos = vecMem->begin() + ctrInd;
        auto ptrOld = vecMem->operator[](ctrInd);
        delete ptrOld;
        vecMem->insert(itPos,newItem);
    }
    void sampleEntries(unsigned int size,std::unordered_set<int> &hashList) const
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
    bool isSufficientAmountExperience() const
    {
        if (this->loopNumber>0)
            return true;
//        if (this->ctrInd>100)
//            return true;
        return false;
    }

};


#endif //TRACK_RACING_REPLAYBUFFER_HPP
