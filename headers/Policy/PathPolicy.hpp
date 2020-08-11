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
typedef std::unordered_map<u_int64_t,vector<double>*> dictHashAction;
typedef std::vector<std::pair<double,Point>> listPointWeighted;

class PathPolicy:public Policy{
    unsigned long maxPathsNumber;
    vector<Point> midVec;

    bool dont_del=false;
    //u_int64_t getAgentSateHash(const State *s);
public:
    void set_dontDel(bool b){dont_del=b;}
    [[nodiscard]] auto get_dictPolicy_size()const{return share_dictHashAction->size();}
    vector<weightedPosition> startPoint;
    unordered_map<u_int64_t ,pair<short,AStar::StatePoint>>* statesIdDict;
    shared_ptr<dictHashAction> share_dictHashAction;
    std::vector<std::pair<double,Point>>* goalPoint;
    vector<pair<double,vector<Point>>> myPaths;
    [[nodiscard]] std::shared_ptr<dictHashAction> getDictPolicy() const{return share_dictHashAction;}
    //void setDictPolicy(dictHashAction* obj){dictPolicy=obj;}
    int getNumberOfState() {
        return share_dictHashAction->size();
    }
    PathPolicy(string namePolicy, int maxSpeedAgent,std::vector<std::pair<double,Point>>* endPoint_, vector<weightedPosition>& startPoint_,
               Point &gridSzie, const string &agentID,vector<Point> midVecPoints,string &home,unsigned long maxPathz=ULONG_MAX,dictionary ptrDict=nullptr) : Policy(std::move(namePolicy),
                       maxSpeedAgent,agentID,home,std::move(ptrDict)),midVec(move(midVecPoints)) {
        this->goalPoint=endPoint_;
        this->startPoint=startPoint_;
        this->statesIdDict= nullptr;
        this->maxPathsNumber = maxPathz;
        this->initPolicy(gridSzie);
        printf("\ndone!\n");
    }

    PathPolicy(string namePolicy, int maxSpeedAgent,const string &agentID,string &home,unordered_map<u_int64_t,vector<double>*>* d,dictionary ptrDict=nullptr)
    :Policy(std::move(namePolicy),maxSpeedAgent,agentID,
            home,std::move(ptrDict)),share_dictHashAction(d),goalPoint(nullptr),
            statesIdDict(nullptr),maxPathsNumber(0),startPoint(0),midVec(0)
    {
        //share_dictHashAction=std::make_shared<auto>(d);
    }
    
    void initPolicy(Point &girdSize){
        share_dictHashAction = std::make_shared<dictHashAction>();

        double weightEnd;
        cout<<"A star..."<<endl;
        auto *AstarObject = new AStar::Generator(this->max_speed,girdSize);
        AstarObject->setMaxPATH(this->maxPathsNumber);
        for (auto & startPointItem : startPoint) {
            for (unsigned long k = 0; k < this->goalPoint->size(); ++k) {
                auto& [weightEnd,endP] = goalPoint->operator[](k);
                for (unsigned int s=max_speed ; s<=this->max_speed;++s)
                {
                    auto zeroSrc  = Point();
                    auto startSpeed  = startPointItem.speedPoint;
                    auto src = AStar::StatePoint{Point(startPointItem.positionPoint),startSpeed};
                    auto dest = AStar::StatePoint{Point(endP),startSpeed};
                    AstarObject->changeMaxSpeed(s);
                    if (midVec.size()>k)
                        AstarObject->findComplexPath(src,midVec[k],dest);
                    else
                        auto res = AstarObject->findPath(src,dest);
                    
                }
                AstarObject->getDict(share_dictHashAction.get(),weightEnd);
                AstarObject->dictPolyClean();
            }
        }

        normalizeDict();
        statesIdDict = AstarObject->hashDictStates;
        copyPaths(AstarObject);
        delete(AstarObject);


    }
    inline void copyPaths(AStar::Generator *astar)
    {

    }
    Point get_action(State *s) override;
    ~PathPolicy() override{
        cout<<"del A star"<<endl;
        if(dont_del){
            cout<<"dont_del PathPolicy"<<endl;
            return;
        }
        for(auto &item : *this->share_dictHashAction)
        {
            delete(item.second);
        }
        delete (statesIdDict);
        delete(goalPoint);
    };
    void reset_policy() override{};
    void policy_data() const override{};
    const std::vector<double>* TransitionAction(const State*) const override;
    void normalizeDict();


    vector<double> minizTrans(const vector<double>* x);

    void policyData(string &strId);

