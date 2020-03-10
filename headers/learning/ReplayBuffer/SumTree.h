//
// Created by ERANHER on 10.3.2020.
//

#ifndef TRACK_RACING_SUMTREE_H
#define TRACK_RACING_SUMTREE_H

#import "util_game.hpp"
/*
reference from:
- https://jaromiru.com/2016/11/07/lets-make-a-dqn-double-learning-and-prioritized-experience-replay/
- https://github.com/jaromiru/AI-blog/blob/348628b105058d876001ca758b6ba59fb1726614/SumTree.py#L3
*/
class SumTree{
    unsigned int capacity;
    vector<float>* dataTree = nullptr;
    vector<float>* tree = nullptr;

    void propagate(int idx, float change){
        auto parent = (idx -1) / 2 ;
        this->tree->operator[](parent)+=change;
        if (parent != 0)
            this->propagate(parent,change);
    }


public:
    explicit SumTree(unsigned int _capacity):capacity(_capacity)
    {
        this->dataTree = new vector<float>(capacity);
        this->tree = new vector<float>(2*capacity-1);
    }

    float total(){ this->tree[0];}

    void update(int idx,)

};

#endif //TRACK_RACING_SUMTREE_H
