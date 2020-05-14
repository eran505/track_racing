//
// Created by ERANHER on 14.5.2020.
//

#ifndef TRACK_RACING_FACTORYAGENT_HPP
#define TRACK_RACING_FACTORYAGENT_HPP


#include "util_game.hpp"
#include "Policy.hpp"
#include "Agent.hpp"
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

    AbstractCreator(PathPolicy* evaderPolicy_,const Point& ptrGirdSize,const Point& mAbstractSize):evaderPolicy(evaderPolicy_){
        originalGridSize=ptrGirdSize;
        abGridSize=mAbstractSize;
    }

    void initializeSimulation()
    {
        
    }



};
#endif //TRACK_RACING_FACTORYAGENT_HPP
