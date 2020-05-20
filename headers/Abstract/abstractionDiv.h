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
#include <memory>
#include "Abstract/Simulation.hpp"
#include "Policy/RtdpAlgo.hpp"
#include "Grid.hpp"
#include "util/utilClass.hpp"
#define DEBUG

typedef unordered_map<u_int64_t,vector<float>*> dictPolicyPath;
typedef std::vector<std::vector<weightedPosition>> doubleVecPoint;
typedef std::unordered_map<u_int64_t ,pair<short,AStar::StatePoint>> hashIdStates;
typedef vector<tuple<Point*,double>> listPointWeightedd;
using AStar::StatePoint;

class abstractionDiv{
    int seedSystem;
    vector<dictPolicyPath*> vecPolicy;
    dictPolicyPath* allDictPolicy;
    hashIdStates* dictHash;
    doubleVecPoint *startPoints_abstraction;
    doubleVecPoint *endPoints_abstraction;
    Point girdSize;
    Point abstractSize;
    std::vector<std::vector<Point>> myPaths;
    unordered_map<int,Point*>* hashActionDict;
    int sizeVectors;
public:
    ~abstractionDiv()
    {
        for(auto item : *hashActionDict)
            delete item.second;
        delete hashActionDict;
        delete startPoints_abstraction;
        delete endPoints_abstraction;
        delete dictHash;
    }
    abstractionDiv(const Point& ptrGirdSize,const Point& mAbstractSize,PathPolicy *policyP,int seed){
        auto sizeMiniGrid = mAbstractSize.array[0]*mAbstractSize.array[1];
        sizeVectors=sizeMiniGrid;
        abstractSize=mAbstractSize;
        girdSize = ptrGirdSize;
        seedSystem = seed;
        allDictPolicy=policyP->getDictPolicy();
        //auto number2D = girdSize.array[0]*girdSize.array[1];
        vecPolicy = vector<dictPolicyPath*>(sizeMiniGrid+1);
        startPoints_abstraction = new doubleVecPoint(sizeMiniGrid+1);
        endPoints_abstraction= new doubleVecPoint(sizeMiniGrid+1);
        for(size_t i=0;i<vecPolicy.size();++i)
        {
            vecPolicy[i]=new dictPolicyPath();
            startPoints_abstraction->operator[](i)=std::vector<weightedPosition>();
            endPoints_abstraction->operator[](i)=std::vector<weightedPosition>();
        }
        dictHash= policyP->statesIdDict;
        myPaths = std::move(policyP->myPaths);
        hashActionDict= Point::getDictAction();
        initAbstract();
        setEndStartPoint(policyP->startPoint,policyP->goalPoint,policyP->max_speed);
        bigGridAbs();
        std::cout<<endl;

    }


    void bigGridAbs()
    {
        vector<vector<weightedPosition>> setPaths;
        for(auto & myPath : myPaths)
        {
            auto &l= setPaths.emplace_back();
            for(u_int32_t j=0;j<myPath.size();++j)
            {
                myPath[j]/=this->abstractSize;
                if (j>0)
                {

                    if (!(myPath[j]==myPath[j-1]))
                        l.emplace_back(myPath[j-1]-myPath[j],myPath[j],1);
                    else
                        l.back().weightedVal+=1;
                }
                else
                    l.emplace_back(Point(),myPath[j],1);
            }
        }
        make_dict_policy(setPaths);
        cout<<endl;
    }
    void make_dict_policy(vector<vector<weightedPosition>>& momvemtns)
    {
        vector<u_int64_t> speedPorbabilityVector;
        const auto index= vecPolicy.size()-1;
        for (auto& item : momvemtns){
            for(int i=0;i<item.size()-1;++i)
            {
                auto difAction =  item[i+1].speedPoint-item[i].speedPoint;
                float actionHkey = difAction.hashMeAction(Point::D_point::actionMax);
                u_int64_t key = Point::hashNnN(item[i+1].positionPoint.hashConst(),
                                               item[i+1].speedPoint.hashConst(Point::maxSpeed));
                if (auto pos = this->vecPolicy[index]->find(key);pos==this->vecPolicy[index]->end())
                {
                    this->vecPolicy[index]->try_emplace(key, new std::vector<float>({actionHkey,-1}));
                }
                else{
                    auto isFind = std::find(pos->second->begin(),pos->second->end(),actionHkey);
                    if(isFind==pos->second->end())
                        pos->second->insert(pos->second->end(),{actionHkey,-1});
                    else
                        {
                            auto b = std::distance(pos->second->begin(),isFind)+1;
                            pos->second->operator[](b)--;
                        }
                }
                if(std::find(speedPorbabilityVector.begin(),speedPorbabilityVector.end(),key)==speedPorbabilityVector.end())
                {
                    speedPorbabilityVector.push_back(key);
                    speedPorbabilityVector.push_back(item[i].weightedVal);
                }
            }
        }
        for(auto& item: *this->vecPolicy[index])
        {
            auto acc=0;
            int indexCur=0;
            std::for_each(item.second->begin(),item.second->end(),[&](float x){
                if(indexCur%2!=0)
                    acc+=x;
                indexCur++;});
            indexCur=0;
            for(auto &num:*item.second)
            {
                if (indexCur%2!=0) num=num/acc;
                indexCur++;
            }
        }

        cout<<"end"<<endl;
    }
    std::vector<simulation> initializeSimulation(configGame &conf){
        //vector<weightedPosition> l = {{Point(0),Point(0),1}};
        std::vector<simulation> simulationList;
        simulationList.reserve(this->sizeVectors);
        for (size_t k = 0 ; k<this->vecPolicy.size();++k) {
            if (this->vecPolicy[k]->empty())
                continue;
            auto [up,low]=getBound(k,this->abstractSize);
            auto listPosStart = startingPosDefender(up,low,conf.maxD);
            auto* a = new Agent(startPoints_abstraction->operator[](k),Section::adversary,10);
            auto* d = new Agent(std::move(listPosStart),Section::gurd,10);
            initRTDP(conf,d,k);
            setPathPolicy(conf,a,k);
            a->getPolicyInt()->add_tran(d->getPolicyInt());
            d->getPolicyInt()->add_tran(a->getPolicyInt());

            auto G = new Grid(up,low,this->endPoints_abstraction->operator[](k));
            simulationList.emplace_back(d,a,G,seedSystem);
            //break;
        }

        return simulationList;
    }
private:

