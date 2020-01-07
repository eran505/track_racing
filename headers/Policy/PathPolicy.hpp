//
// Created by ise on 30.12.2019.
//

#ifndef TRACK_RACING_PATHPOLICY_HPP
#define TRACK_RACING_PATHPOLICY_HPP

#include "../serach/Astar.hpp"
#include "Policy.hpp"

#include <utility>

typedef vector<tuple<Point*,double>> listPointWeighted;

class PathPolicy:public Policy{

    listPointWeighted goalPoint;
    listPointWeighted startPoint;
    unordered_map<int,vector<float>*> *dictPolicy;
    int getAgentSateHash(State *s);
public:
    int getNumberOfState() {
        return dictPolicy->size();
    }
    PathPolicy(string namePolicy, int maxSpeedAgent,listPointWeighted endPoint_, listPointWeighted startPoint_,
               Point &gridSzie_) : Policy(std::move(namePolicy), maxSpeedAgent) {
        this->goalPoint=std::move(endPoint_);
        this->dictPolicy= nullptr;
        this->startPoint=std::move(startPoint_);
        this->initPolicy(gridSzie_);
        printf("\ndone!\n");
    }
    void initPolicy(Point &girdSize){
        cout<<"A star..."<<endl;
        auto *xx = new AStar::Generator(this->max_speed,girdSize);
        for (unsigned long i = 0; i < this->startPoint.size(); ++i) {
            for (unsigned long k = 0; k < this->goalPoint.size(); ++k) {
                for (unsigned int s=1 ; s<=this->max_speed;++s)
                {
                    auto startP = std::get<0>(startPoint[i]);
                    auto endP = std::get<0>(goalPoint[i]);
                    auto weightEnd = std::get<1>(goalPoint[i]);
                    auto zeroSrc  = Point();
                    auto zeroDest  = Point();
                    auto src = AStar::StatePoint{Point(*startP),zeroSrc};
                    auto dest = AStar::StatePoint{Point(*endP),zeroDest};
                    xx->changeMaxSpeed(s);
                    auto res = xx->findPath(src,dest);
                    dictPolicy =xx->getDict(weightEnd);

                }
            }
        }


        delete(xx);
    }
    Point get_action(State *s) override;
    ~PathPolicy() override{
        for(auto &item : *this->dictPolicy)
            delete(item.second);
        delete(dictPolicy);
    };
    void reset_policy() override{};
    void policy_data() const override{};
    const std::vector<float>* TransitionAction(State*) override;

};

Point PathPolicy::get_action(State *s) {
    auto EntryIndx = getAgentSateHash(s);
    std::unordered_map<int, std::vector<float>*>::iterator it;
    it = this->dictPolicy->find(EntryIndx);
    if (it==this->dictPolicy->end())
        throw;
    // get value between zero to one
    float r = static_cast <float> (random()) / static_cast <float> (RAND_MAX);
    double acc=0;
    unsigned long i;
    for (i=1; i <it->second->size() ; ++i) {
        acc+=it->second->operator[](i);
        if (r<=acc)
            break;
        i+=1;
    }
    int indexI =int(it->second->operator[](int(i-1)));
    Point choosenAction = *this->hashActionMap->at(indexI);
    return choosenAction;

}

const std::vector<float> *PathPolicy::TransitionAction(State *s) {

    int EntryIdx = getAgentSateHash(s);
    auto pos = this->dictPolicy->find(EntryIdx);
    if (pos==this->dictPolicy->end())
        throw std::runtime_error(std::string("Error: cant find the key "));;
    return this->dictPolicy->at(EntryIdx);

}

int PathPolicy::getAgentSateHash(State *s) {
    int hSpeed = s->get_speed(this->id_agent).hashConst(Point::maxSpeed);
    int hPos = s->get_position(this->id_agent).hashConst();
    int EntryIndx = Point::hashNnN(hPos,hSpeed);
    return EntryIndx;
}

//            int sumAll = accumulate(itemFirst.second->begin(), itemFirst.second->end(), 0,
//                    [](int v, map<int,int>::value_type& pair){
//                return v + pair.second;
//                // if we want to sum the first item, change it to
//                // return v + pair.first;
//            });

#endif //TRACK_RACING_PATHPOLICY_HPP
