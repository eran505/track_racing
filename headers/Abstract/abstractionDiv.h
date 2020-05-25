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
#include <variant>
#include "Abstract/Simulation.hpp"
#include "Policy/RtdpAlgo.hpp"
#include "Grid.hpp"
#include "util/utilClass.hpp"
#define DEBUG

typedef unordered_map<u_int64_t,vector<float>*> dictPolicyPath;
typedef std::vector<std::vector<weightedPosition>> doubleVecPoint;
typedef std::unordered_map<u_int64_t ,pair<short,AStar::StatePoint>> hashIdStates;
typedef vector<tuple<Point*,double>> listPointWeightedd;
typedef std::vector<pair<float,vector<pair<Point,Point>>>> weightedVectorPosSpeed ;
using AStar::StatePoint;

class abstractionDiv{
    int seedSystem;
    vector<dictPolicyPath*> vecPolicy;
    dictPolicyPath* allDictPolicy;
    hashIdStates* dictHash;
    std::unique_ptr<doubleVecPoint> startPoints_abstraction;
    std::unique_ptr<doubleVecPoint> endPoints_abstraction; // probabily for the speed in each cell in the bigGrid
    unordered_map<u_int64_t,float>* speedPorbabilityVector;
    Point girdSize;
    Point abstractSize;
    std::vector<std::pair<float,std::vector<Point>>> myPaths;
    unordered_map<int,Point*>* hashActionDict;
    int sizeVectors;

public:
    ~abstractionDiv()
    {
        for(auto item : *hashActionDict)
            delete item.second;
        delete hashActionDict;
        delete dictHash;
    }
    abstractionDiv(const Point& ptrGirdSize,
            const Point& mAbstractSize,PathPolicy *policyP,int seed){
        auto tmp = ptrGirdSize/mAbstractSize;
        auto sizeMiniGrid = tmp.accMulti();
        sizeVectors=sizeMiniGrid;
        abstractSize=mAbstractSize;
        girdSize = ptrGirdSize;
        seedSystem = seed;
        allDictPolicy=policyP->getDictPolicy();
        //auto number2D = girdSize.array[0]*girdSize.array[1];
        vecPolicy = vector<dictPolicyPath*>(sizeMiniGrid+1);
        startPoints_abstraction = std::make_unique<doubleVecPoint>(sizeMiniGrid+1);
        endPoints_abstraction = std::make_unique<doubleVecPoint>(sizeMiniGrid+1);
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
        //setEndStartPoint(policyP->startPoint,policyP->goalPoint,policyP->max_speed);
        entryPoint(policyP->max_speed);
        std::cout<<endl;

    }

