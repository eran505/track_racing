//
// Created by ise on 30.12.2019.
//

#ifndef TRACK_RACING_PATHPOLICY_HPP
#define TRACK_RACING_PATHPOLICY_HPP

#include "../serach/Astar.hpp"
#include "Policy.hpp"
#include <assert.h>
#include <utility>

typedef vector<tuple<Point*,double>> listPointWeighted;

class PathPolicy:public Policy{
    unsigned long maxPathsNumber;
    listPointWeighted goalPoint;
    listPointWeighted startPoint;
    vector<Point> midVec;
    Point actionmy;
    unordered_map<int,vector<float>*> *dictPolicy;
    int getAgentSateHash(State *s);
public:
    int getNumberOfState() {
        return dictPolicy->size();
    }
    PathPolicy(string namePolicy, int maxSpeedAgent,listPointWeighted endPoint_, listPointWeighted startPoint_,
               Point &gridSzie, string agentID,vector<Point> midVecPoints,string &home,unsigned long maxPathz=ULONG_MAX) : Policy(std::move(namePolicy),
                       maxSpeedAgent,std::move(agentID),home),midVec(move(midVecPoints)) {
        this->goalPoint=std::move(endPoint_);
        this->dictPolicy= nullptr;

        this->maxPathsNumber = maxPathz;
        this->startPoint=std::move(startPoint_);
        this->initPolicy(gridSzie);
        printf("\ndone!\n");
    }
    void initPolicy(Point &girdSize){
        dictPolicy = new unordered_map<int, vector<float>*>();
        double weightEnd;
        cout<<"A star..."<<endl;
        auto *xx = new AStar::Generator(this->max_speed,girdSize);
        xx->setMaxPATH(this->maxPathsNumber);
        for (unsigned long i = 0; i < this->startPoint.size(); ++i) {
            for (unsigned long k = 0; k < this->goalPoint.size(); ++k) {
                for (unsigned int s=max_speed ; s<=this->max_speed;++s)
                {
                    auto startP = std::get<0>(startPoint[i]);
                    auto endP = std::get<0>(goalPoint[k]);
                    weightEnd = std::get<1>(goalPoint[k]);
                    auto zeroSrc  = Point();
                    auto startSpeed  = Point(0,0,max_speed);
                    //auto zeroDest  = Point();
                    auto src = AStar::StatePoint{Point(*startP),startSpeed};
                    auto dest = AStar::StatePoint{Point(*endP),startSpeed};
                    xx->changeMaxSpeed(s);
                    if (midVec.size()>k)
                        xx->findComplexPath(src,midVec[k],dest);
                    else
                        auto res = xx->findPath(src,dest);
                    
                }
                xx->getDict(dictPolicy,weightEnd);
                xx->dictPolyClean();
            }
        }

        normalizeDict();
        delete(xx);
    }
    Point get_action(State *s) override;
    ~PathPolicy() override{
        int ctr=0;
        cout<<"del A star"<<endl;
        for(auto &item : *this->dictPolicy)
        {
            //cout<<item.first<<"\tctr:\t"<<ctr<<endl;
            delete(item.second);
            ctr++;
        }
        delete(dictPolicy);
    };
    void reset_policy() override{};
    void policy_data() const override{};
    const std::vector<float>* TransitionAction(State*) override;
    void normalizeDict();
};

Point PathPolicy::get_action(State *s) {
    auto EntryIndx = getAgentSateHash(s);
    //trajectory.push_back(s->to_string_state()+"="+std::to_string(EntryIndx));
    std::unordered_map<int, std::vector<float>*>::iterator it;
    it = this->dictPolicy->find(EntryIndx);
    if (it==this->dictPolicy->end())
        throw;
    // get value between zero to one
    float r = static_cast <float> (random()) / static_cast <float> (RAND_MAX);
    float acc=0;
    unsigned long i;
    for (i=1; i <it->second->size() ; ++i) {
        acc+=it->second->operator[](i);
        if (r<=acc)
            break;
        i+=1;
    }
    // problem with precision number (floating point)
    if (it->second->size()<i)
    {
        i=it->second->size()-1;
    }

    int indexI =int(it->second->operator[](int(i-1)));
    Point choosenAction = *this->hashActionMap->at(indexI);
    return choosenAction;

}

const std::vector<float> *PathPolicy::TransitionAction(State *s) {
    /* first  - action
     * second - probabilitiy*/
    //cout<<s->to_string_state()<<endl;
    int EntryIdx = getAgentSateHash(s);
    auto pos = this->dictPolicy->find(EntryIdx);
    if (pos==this->dictPolicy->end())
        throw std::runtime_error(std::string("Error: cant find the key "));

    return this->dictPolicy->at(EntryIdx);

}

int PathPolicy::getAgentSateHash(State *s) {
    int hSpeed = s->get_speed(this->id_agent).hashConst(Point::maxSpeed);
    int hPos = s->get_position(this->id_agent).hashConst();
    int EntryIndx = Point::hashNnN(hPos,hSpeed);
    return EntryIndx;
}

void PathPolicy::normalizeDict(){
    for(auto &item : *this->dictPolicy)
    {
        const size_t vecSize = item.second->size();
        float sumProbability=0;
        int indx=0;
        for (auto sec_Item : *item.second) {
            if (indx%2==1)
                sumProbability+=sec_Item;
            indx++;
        }
        //assert(sumProbability<=1.0);

        if (sumProbability>1.0) {
            cout<<"";
        }


        if (sumProbability==1.0) {
            continue;
        }

        if (sumProbability==0) {
            continue;
        }


        for(indx=1; indx<vecSize;indx=indx+2)
        {
            item.second->operator[](indx)=item.second->operator[](indx)/sumProbability;
        }

    }
}


//            int sumAll = accumulate(itemFirst.second->begin(), itemFirst.second->end(), 0,
//                    [](int v, map<int,int>::value_type& pair){
//                return v + pair.second;
//                // if we want to sum the first item, change it to
//                // return v + pair.first;
//            });

#endif //TRACK_RACING_PATHPOLICY_HPP
