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
#include <utility>
#include "Abstract/RealTimeSimulation.hpp"
#include "containerAbstract.h"
class AbstractCreator{
    vector<Point> allAbst;
    Point originalGridSize;
    vector<simulation> lsim;
    const PathPolicy* evaderPolicy;
    Point abGridSize;
    vector<containerAbstract> l_containers;
    Point divPoint;
    std::vector<simulation> simulationVector;

    int seed;
    u_int32_t iter = 1000000;
    std::unique_ptr<rtSimulation> rtSim= nullptr;
public:
    vector<containerAbstract>& get_con(){return l_containers;}
    std::unordered_map<u_int32_t,Agent*> mapAgent;

    AbstractCreator(const PathPolicy* evaderPolicy, const Point& ptrGirdSize, vector<Point> lPointAb, int seed_):
        allAbst(std::move(lPointAb)),originalGridSize(ptrGirdSize),evaderPolicy(evaderPolicy), seed(seed_){

    }
    void factory_containerAbstract(configGame &conf,const std::vector<weightedPosition>& defenderStart)
    {
        std::for_each(allAbst.begin(),allAbst.end(),[&](const Point& p){
            this->abGridSize=p;
            this->divPoint=this->originalGridSize/p;
            this->initializeSimulation(conf,defenderStart);
        });
        cout<<"";
    }
    void helperGridSim(configGame &conf)
    {
        auto abstractionObject_Helper = abstractionDiv(originalGridSize,abGridSize,evaderPolicy,seed,Point(0),Point(4,0,0));
        vector<simulation> offset_grids;
        abstractionObject_Helper.miniGrid_initializeSimulation(conf,offset_grids);
        std::for_each(offset_grids.begin(),offset_grids.end(),[&](simulation &item){item.simulate(iter);});

    }
    static void reduceMemo(simulation &t)
    {
        t.agents[event::agnetIDX::defenderInt].get()->getPolicyInt()->minimization();
    }
    void initializeSimulation(configGame &conf,const std::vector<weightedPosition> &defenderStart)
    {
        auto abstractionObject = abstractionDiv(originalGridSize,abGridSize,evaderPolicy,seed,Point(0),Point(0,0,0));
        auto workerTasks = abstractionObject.initializeSimulation(conf,defenderStart);


        lsim = std::move(workerTasks);
        vector<int> l;
        //workerTasks.pop_back();
        std::vector<std::thread> workers;
        //workers.reserve(workerTasks.size());
        lsim.back().simulate(iter*0.8);
        //RemoveIfVector(workerTasks);
        workers.reserve(lsim.size());
        std::for_each(lsim.back().getCollustionMap().begin(),lsim.back().getCollustionMap().end(),
                [&](auto &item){cout<<item.first<<";"<<item.second<<endl;});

        //lsim.back().getDefAgentDATA();


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
                //reduceMemo(t);
            }
            else{
                //delete t.getDefAgentPTR()->getPolicyInt();
            }
        });
        // Need to reset the Agent
        lsim.back().agents[event::agnetIDX::defenderInt].get()->getPolicyInt()->learnRest();
        lsim.back().simulate(iter*2); // learn again on the modified rewards
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

        l_containers.emplace_back(Point(0),Point(this->abGridSize),Point(this->originalGridSize),lsim.back().gridID);
        auto& obj = l_containers.back();
        for (auto &item : lsim)
        {
            //auto ptrTmp = item.getDefAgentPTR();
            cout<<"Is copying,,,"<<item.gridID<<endl;
            obj.insetToDict(item.gridID,item.agents[0]);
            //mapAgent.try_emplace(item.gridID,item.agents[0].get());
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
