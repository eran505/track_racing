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
class AbstractCreator{

    PathPolicy* evaderPolicy;
    Point originalGridSize;
    Point abGridSize;
    std::vector<simulation> simulationVector;
    int seed;
    u_int32_t iter = 1000000;
public:
    AbstractCreator(PathPolicy* evaderPolicy_,const Point& ptrGirdSize,const Point& mAbstractSize,int seed_):evaderPolicy(evaderPolicy_),seed(seed_){
        originalGridSize=ptrGirdSize;
        abGridSize=mAbstractSize;
    }

    void initializeSimulation(configGame &conf)
    {
        auto abstractionObject = abstractionDiv(originalGridSize,abGridSize,evaderPolicy,seed);
        auto workerTasks = abstractionObject.initializeSimulation(conf);
        vector<int> l;
        std::vector<std::thread> workers;
        workers.reserve(workerTasks.size());
        #ifdef Sync
        std::for_each(workerTasks.begin(),workerTasks.end(),[&](simulation &t)
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
    }



};
#endif //TRACK_RACING_FACTORYAGENT_HPP
