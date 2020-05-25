//
// Created by ERANHER on 25.5.2020.
//
#define GOT_HERE std::cout << "At " __FILE__ ":" << __LINE__ << std::endl

#ifndef TRACK_RACING_REALTIMESIMULATION_HPP
#define TRACK_RACING_REALTIMESIMULATION_HPP
#include "headers/Abstract/Simulation.hpp"

class rtSimulation{

    /*
     *
     * Agent A with fully policy + real starting place
     * Grid with the real goals and real dimensions
     * Vector<RTDP> order by the ID mini-grids
     * Component that take a real-state -> abstract state with the ID min-grid
     * Component recognize when two agents are in the same mini grid
     * Score Tracking e.g. coll,wall ....
     * output the paths of the two agents
     *
     *
     *
     * */

    u_int32_t GetActionAbstract(State *s){
        
    }
    void stateTransform(State *s)
    {

    }
};

#endif //TRACK_RACING_REALTIMESIMULATION_HPP
