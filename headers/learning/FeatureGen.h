//
// Created by ERANHER on 21.1.2020.
//

#ifndef TRACK_RACING_FEATUREGEN_H
#define TRACK_RACING_FEATUREGEN_H

#include "util_game.hpp"
#include "State.hpp"


/*
 * [[si,sj]]
 */

class FeatureGen{
    string uAgentId;
    const string uOppId="0A";
    int sizeVec= int(Point::D_point::D)*4+1;

public:
    FeatureGen(string myId ):uAgentId(std::move(myId)){

    }
    ~FeatureGen()= default;



    vector<float>* getFeaturesSA( State* s,const Point& actionA){
        auto vec = this->getFeaturesS(s);
        for (int i = 0; i < Point::D_point::D; ++i) {
            vec->push_back(actionA[i]);
        }
        return vec;
    }

    vector<float>* getFeaturesS( State* s){
        int sizePoint  = Point::D_point::D;
        auto posAgent = s->get_position(this->uAgentId);
        auto posAdv = s->get_position(this->uOppId);
        auto speedAgent = s->get_speed(this->uAgentId);
        auto speedAdv = s->get_speed(this->uOppId);
        auto budgetAgent = s->get_budget(this->uAgentId);
        auto budgetAdv = s->get_budget(this->uOppId);
        auto dist = posAgent-posAdv;
        auto* vec = new vector<float>(this->sizeVec);
        int index=0;

        for(int i = 0; i < sizePoint; ++i)
        {
            vec->operator[](i+index)=posAgent[i];
        }
        index+=sizePoint;
        for(int i = 0; i < sizePoint; ++i)
        {
            vec->operator[](i+index)=posAdv[i];
        }
        index+=sizePoint;
        for (int i = 0; i < sizePoint; ++i) {
            vec->operator[](i+index)=speedAgent[i];
        }
        index+=sizePoint;
        for (int i = 0; i < sizePoint; ++i) {
            vec->operator[](i+index)=speedAdv[i];
        }
        index+=sizePoint;
        vec->operator[](index)=budgetAgent;

        return vec ;
    }

};


#endif //TRACK_RACING_FEATUREGEN_H