    void policyDataAbstract(const string &strId, const Point& ab);

    void treeTraversal(State *ptrState, string &strIdExp, const Point *ab= nullptr);

    u_int64_t getAgentSateHash(const State *s)const;
};



Point PathPolicy::get_action(State *s) {
    auto EntryIndx = getAgentSateHash(s);
    //trajectory.push_back(s->to_string_state()+"="+std::to_string(EntryIndx));
    std::unordered_map<u_int64_t , std::vector<double>*>::iterator it;
    it = this->share_dictHashAction->find(EntryIndx);
    if (it==this->share_dictHashAction->end())
        throw;
    // get value between zero to one
    double r = static_cast <double> (random()) / static_cast <double> (RAND_MAX);
    double acc=0;
    unsigned long i;
    for (i=1; i <it->second->size() ; ++i) {
        acc+=it->second->operator[](i);
        if (r<=acc)
            break;
        i+=1;
    }
    // problem with precision number (doubleing point)
    if (it->second->size()<i)
    {
        i=it->second->size()-1;
    }

    int indexI =int(it->second->operator[](int(i-1)));
    Point choosenAction = *this->hashActionMap->at(indexI);
    return choosenAction;

}

const std::vector<double> *PathPolicy::TransitionAction(const State *s)const {
    /* first  - action
     * second - probabilitiy*/
    //cout<<s->to_string_state()<<endl;
    auto EntryIdx = getAgentSateHash(s);
    auto pos = this->share_dictHashAction->find(EntryIdx);
    if (pos==this->share_dictHashAction->end())
        throw std::runtime_error(std::string("Error: cant find the key "));

    return this->share_dictHashAction->at(EntryIdx);

}

u_int64_t PathPolicy::getAgentSateHash(const State *s)const {
    //cout<<s->to_string_state()<<endl;
    auto hSpeed = s->get_speed(this->id_agent).hashConst(Point::maxSpeed);
    auto hPos = s->get_position_ref(this->id_agent).hashConst();
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
void PathPolicy::treeTraversal(State *ptrState,string &strIdExp,const Point *ab)
{

    std::deque<pair<State,double>> q;
    q.emplace_back(*ptrState,1);
    double probAcc=1;
    vector <pair<State,double>> path;
    while (!q.empty())
    {
        auto pairCur = q.front();
        auto curState = std::get<0>(pairCur);
        auto prob =std::get<1>(pairCur);

        //cout<<curState.to_string_state()<<endl;
        q.pop_front();
        if(curState.isEndState(this->id_agent))
        {
            vector<Point> v;
            path.emplace_back(curState,prob);
            //v.push_back(std::to_string(probAcc));
            for( auto item : path)
            {
                auto posA = item.first.get_position_ref(this->id_agent);
                v.push_back(posA);
                //cout<<posA.to_str()<<",";
            }
            //cout<<endl;
            auto posPair = path[path.size()-1];
            auto p = posPair.second;
            probAcc = probAcc/p;
            path.pop_back();
            myPaths.emplace_back(probAcc,std::move(v));
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
    policyData(strIdExp);
    if(ab)
    policyDataAbstract(strIdExp,*ab);
}


void PathPolicy::normalizeDict(){
    for(auto &item : *this->share_dictHashAction)
    {
        const size_t vecSize = item.second->size();
        double sumProbability=0;
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

vector<double> PathPolicy::minizTrans(const vector<double> *x) {
    vector<double> newVector;
    unordered_map<double,double> d;
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


void PathPolicy::policyData( string &strId)
{
    string pathFile=this->home+"/car_model/exp/data/";
    try{
        string nameFileCsv= strId + "_Attacker.csv";
        csvfile csv(std::move(pathFile+nameFileCsv),","); // throws exceptions!
        for(auto &item: myPaths)
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
void PathPolicy::policyDataAbstract( const string &strId, const Point& ab)
{
    string pathFile=this->home+"/car_model/exp/data/";
    try{
        string nameFileCsv= strId + "_Attacker_Abstract.csv";
        csvfile csv(std::move(pathFile+nameFileCsv),","); // throws exceptions!
        for(auto &item: myPaths)
        {
            const auto& [probability,pathI] = item;
            csv<<probability;
            for (const auto& pointStr:pathI) {
                csv<<(pointStr/ab).to_str();

            }
            csv<<endrow;
        }
    }
    catch (const std::exception &ex){std::cout << "Exception was thrown: " << ex.what() << std::endl;}

}

#endif //TRACK_RACING_PATHPOLICY_HPP
