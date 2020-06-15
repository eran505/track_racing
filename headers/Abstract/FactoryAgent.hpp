//
// Created by ERANHER on 14.5.2020.
//

#ifndef TRACK_RACING_FACTORYAGENT_HPP
#define TRACK_RACING_FACTORYAGENT_HPP
#define Sync

#include "util_game.hpp"
#include "Policy.hpp"
#include "Agent.hpp"
#include "util/utilClass.hpp"
/**
 *
 *  Grid (size) -> goals
 *  Agents() -> staring point
 *  Policy()
 *
 *
 *
 * **/
#include "Abstract/abstractionDiv.h"
#include "Abstract/Simulation.hpp"
#include <thread>
#include "Abstract/RealTimeSimulation.hpp"
class AbstractCreator{

    vector<simulation> lsim;
    PathPolicy* evaderPolicy;
    Point originalGridSize;
    Point abGridSize;
    Point divPoint;
    std::vector<simulation> simulationVector;
    int seed;
    u_int32_t iter = 500000;
    std::unique_ptr<rtSimulation> rtSim= nullptr;
    unordered_map<u_int32_t ,Agent*> lAgent;
public:
    unordered_map<u_int32_t ,Agent*> getLAgents(){return lAgent;}
    AbstractCreator(PathPolicy* evaderPolicy, const Point& ptrGirdSize, const Point& mAbstractSize, int seed_):
            abGridSize(mAbstractSize), evaderPolicy(evaderPolicy), seed(seed_){
        originalGridSize=ptrGirdSize;
        abGridSize=mAbstractSize;
        divPoint = ptrGirdSize/mAbstractSize;
    }

    auto initializeSimulation(configGame &conf,std::vector<weightedPosition> defenderStart)
    {
        auto abstractionObject = abstractionDiv(originalGridSize,abGridSize,evaderPolicy,seed,Point(0),Point(0,0,0));
        auto workerTasks = abstractionObject.initializeSimulation(conf,defenderStart);

        auto abstractionObject_Helper = abstractionDiv(originalGridSize,abGridSize,evaderPolicy,seed,Point(0),Point(4,0,0));
        vector<simulation> offset_grids;
        abstractionObject_Helper.miniGrid_initializeSimulation(conf,offset_grids);


        lsim = std::move(workerTasks);
        vector<int> l;
        //workerTasks.pop_back();
        std::vector<std::thread> workers;
        //workers.reserve(workerTasks.size());
        lsim.back().simulate(iter/2);
        //RemoveIfVector(workerTasks);
        workers.reserve(lsim.size());
        std::for_each(lsim.back().getCollustionMap().begin(),lsim.back().getCollustionMap().end(),
                [&](auto &item){cout<<item.first<<";"<<item.second<<endl;});

        //lsim.back().getDefAgentDATA();
        //exit(0);

        #ifdef Sync

        std::for_each(lsim.begin(),lsim.end()-1,[&](simulation &t)
        {
            cout<<"gridID:\t"<<t.gridID<<endl;
            if(IsReachable(t.gridID) ){
                cout<<"t.gridID="<<t.gridID<<endl;
                t.simulate(iter);
                auto newCollReward = t.getAvgExpectedReward();
                insetBigAbstractGridReward(t.gridID,newCollReward);
                cout<<"getAvgExpectedReward:\t"<<newCollReward<<endl;
            }
            else{
                //delete t.getDefAgentPTR()->getPolicyInt();
            }
        });

        lsim.back().simulate(iter); // learn again on the modified rewards
        std::for_each(lsim.back().getCollustionMap().begin(),lsim.back().getCollustionMap().end(),
                      [&](auto &item){cout<<item.first<<";"<<item.second<<endl;});
        #else
        for(size_t index=0;index<lsim.size();++index)
        {
            if(!IsReachable(lsim[index].gridID))
                continue;
            std::function<void()> func = [&]() {
                lsim[index].simulate(iter);
                std::cout << "From Thread ID : "<<std::this_thread::get_id() << "\n";
            };
            workers.emplace_back(func);
        }
        std::for_each(workers.begin(),workers.end(),[](std::thread &t){
           if(t.joinable()) t.join();
        });
        #endif
        //lAgent.reserve(lsim.size());

        for (auto &item : lsim)
        {
            //auto ptrTmp = item.getDefAgentPTR();
            cout<<"Is copying,,,"<<item.gridID<<endl;
            lAgent.insert({item.gridID,item.agents[0].get()});
        }
        cout<<"done!"<<endl;
    }

    Point keyToPoint(unsigned int key)
    {

        u_int32_t x = (key%(divPoint[0]*divPoint[1]))/divPoint[0];
        u_int32_t  y = key%divPoint[0];
        u_int32_t  z = key/(divPoint[1]*divPoint[0]);

        return Point(x,y,z);
    }
    void RemoveIfVector(vector<simulation> &l)
    {
        auto iteVec = std::copy_if(l.begin(),l.end(),lsim.begin(),[&](simulation& itemX){
            if(l.back().isInCollMap(keyToPoint(itemX.gridID).to_str()) or itemX.gridID==l.back().gridID)
                return true;
            return false;
        });
        //lsim.erase(iteVec);
    }
    bool IsReachable(u_int32_t key){
        return lsim.back().isInCollMap(keyToPoint(key).to_str());
    }
    void insetBigAbstractGridReward(u_int32_t idGrid, double reward)
    {
        lsim.back().getRtdpAlgo()->insetRewardMap(keyToPoint(idGrid).expHash(),reward);
        lsim.back().getRtdpAlgo()->resetAlgo();
    }
};
#endif //TRACK_RACING_FACTORYAGENT_HPP
