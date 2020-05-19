//
// Created by ERANHER on 14.5.2020.
//

#ifndef TRACK_RACING_FACTORYAGENT_HPP
#define TRACK_RACING_FACTORYAGENT_HPP


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

class AbstractCreator{

    PathPolicy* evaderPolicy;
    Point originalGridSize;
    Point abGridSize;
    std::vector<simulation> simulationVector;
    int seed;
public:
    AbstractCreator(PathPolicy* evaderPolicy_,const Point& ptrGirdSize,const Point& mAbstractSize,int seed_):evaderPolicy(evaderPolicy_),seed(seed_){
        originalGridSize=ptrGirdSize;
        abGridSize=mAbstractSize;
    }

    void initializeSimulation(configGame &conf)
    {
        auto abstractionObject = abstractionDiv(originalGridSize,abGridSize,evaderPolicy,seed);
        auto workerTasks = abstractionObject.initializeSimulation(conf);
        for(auto &item:workerTasks)
        {
            item.simulate(5000000);
        }
    }



};
#endif //TRACK_RACING_FACTORYAGENT_HPP