    inline static vector<weightedPosition> startingPosDefender(const Point &up,const  Point &low,const int maxD)
    {
        vector<weightedPosition> l3;
        vector<Point> l;
        for(int x=up[0]-1;x>=(up[0]-(maxD));--x)
            for(int y=low[1];y<up[1];++y)
                for(int z=0;z<up[2]/3;++z)
                    l.emplace_back(x,y,z);

        vector<Point> l2;
        for(short x=0;x<=maxD;++x)
        {
            for(int y=0;y<=maxD;++y)
                for(int z=0;z<=maxD;++z)
                    l2.emplace_back(x,y,z);
        }
        float sizeL = l2.size()*l.size();
        for(auto &speed:l2)
            for(auto &ipos:l)
                l3.emplace_back(speed,ipos,float(1)/sizeL);
        return l3;
    }
    pair<Point,Point> getBound(int gridIndx,Point &abstract){
        int upIndx=gridIndx/abstract[0];
        int lowIndx = gridIndx%abstract[0];
        Point up(abstract[0]+upIndx*abstract[0],abstract[1]+lowIndx*abstract[1],abstract[2]*abstract[2]);
        Point low(0+upIndx*abstract[0],0+lowIndx*abstract[1],0);
        return {up,low};
    }
    void initRTDP(configGame &conf,Agent* d,u_int32_t k)
    {

        auto listQtalbe = vector<pair<int,int>>();
        listQtalbe.emplace_back(conf.maxD,1);
        shared_ptr<unordered_map<string,string>> gameInfo_share = std::make_shared<unordered_map<string,string>>();
        auto [it, result] = gameInfo_share->emplace("ID",conf.idNumber);
        listQtalbe.emplace_back(0,vecPolicy[k]->size());
        Policy* rtdp = new RtdpAlgo(conf.maxD,this->sizeVectors,listQtalbe,d->get_id(),conf.home,gameInfo_share);
        d->setPolicy(rtdp);
    }
    void setPathPolicy(configGame &conf, Agent* a,u_int32_t k)
    {
        Policy* aP = new PathPolicy("Path", conf.maxA, a->get_id(), conf.home,this->vecPolicy[k]);
        auto *tmp_pointer = dynamic_cast <PathPolicy*>(aP);
        cout<<tmp_pointer->name<<endl;
        a->setPolicy(aP);
    }
    void setDictHashState(hashIdStates* mDictHash){ dictHash=mDictHash;}
    void setEndStartPoint(vector<weightedPosition>& start, listPointWeighted* end,int maxSpeed)
    {
        for(auto &itemI : start) {
            auto s = Point(itemI.positionPoint);
            auto idx = emplaceInDictVec(s);
            startPoints_abstraction->operator[](idx).emplace_back(Point(0,0,maxSpeed),std::move(s),1);
        }
        for(auto &[a,b] : *end) {
            auto e = Point(b);
            auto idx = emplaceInDictVec(e);
            endPoints_abstraction->operator[](idx).emplace_back(Point(0),std::move(e),1);
        }

    }
    static void insetToList(std::vector<weightedPosition> &l , AStar::StatePoint &s){
        auto to_inset = weightedPosition(s.speed,s.pos,1);
        if (std::find(l.begin(), l.end(),to_inset) == l.end())
            l.push_back(std::move(to_inset));

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
    void normalizeStartVectorPoint()
    {
        std::for_each(this->startPoints_abstraction->begin(),this->startPoints_abstraction->end(),
                [&](std::vector<weightedPosition> &l)
                {
                    auto size = float (l.size());
                    std::for_each(l.begin(),l.end(),[&](weightedPosition &p){
                        p.weightedVal=1/size;
                    });
                });
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
                    // inset entry point into big-Grid
                    //this->vecPolicy[vecPolicy.size()-1]->emplace(pos->first,pos->second);

                }
                statS = StatePoint(statSOld);

                normalizeStartVectorPoint();
            }
        }

    }


};





#endif //TRACK_RACING_ABSTRACTIONDIV_H


