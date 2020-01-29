//
// Created by ise on 26.1.2020.
//

#ifndef TRACK_RACING_REPLAYBUFFER_HPP
#define TRACK_RACING_REPLAYBUFFER_HPP

#include "State.hpp"
#include "util_game.hpp"

class ReplayBuffer{
    vector<vector<double>*>* aAction;
    vector<vector<vector<double>>>* rRewardNextStates;
    vector<vector<vector<double>>>* pProbabilityNextStates;
    vector<vector<vector<double>>>* nNextStates;
    vector<vector<double>>* stateS;
    unsigned int memSize;
    unsigned int featureSize;

public:
    ReplayBuffer(unsigned int _memSize, unsigned int _featureSize){
        this->memSize=_memSize;
        this->stateS=new vector<vector<double>>(memSize);
        this->featureSize=_featureSize;
        this->aAction = new vector<vector<double>*>(memSize);
        this->rRewardNextStates=new vector<vector<vector<double>>>(memSize);
        this->pProbabilityNextStates=new vector<vector<vector<double>>>(memSize);
        this->nNextStates=new vector<vector<vector<double>>>(memSize);
    }

};


#endif //TRACK_RACING_REPLAYBUFFER_HPP
