//
// Created by ERANHER on 14.5.2020.
//

#ifndef TRACK_RACING_FACTORYAGENT_HPP
#define TRACK_RACING_FACTORYAGENT_HPP
//#define Sync

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

    vector<simulation<State>> lsim;
    PathPolicy* evaderPolicy;
    Point originalGridSize;
    Point abGridSize;
    std::vector<simulation<State>> simulationVector;
    int seed;
    u_int32_t iter = 1000000;
    std::unique_ptr<rtSimulation> rtSim= nullptr;
    vector<shared_ptr<Agent>> lAgent;
public:
    vector<shared_ptr<Agent>> getLAgents(){return lAgent;}
    AbstractCreator(PathPolicy* evaderPolicy_,const Point& ptrGirdSize,const Point& mAbstractSize,int seed_):evaderPolicy(evaderPolicy_),seed(seed_){
        originalGridSize=ptrGirdSize;
        abGridSize=mAbstractSize;
    }

    auto initializeSimulation(configGame &conf,std::vector<weightedPosition> defenderStart)
    {
        auto abstractionObject = abstractionDiv(originalGridSize,abGridSize,evaderPolicy,seed);
        auto workerTasks = abstractionObject.initializeSimulation(conf,defenderStart);
        lsim = std::move(workerTasks);
        vector<int> l;
        //workerTasks.pop_back();
        std::vector<std::thread> workers;
        workers.reserve(lsim.size());
        //workerTasks.back().simulate(iter);
        //exit(0);
        #ifdef Sync
        std::for_each(lsim.begin(),lsim.end(),[&](simulation<State> &t)
        {
            t.simulate(iter);
        });
        #else
        for(auto &item:workerTasks)
        {
            std::function<void()> func = [&]() {
                item.simulate(iter);
                std::cout << "From Thread ID : "<<std::this_thread::get_id() << "\n";
            };
            workers.emplace_back(func);
        }
        std::for_each(workers.begin(),workers.end(),[](std::thread &t){
           if(t.joinable()) t.join();
        });
        #endif
        cout<<"done!"<<endl;
        lAgent.reserve(lsim.size());
        for (auto &item : lsim)
        {lAgent.emplace_back(item.getDefAgent());}
    }
};
#endif //TRACK_RACING_FACTORYAGENT_HPP
