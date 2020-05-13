//
// Created by ise on 30.12.2019.
//

#ifndef TRACK_RACING_PATHPOLICY_HPP
#define TRACK_RACING_PATHPOLICY_HPP

#include "../serach/Astar.hpp"
#include "Policy.hpp"
#include <cassert>
#include <utility>
#include "util/csvfile.hpp"

typedef shared_ptr<unordered_map<string ,string>> dictionary;

typedef vector<tuple<Point*,double>> listPointWeighted;

class PathPolicy:public Policy{
    unsigned long maxPathsNumber;


    vector<Point> midVec;
    vector<std::tuple<double,vector<Point>>> pathz;
    Point actionmy;
    unordered_map<u_int64_t,vector<float>*> *dictPolicy;
    u_int64_t getAgentSateHash(State *s);
public:
    unordered_map<u_int64_t ,pair<short,AStar::StatePoint>>* statesIdDict;
    unordered_map<u_int64_t,vector<float>*>* getDictPolicy(){return dictPolicy;}
    listPointWeighted goalPoint;
    listPointWeighted startPoint;
    int getNumberOfState() {
        return dictPolicy->size();
    }
    PathPolicy(string namePolicy, int maxSpeedAgent,listPointWeighted endPoint_, listPointWeighted startPoint_,
               Point &gridSzie, string agentID,vector<Point> midVecPoints,string &home,unsigned long maxPathz=ULONG_MAX,dictionary ptrDict=nullptr) : Policy(std::move(namePolicy),
                       maxSpeedAgent,std::move(agentID),home,ptrDict),midVec(move(midVecPoints)) {
        this->goalPoint=std::move(endPoint_);
        this->dictPolicy= nullptr;
        this->statesIdDict= nullptr;
        this->maxPathsNumber = maxPathz;
        this->startPoint=std::move(startPoint_);
        this->initPolicy(gridSzie);
        printf("\ndone!\n");
    }
    void initPolicy(Point &girdSize){
        dictPolicy = new unordered_map<u_int64_t, vector<float>*>();
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
                    auto startSpeed  = Point(0,0,s);
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
        statesIdDict = xx->hashDictStates;
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
        delete (statesIdDict);
    };
    void reset_policy() override{};
    void policy_data() const override{};
    const std::vector<float>* TransitionAction(State*) override;
    void normalizeDict();

    void treeTraversal(State *ptrState, string &strIDExp);
    void policyData(string &strID);
    vector<float> minizTrans(const vector<float>* x);
};



Point PathPolicy::get_action(State *s) {
    auto EntryIndx = getAgentSateHash(s);
    //trajectory.push_back(s->to_string_state()+"="+std::to_string(EntryIndx));
    std::unordered_map<u_int64_t , std::vector<float>*>::iterator it;
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
    auto EntryIdx = getAgentSateHash(s);
    auto pos = this->dictPolicy->find(EntryIdx);
    if (pos==this->dictPolicy->end())
        throw std::runtime_error(std::string("Error: cant find the key "));

    return this->dictPolicy->at(EntryIdx);

}

u_int64_t PathPolicy::getAgentSateHash(State *s) {
    //cout<<s->to_string_state()<<endl;
    auto hSpeed = s->get_speed(this->id_agent).hashConst(Point::maxSpeed);
    auto hPos = s->get_position(this->id_agent).hashConst();
    u_int64_t EntryIndx = Point::hashNnN(hPos,hSpeed);
    return EntryIndx;
}

/**
 * TreeTraversal output to csv file the path of the agents,
 * with the probabilities for each path.
 *
 * @param pointer of the initial state object
 * @return void (write to disk csv file)
 *
 * **/
void PathPolicy::treeTraversal(State *ptrState,string &strIdExp)
{

    vector<std::tuple<double,vector<Point>>> res;
    std::deque<pair<State,float>> q;
    q.emplace_back(*ptrState,1);
    float probAcc=1;
    vector <pair<State,float>> path;
    while (!q.empty())
    {
        auto pairCur = q.front();
        auto curState = std::get<0>(pairCur);
        auto prob =std::get<1>(pairCur);

        //cout<<curState.to_string_state()<<endl;
        q.pop_front();
        if(curState.isEndState())
        {
            vector<Point> v;
            path.emplace_back(curState,prob);
            //v.push_back(std::to_string(probAcc));
            for( auto item : path)
            {
                auto posA = item.first.get_position(this->id_agent);
                v.push_back(posA);
                //cout<<posA.to_str()<<",";
            }
            //cout<<endl;
            auto posPair = path[path.size()-1];
            auto p = posPair.second;
            probAcc = probAcc/p;
            path.pop_back();
            res.emplace_back(probAcc,move(v));
            continue;
        }
        if (!path.empty())
        if (curState.to_string_state() == std::get<0>( path[path.size()-1]).to_string_state())
        {
            auto posPair = path[path.size()-1];
            auto p = posPair.second;
            probAcc = probAcc/p;
            path.pop_back();
            continue;
        }
        path.emplace_back(curState,prob);
        auto next = minizTrans(this->TransitionAction(&curState));
        q.push_front({curState,probAcc});
        probAcc=probAcc*prob;
        for (int i = 0 ; i<next.size() ; ++i)
        {
            auto pos = this->hashActionMap->find(next.operator[](i));
            if ( pos == this->hashActionMap->end())
                throw std::invalid_argument("Action index is invalid");
            Point *actionI = pos->second;
            State tmp(curState);
            tmp.applyAction(this->id_agent,*actionI,this->max_speed);
            //cout<<tmp.to_string_state()<<endl;
            q.push_front({tmp,next[++i]});
        }

    }
    pathz = res;
    policyData(strIdExp);
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

vector<float> PathPolicy::minizTrans(const vector<float> *x) {
    vector<float> newVector;
    unordered_map<float,float> d;
    for (int i = 0; i <x->size() ; ++i) {
        auto keyH = x->operator[](i);
        auto prob = x->operator[](++i);
        auto pos = d.find(keyH);
        if (pos==d.end())
            d.insert({keyH,prob});
        else
            d[keyH]+=prob;
    }
    for (auto item : d) {
        newVector.push_back(item.first);
        newVector.push_back(item.second);
    }
    return newVector;
}


void PathPolicy::policyData( string &strID)
{
    string pathFile=this->home+"/car_model/exp/data/";
    try{
        string nameFileCsv=strID+"_Attacker.csv";
        csvfile csv(std::move(pathFile+nameFileCsv),","); // throws exceptions!
        for(auto &item: pathz)
        {
            const auto& [probability,pathI] = item;
            csv<<probability;
            for (const auto& pointStr:pathI) {
                csv<<pointStr.to_str();
            }
            csv<<endrow;
        }
    }
    catch (const std::exception &ex){std::cout << "Exception was thrown: " << ex.what() << std::endl;}

}


#endif //TRACK_RACING_PATHPOLICY_HPP
