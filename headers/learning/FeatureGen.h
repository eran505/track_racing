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
    State::agentEnum uAgentId=State::agentEnum::D;
    State::agentEnum uOppId=State::agentEnum::A;
    int sizeVec;
    string home;
    int indexer;
    int maxSpeed;
    unordered_map<int,Point*>* actionMap;
    unordered_map<unsigned long,string> mapStateHash;

public:
    void set_string_home(string &stringHome){this->home=stringHome;}
    FeatureGen(int _numOfGoals,int _maxSpeed):sizeVec(0),indexer(0),actionMap(nullptr){
        this->sizeVec+=int(Point::D_point::D)*5;
        this->sizeVec+=_numOfGoals*int(Point::D_point::D)*2;
        actionMap=Point::getDictAction();
        this->maxSpeed=_maxSpeed;
    }
    ~FeatureGen(){
        for (auto &item : *this->actionMap)
            delete(item.second);
    };

    int getFeatureVecSize(){ return this->sizeVec;}
    vector<double>* getFeaturesSA( State* s,const Point& actionA){
        auto vec = this->getFeaturesS(s);
        for (int i = 0; i < Point::D_point::D; ++i) {
            vec->push_back(actionA[i]);
        }
        return vec;
    }
    void insetPoint(const Point &p,vector<double>* vec)
    {
        for(int i = 0; i < Point::D_point::D; ++i)
        {
            vec->operator[](i+indexer)=p[i];
        }
        indexer+=int(Point::D_point::D);
    }
    void distWall(Point &size,Point &pos,vector<double> *vec){
        Point dis = size-pos;
        insetPoint(dis,vec);  // from gird wall
        insetPoint(pos,vec); //  from zero


    }

    void distFirstMinusSec(const Point &F,const Point &S,vector<double> *vec){
        Point dis = F-S;
        insetPoint(dis,vec);
    }

    void distOpAbs(const Point &attackPos,const Point &attackDef,vector<double> *vec){
        Point dis = attackPos-attackDef;
        for(int i = 0; i < Point::D_point::D; ++i){
            dis.array[i] = abs(dis.array[i]);
        }
        insetPoint(dis,vec);
    }

    void isGettingCloser(vector<double> *vec,State *s){
        for (auto const actionI : *actionMap)
        {

        }
    }


    vector<double>* getFeaturesS(State* s){
        auto size_grid = s->g_grid->getPointSzie();

        auto goalz = s->g_grid->get_goals();
        int sizePoint  = Point::D_point::D;
        auto posAgent = s->get_position_ref(this->uAgentId);
        auto posAdv = s->get_position_ref(this->uOppId);
        auto speedAgent = s->get_speed(this->uAgentId);
        auto speedAdv = s->get_speed(this->uOppId);
        auto budgetAgent = s->get_budget(this->uAgentId);
        auto budgetAdv = s->get_budget(this->uOppId);
        auto* vec = new vector<double>(this->sizeVec);


        indexer=0;
        distWall(size_grid,posAgent,vec); // 1 - distance for walls (2)
        insetPoint(speedAgent,vec);// 3 - speed A
        insetPoint(speedAdv,vec);// 4 - speed D
        distFirstMinusSec(posAgent,posAdv,vec);

        for (auto const& goalIdx : goalz)
        {
            distFirstMinusSec(posAdv,goalIdx,vec);
            distFirstMinusSec(posAgent,goalIdx,vec);
            //insetPoint(*goalIdx,vec);
        }

        return vec ;
    }
    static unsigned long hashValueMe(vector<double> &vec){

        unsigned long seed = vec.size();
        for(auto& i : vec) {
            seed ^= long(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }




};


#endif //TRACK_RACING_FEATUREGEN_H
