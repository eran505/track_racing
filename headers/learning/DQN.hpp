//
// Created by ise on 10.2.2020.
//
#include "neuralNet.h"
#include "ReplayBuffer/ReplayBuffer.hpp"
#ifndef TRACK_RACING_DQN_HPP
#define TRACK_RACING_DQN_HPP

class DQN{
    ReplayBuffer *memoryBuffer;
    neuralNet *localNet;



public:
    DQN();
    void update();
    ~DQN();
    Point pickAction();
    void learn();

};


#endif //TRACK_RACING_DQN_HPP
