//
// Created by eranhe on 10/05/2020.
//

#ifndef TRACK_RACING_ABSTRACTIONDIV_H
#define TRACK_RACING_ABSTRACTIONDIV_H

#include <sys/param.h>
#include "util_game.hpp"
#include "serach/Astar.hpp"
#include "Policy/PathPolicy.hpp"
#include <deque>
#define DEBUG

typedef unordered_map<u_int64_t,vector<float>*> dictPolicyPath;
typedef std::vector<std::vector<AStar::StatePoint>*> doubleVecPoint;
typedef std::unordered_map<u_int64_t ,pair<short,AStar::StatePoint>> hashIdStates;
typedef vector<tuple<Point*,double>> listPointWeightedd;
using AStar::StatePoint;

class abstractionDiv{

    vector<dictPolicyPath*> vecPolicy;
    dictPolicyPath* allDictPolicy;
    hashIdStates* dictHash;
    doubleVecPoint *startPoints_abstraction;
    doubleVecPoint *endPoints_abstraction;
    Point girdSize;
    Point abstractSize;
    unordered_map<int,Point*>* hashActionDict;
public:
    ~abstractionDiv()
    {
        for(auto item : *hashActionDict)
            delete item.second;
        delete hashActionDict;
        for (auto item : *startPoints_abstraction)
            delete item;
        for(auto item : *endPoints_abstraction)
            delete item;
        delete startPoints_abstraction;
        delete endPoints_abstraction;
        delete dictHash;
    }
    abstractionDiv(const Point& ptrGirdSize,const Point& mAbstractSize,PathPolicy *policyP){
        auto sizeMiniGrid = mAbstractSize.array[0]*mAbstractSize.array[1];
        abstractSize=mAbstractSize;
        girdSize = ptrGirdSize;

        allDictPolicy=policyP->getDictPolicy();
        //auto number2D = girdSize.array[0]*girdSize.array[1];
        vecPolicy = vector<dictPolicyPath*>(sizeMiniGrid);
        startPoints_abstraction = new doubleVecPoint(sizeMiniGrid);
        endPoints_abstraction= new doubleVecPoint(sizeMiniGrid);
        for(size_t i=0;i<sizeMiniGrid;++i)
        {
            vecPolicy[i]=new dictPolicyPath();
            startPoints_abstraction->operator[](i)= new std::vector<AStar::StatePoint>();
            endPoints_abstraction->operator[](i)= new std::vector<AStar::StatePoint>();
        }
        dictHash= policyP->statesIdDict;
        hashActionDict= Point::getDictAction();
        initAbstract();
        setEndStartPoint(policyP->startPoint,policyP->goalPoint,policyP->max_speed);
        std::cout<<endl;
    }

private:
    void setDictHashState(hashIdStates* mDictHash){ dictHash=mDictHash;}
    void setEndStartPoint(listPointWeighted* start, listPointWeighted* end,int maxSpeed)
    {
        for(auto &[b,a] : *start) {
            auto s = Point(a);
            auto idx = emplaceInDictVec(s);
            startPoints_abstraction->operator[](idx)->emplace_back(std::move(s),Point(0,0,maxSpeed));
        }
        for(auto &[a,b] : *end) {
            auto e = Point(a);
            auto idx = emplaceInDictVec(e);
            endPoints_abstraction->operator[](idx)->emplace_back(std::move(e),Point(0));
        }

    }
    static void insetToList(std::vector<AStar::StatePoint> *l , AStar::StatePoint &s){
        if (std::find(l->begin(), l->end(), s) == l->end())
            l->push_back(std::move(s));

    }
    static inline u_int64_t getIndexEntry(StatePoint &s){
        auto hSpeed = s.get_speed().hashConst(Point::maxSpeed);
        auto hPos = s.get_position().hashConst();
        auto entryIdx = Point::hashNnN(hPos, hSpeed);
        return entryIdx;
    }
    int emplaceInDictVec(const Point &statePos)
    {
        auto row = statePos.array[0]/abstractSize.array[0];
        auto col = statePos.array[1]/abstractSize.array[1];
        //std::cout<<state.pos.to_str()<<" : "<<(abstractSize.array[0])*row+col<<endl;

        return (abstractSize.array[0])*row+col;
    }
    //TODO: case: what if point is a goal and also part of a path? 12/5
    void initAbstract(){
        // cycle over all starting point
        for (const auto &item: *dictHash)
        {
            //cout<<"in\n";
            auto idx = emplaceInDictVec(item.second.second.pos);
            auto pos = allDictPolicy->find(item.first);
            if (pos==allDictPolicy->end())
                throw;
            vecPolicy[idx]->emplace(pos->first,pos->second);
            auto statS = item.second.second;
            auto vecNext = pos->second;
            auto statSOld = StatePoint(statS);
            for (size_t k=0;k<vecNext->size()-1;k+=2)
            {
                auto action = hashActionDict->operator[](vecNext->operator[](k));

                statS.speed.operator+=(*action);
                statS.pos.operator+=(statS.speed);
                auto newIdx = emplaceInDictVec(statS.pos);
                auto newID = getIndexEntry(statS);
                if (newIdx != idx)
                {
                    auto tmp = StatePoint(statSOld);
                    insetToList(startPoints_abstraction->operator[](newIdx),statS);
                    insetToList(endPoints_abstraction->operator[](idx),tmp);

                }
                statS = StatePoint(statSOld);


            }
        }

    }


};





#endif //TRACK_RACING_ABSTRACTIONDIV_H


