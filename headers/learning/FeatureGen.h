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
    int sizeVec;
    int indexer;
    unordered_map<int,Point*>* actionMap;
public:
    FeatureGen(string myId, int _numOfGoals):sizeVec(0),uAgentId(std::move(myId)),indexer(0),actionMap(nullptr){
        this->sizeVec+=int(Point::D_point::D)*7+1;
        this->sizeVec+=_numOfGoals*int(Point::D_point::D);
        actionMap=Point::getDictAction();
    }
    ~FeatureGen(){
        for (auto &item : *this->actionMap)
            delete(item.second);
    };
    int getFeatureVecSize(){ return this->sizeVec;}
    vector<float>* getFeaturesSA( State* s,const Point& actionA){
        auto vec = this->getFeaturesS(s);
        for (int i = 0; i < Point::D_point::D; ++i) {
            vec->push_back(actionA[i]);
        }
        return vec;
    }
    void insetPoint(const Point &p,vector<float>* vec)
    {
        for(int i = 0; i < Point::D_point::D; ++i)
        {
            vec->operator[](i+indexer)=p[i];
        }
        indexer+=int(Point::D_point::D);
    }
    void distWall(Point &size,Point &pos,vector<float> *vec){
        Point dis = size-pos;
        insetPoint(dis,vec);
    }

    void distOpAbs(const Point &attackPos,const Point &attackDef,vector<float> *vec){
        Point dis = attackPos-attackDef;
        for(int i = 0; i < Point::D_point::D; ++i){
            dis.array[i] = abs(dis.array[i]);
        }
        insetPoint(dis,vec);
    }


    vector<float>* getFeaturesS(State* s){
        auto size_grid = s->g_grid->getPointSzie();
        auto goalz = s->g_grid->get_goals();
        int sizePoint  = Point::D_point::D;
        auto posAgent = s->get_position(this->uAgentId);
        auto posAdv = s->get_position(this->uOppId);
        auto speedAgent = s->get_speed(this->uAgentId);
        auto speedAdv = s->get_speed(this->uOppId);
        auto budgetAgent = s->get_budget(this->uAgentId);
        auto budgetAdv = s->get_budget(this->uOppId);
        auto* vec = new vector<float>(this->sizeVec);


        indexer=0;
        insetPoint(size_grid,vec);
        insetPoint(posAgent,vec);
        insetPoint(posAdv,vec);
        insetPoint(speedAgent,vec);
        insetPoint(speedAdv,vec);
        distWall(size_grid,posAgent,vec);
        distOpAbs(posAdv,posAgent,vec);
        vec->operator[](indexer)=budgetAgent;
        for (auto const goalIdx : goalz)
        {
            insetPoint(*goalIdx,vec);
        }



        return vec ;
    }

};


#endif //TRACK_RACING_FEATUREGEN_H