    /**
     * TODO: need to fix this [initAbstract] by-pass
     * */
    void entryPoint(int maxSpeed)
    {
        weightedVectorPosSpeed lp;
        for(auto & myPathPair : myPaths)
        {
            auto &lpI = lp.emplace_back();
            auto &[lpIp,lpIVector]=lpI;
            auto &[p,listPos]=myPathPair; // <p,vec>
            lpIp=p;
            lpIVector.emplace_back(listPos.front(),Point(0,0,maxSpeed));
            for(size_t i = 1;i<listPos.size();++i)
            {
                auto speedI = listPos[i]-listPos[i-1];
                lpIVector.emplace_back(listPos[i],speedI);
            }
        }
        bigGridAbs(lp);
    }
    /**
     * lp = the paths an entrt looks like the following: -> <probability(float),vector(pos,speed)>
     * **/
    void bigGridAbs(weightedVectorPosSpeed& lp)
    {
        vector<pair<float,vector<weightedPosition>>> setPaths;
        vector<Point> goals;
        short index=-1;
        for(auto & myPathPair : myPaths)
        {
            index++;
            auto &[p,myPath]=myPathPair;
            auto &pairItem= setPaths.emplace_back();
            auto &l = pairItem.second;
            pairItem.first=p;
            for(u_int32_t j=0;j<myPath.size();++j)
            {
                myPath[j]/=this->abstractSize;
                if (j>0)
                {
                    auto tmpI = lp[index].second[j];
                    if (!(myPath[j]==myPath[j-1]))
                    {
                        l.emplace_back(myPath[j]-myPath[j-1],myPath[j],1);
                        inset_boundry_point(lp[index].second[j],lp[index].first,myPath[j], true);
                        inset_boundry_point(lp[index].second[j-1],lp[index].first,myPath[j-1],false);
                    }
                    else
                        l.back().weightedVal+=1;
                }
                else{
                    inset_boundry_point(lp[index].second[j],lp[index].first,myPath[j], true);
                    l.emplace_back(Point(),myPath[j],1);
                    inset_boundry_point_bigGrid(lp[index].first,myPath[j], true);//insert to big grid
                }
            }
            inset_boundry_point_bigGrid(lp.back().first,myPath.back());//insert to big grid
            inset_boundry_point(lp.back().second.back(),lp.back().first,myPath.back());
        }
        normProbability(this->startPoints_abstraction.get());
        normProbability(this->endPoints_abstraction.get());
        make_dict_policy(setPaths);
        cout<<endl;
    }
    void inset_boundry_point_bigGrid(float val,Point &gridPoint_,bool entry=false)
    {
        doubleVecPoint *vec_to_inset= nullptr;
        if(entry)
            vec_to_inset = startPoints_abstraction.get();
        else
            vec_to_inset = endPoints_abstraction.get();
        auto pos = std::find_if(vec_to_inset->back().begin(),vec_to_inset->back().end(),[&](weightedPosition &p){
            if(p.positionPoint==gridPoint_) return true;
            return false;
        });
        if (pos==vec_to_inset->back().end())
            vec_to_inset->back().emplace_back(Point(0),gridPoint_,val);
        else
            pos->weightedVal+=val;

    }
    void inset_boundry_point(pair<Point,Point> &statePoint,float val,Point &gridPoint_,bool entry=false)
    {
        doubleVecPoint* vec_to_inset= nullptr;
        if(entry)
            vec_to_inset = startPoints_abstraction.get();
        else
            vec_to_inset = endPoints_abstraction.get();

        auto& vec = vec_to_inset->operator[](gridPoint_[0]*this->abstractSize[0]+gridPoint_[1]);
        if (auto pos=std::find_if(vec.begin(),vec.end(),[&](weightedPosition &pState){
            if(pState.speedPoint==statePoint.second && pState.positionPoint==statePoint.first)return true;
            return false; });pos == vec.end())
        {
            vec.emplace_back(statePoint.second,statePoint.first,val);
        }else{
            pos->weightedVal+=val;
        }
    }
    /**
     * make the big policy attacker grid
     * speedProbabilityVector = how much time the agent need to spent in the cell,
     * the probability of moving to the next cell
     * */
    void make_dict_policy(vector<pair<float,vector<weightedPosition>>>& momvemtns)
    {
        vector<pair<u_int64_t,pair<vector<pair<short,float>>,float>>> moveSpeedAbstract;
        const auto index= vecPolicy.size()-1;
        auto zero = Point(0);
        auto zeroHashAction = float(zero.hashMeAction(Point::D_point::actionMax));
        for (auto& itemPair : momvemtns){
            auto& [p,item]=itemPair;
            for(int i=0;i<item.size()-1;++i)
            {
                auto difAction =  item[i+1].speedPoint-item[i].speedPoint;
                auto diffPos = item[i+1].positionPoint-item[i].positionPoint;
                auto diffPosHASH = float(diffPos.hashMeAction(Point::D_point::actionMax));
                auto actionHkey = float(difAction.hashMeAction(Point::D_point::actionMax));
                auto op_difAction = item[i].speedPoint*-1;
                auto key_op_difAction = float(op_difAction.hashMeAction(Point::D_point::actionMax));

                u_int64_t key = Point::hashNnN(item[i].positionPoint.hashConst(),
                                               item[i].speedPoint.hashConst(Point::maxSpeed));

                // for the case when the agent has zero speed in the state
                u_int64_t keyZero = Point::hashNnN(item[i].positionPoint.hashConst(),
                                                   zero.hashConst(Point::maxSpeed));

                if (auto pos = this->vecPolicy[index]->find(key);pos==this->vecPolicy[index]->end())
                {
                    this->vecPolicy[index]->try_emplace
                    (key, new std::vector<float>({actionHkey,-p,key_op_difAction,0}));

                    this->vecPolicy[index]->try_emplace
                    (keyZero, new std::vector<float>({diffPosHASH,-p,zeroHashAction,0}));

                }
                else{
                    auto isFind = std::find(pos->second->begin(),pos->second->end(),actionHkey);
                    if(isFind==pos->second->end())
                        pos->second->insert(pos->second->end(),{actionHkey,-p});
                    else
                        {
                            auto b = std::distance(pos->second->begin(),isFind)+1;
                            pos->second->operator[](b)-=p;
                        }
                    auto posZero = this->vecPolicy[index]->find(keyZero);
                    auto isFindZero = std::find(posZero->second->begin(),posZero->second->end(),diffPosHASH);
                    if (isFindZero==posZero->second->end())
                    {
                        pos->second->insert(pos->second->end(),{diffPosHASH,-p});
                    } else
                        {
                            auto b = std::distance(posZero->second->begin(),isFindZero)+1;
                            posZero->second->operator[](b)-=p;
                        }

                }

                if(auto pos = std::find_if(moveSpeedAbstract.begin(),moveSpeedAbstract.end(),[&](auto& entryI){
                    if(entryI.first==key) return true;
                    return false;
                });pos==moveSpeedAbstract.end())
                {
                    auto& curEntry = moveSpeedAbstract.emplace_back();
                    curEntry.first=key;
                    curEntry.second.second=p;
                    curEntry.second.first.emplace_back(item[i].weightedVal,p);

                } else{
                    pos->second.second+=p;
                    pos->second.first.emplace_back(item[i].weightedVal,p);
                }
            }
        }
        for(auto& item: *this->vecPolicy[index])
        {
            float acc=0;
            int indexCur=0;
            std::for_each(item.second->begin(),item.second->end(),[&](float x){
                if(indexCur%2!=0)
                {
                    acc+=x;
                }
                indexCur++;});
            indexCur=0;
            for(auto &num:*item.second)
            {
                if (indexCur%2!=0){

                    if(num==0)
                        num=1.0+acc;
                    else{num*=-1;}
                }
                indexCur++;
            }
        }
        getSpeedDictAbs(moveSpeedAbstract);
        cout<<"end"<<endl;
    }
    std::vector<simulation<State>> initializeSimulation(configGame &conf,std::vector<weightedPosition>& StartingDefender){
        //vector<weightedPosition> l = {{Point(0),Point(0),1}};
        std::vector<simulation<State>> simulationList;
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
        std::for_each(StartingDefender.begin(),StartingDefender.end(),[&](weightedPosition &data){
            data.positionPoint/=abstractSize;
            data.speedPoint/=abstractSize;
        });
        conf.maxD=1;
        conf.maxA=1;
        auto k=vecPolicy.size()-1;
        auto [up,low]=getBound(0,this->abstractSize);
        auto* a = new Agent(startPoints_abstraction->back(),Section::adversary,10);
        auto* d = new Agent(StartingDefender,Section::gurd,10);
        initRTDP(conf,d,k,float(conf.maxD)/float(this->abstractSize[0]));
        setPathPolicy(conf,a,k);
        a->getPolicyInt()->add_tran(d->getPolicyInt());
        d->getPolicyInt()->add_tran(a->getPolicyInt());
        auto G = new Grid(up,low,this->endPoints_abstraction->operator[](k));
        simulationList.emplace_back(d,a,G,seedSystem);

        return simulationList;
    }
private:
    inline void initBigGrid(configGame &conf, std::vector<simulation<State>> &vecSim,std::vector<weightedPosition>& StartingDefender)
    {

    }
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
        int z = gridIndx/abstract[2];
        Point up(abstract[0]+upIndx*abstract[0],abstract[1]+lowIndx*abstract[1],abstract[2]);
        Point low(0+upIndx*abstract[0],0+lowIndx*abstract[1],z);
        return {up,low};
    }
    void initRTDP(configGame &conf,Agent* d,u_int32_t k,float stoProb=1)
    {

        auto listQtalbe = vector<pair<int,int>>();
        listQtalbe.emplace_back(conf.maxD,1);
        shared_ptr<unordered_map<string,string>> gameInfo_share = std::make_shared<unordered_map<string,string>>();
        auto [it, result] = gameInfo_share->emplace("ID",conf.idNumber);
        listQtalbe.emplace_back(0,vecPolicy[k]->size());
        Policy* rtdp = new RtdpAlgo(conf.maxD,this->sizeVectors,listQtalbe,d->get_id(),conf.home,gameInfo_share);
        auto tmp = dynamic_cast<RtdpAlgo*>(rtdp);
        tmp->setStochasticMovement(stoProb);
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
        auto z = statePos.array[2]/abstractSize.array[2];

        return (abstractSize.array[0])*row+col+z*(abstractSize.array[2]);
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
                    //insetToList(startPoints_abstraction->operator[](newIdx),statS);
                    //insetToList(endPoints_abstraction->operator[](idx),tmp);
                    // inset entry point into big-Grid
                    //this->vecPolicy[vecPolicy.size()-1]->emplace(pos->first,pos->second);

                }
                statS = StatePoint(statSOld);
                normalizeStartVectorPoint();
            }
        }

    }
    static inline void normProbability(doubleVecPoint* ptrVec)
    {
        std::for_each(ptrVec->begin(),ptrVec->end(),[](vector<weightedPosition> &l){
            float acc=0;
            std::for_each(l.begin(),l.end(),[&](weightedPosition &p){
                acc+=p.weightedVal;
            });
            std::for_each(l.begin(),l.end(),[&](weightedPosition &p){
                p.weightedVal=p.weightedVal/acc;
            });
        });
    }
    inline void getSpeedDictAbs(vector<pair<u_int64_t,pair<vector<pair<short,float>>,float>>>& moveSpeedAbstract)
    {
        this->speedPorbabilityVector = new unordered_map<u_int64_t ,float>();
        for(auto& item: moveSpeedAbstract){
            auto key = item.first;
            float val = 0;
            std::for_each(item.second.first.begin(),item.second.first.end(),[&](pair<short,float>& x)
            {
                val+=(x.first*(x.second/item.second.second));
            });
            this->speedPorbabilityVector->emplace(key,val);
        }
    }
    void combainPolicyandMoveDict(int k)
    {
        auto* posLast = this->vecPolicy.back();
        for(auto itemPair : *posLast)
        {
            auto pos = speedPorbabilityVector->find(itemPair.first);
            if (pos==speedPorbabilityVector->end())
                throw;
            auto p = pos->second;
            auto posSpeed = this->dictHash->find(itemPair.first)->second.second;

        }
    }
};





#endif //TRACK_RACING_ABSTRACTIONDIV_H


