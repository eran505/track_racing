//
// Created by ise on 15.12.2019.
//

#include "RtdpAlgo.hpp"

#include <utility>
RtdpAlgo::RtdpAlgo(int maxSpeedAgent, int grid_size, vector<pair<int,int>>& max_speed_and_budget,const string &agentID,string &home,dictionary &ptrDict,short miniGrid)
        : Policy("RTDP", maxSpeedAgent,agentID,home,ptrDict) {
   this->RTDP_util_object = new RTDP_util(grid_size,max_speed_and_budget,home);
    this->RTDP_util_object->set_tran(&this->tran);
    this->RTDP_util_object->MyPolicy(this);
    if(miniGrid==2)
        this->evaluationState = [this](State *s){return this->EvalState2(s);};
    else if(miniGrid==3)
        this->evaluationState = [this](State *s){return this->EvalState3(s);};
    else this->evaluationState = [this](State *s){return this->EvalState(s);};
}



void RtdpAlgo::reset_policy() {
    this->empty_stack_update();
    Policy::reset_policy();
}


const vector<double >* RtdpAlgo::TransitionAction(State *s)
{

    return this->RTDP_util_object->get_probabilty(s);
}

Point RtdpAlgo::get_action(State *s)
{
    //return the argmax action in the given state row
    auto action = this->RTDP_util_object->get_argmx_action(s);
    //cout<<"___action:\t"<<action.to_str()<<"\t";
    if (this->evalPolicy)
    {
        //auto actionIndx = int(this->RTDP_util_object->get_max_valueQ(s));
        //action = *this->hashActionMap->find(actionIndx)->second;
        if (!s->takeOff)
            if(action.hashMeAction(Point::actionMax)!=13 ){
                //s->set_speed(this->id_agent,Point(0,0,max_speed)); //#TODO: uncomment this !!!!
                //this->inset_to_stack(s,action,entry);
                //action.array[2]=this->max_speed;
                s->takeOff=true;
            }
        return action;
    }

    if(ctrInFun%400000==0) cout<<"QTable updates: "<<this->RTDP_util_object->get_update_ctr()<<endl;
    ctrInFun++;


    u_int64_t entry=this->RTDP_util_object->last_entry;
    //update state action
    // set the max speed in the Z coordinate at the when taking off
    //inset to stack for backup update
    this->inset_to_stack(s,action,entry);

    if (!s->takeOff)
        if(action.hashMeAction(Point::actionMax)!=zeroIndexAction ){
            //s->set_speed(this->id_agent,Point(0,0,max_speed));
            //this->inset_to_stack(s,action,entry);
            //action.array[2]=this->max_speed;
            s->takeOff=true;
            //this->tmp=s->to_string_state();
        }
    //TODO: inset the state action tuple to the stack to update at the end of the episode

    this->update(s,action,entry);


    if(stoMove())
    {
        //cout<<"  [zero]  ";
        return s->get_speed(this->id_agent)*-1;
    }
    return action;
}

//double RtdpAlgo::expected_reward(State *s, Point &action)
//{
//    deque<Point> stacAction;
//    int index_pos = 0;
//    stacAction.push_back(action);
//    this->RTDP_util_object->applyAction(s,this->id_agent,action,this->max_speed);
//
//}
//

double RtdpAlgo::bellman_update(State *s, Point &action) {
    auto stateCur = new State(*s);
    // generate all possible next state with the probabilities
    vector <pair<State*,double>> state_tran_q;


    this->applyActionToState(stateCur, &action);
    if(_stochasticMovement!=1)
    {
        auto zeroState = new State(*s);
        auto slideAction = s->get_speed(this->id_agent)*-1;
        this->applyActionToState(zeroState,&slideAction);
        state_tran_q.emplace_back(zeroState,1-_stochasticMovement);
        state_tran_q.emplace_back(stateCur,_stochasticMovement);
    }else{
        state_tran_q.emplace_back(stateCur,1);
    }

    // is wall
    if (this->is_wall == true)
    {
        //do something
    }

    for (Policy *item_policy: tran)
    {
        vector <pair<State*,double>> state_tran_q_tmp;
        for (const auto &value: state_tran_q)
        {

            double probability = value.second;

            // Warning MUST del the Tran vec (options_actions)!!!
            auto options_actions = item_policy->TransitionAction(value.first);
            for(std::vector<double>::size_type i = 0; i != options_actions->size(); i++) {
                // copy state
                auto *new_state = new State(*value.first);
                // take the action
                auto pos = this->hashActionMap->find(options_actions->operator[](i));
                if ( pos == this->hashActionMap->end()) {
                    throw std::invalid_argument("Action index is invalid");;
                } else {
                    Point *actionI = pos->second;
                    item_policy->applyActionToState(new_state,actionI);
                    state_tran_q_tmp.emplace_back(new_state,options_actions->operator[](++i)*probability);
                }
            }
            //delete(options_actions);
            //del the state that were alloc on the heap
            delete(value.first);
        }
        state_tran_q=state_tran_q_tmp;
    }

    return this->UpdateCalc(state_tran_q);
}
tuple<double,bool> RtdpAlgo::EvalState2(State *s)
{
    if (s->g_grid->is_wall(s->get_position_ref(this->GetId())))
    {
        return {WallReward,true};
    }
    if (s->is_collusion(this->id_agent,this->cashID))
    {
        return {getReward(s->get_position_ref(this->id_agent)),true};
    }
    if (auto x = s->isGoal(this->cashID);x>=0)
        return {GoalReward*x,true};
    return {0,false};
}

tuple<double,bool> RtdpAlgo::EvalState(State *s) {

    if (s->isGoal(this->cashID)>=0) {
        return {GoalReward,true};
    } else if (s->g_grid->is_wall(s->get_position_ref(this->GetId()))){
        return {WallReward,true};
    }else if(s->isEndState(this->cashID))
    {
        return {0,true};
    }
    else{
            auto res = s->is_collusion(this->id_agent);
            if (!res.empty())
            {
                return {CollReward,true};
            }
    }
    return {0,false};
}

tuple<double,bool> RtdpAlgo::EvalState3(State *s) {


    if (s->g_grid->is_wall(s->get_position_ref(this->GetId()))){
        return {WallReward,true};
    }
    auto res = s->is_collusion(this->id_agent,this->cashID);
    if (res){
        if(get_lastPos()==s->get_position_ref(this->id_agent))
            return {CollReward,true};
    }
    if (s->isGoal(this->cashID)>=0) {
        return {GoalReward, true};
    }
    if(s->isEndState(this->cashID)){
            return {0,true};
    }
    return {0,false};
}

const Point& RtdpAlgo::get_lastPos() const{
    return this->stackStateActionIdx.back().first.get_position_ref(this->id_agent);

}

double RtdpAlgo::UpdateCalc(const vector<pair<State *, double>>& state_tran_q) {
    double res=0;
    for (auto &item:state_tran_q)
    {
        auto [val,isSndState] = this->evaluationState(item.first);
        if (!isSndState)
            val = this->RTDP_util_object->get_max_valueQ(item.first);
        res+=val*item.second*this->RTDP_util_object->discountFactor;
        delete(item.first);
    }
    return res;
}

void RtdpAlgo::update(State *s, Point &action,u_int64_t entryMatrix)
{
    auto val = this->bellman_update(s,action);
    this->RTDP_util_object->set_value_matrix(entryMatrix,action,val);
}

void RtdpAlgo::inset_to_stack(State *s,Point &action,u_int64_t state_entry)
{
    stackStateActionIdx.push_back({State(*s),{state_entry,action.hashMeAction(Point::actionMax)}});
    ctr_stack++;
}

void RtdpAlgo::empty_stack_update() {
    for (int i = ctr_stack-1 ; i > -1 ; --i) {
        // update the action state
        auto pos = this->hashActionMap->find(this->stackStateActionIdx[i].second.second);
        if (pos==this->hashActionMap->end())
            throw;
        this->update(&this->stackStateActionIdx[i].first,*pos->second,
                this->stackStateActionIdx[i].second.first);

    }
    ctr_stack=0;
    this->stackStateActionIdx.clear();
}

void RtdpAlgo::policy_data() const {
    this->RTDP_util_object->policyData();
}

bool RtdpAlgo::stoMove() {
    if(this->_stochasticMovement==1)
        return false;
    auto rand = this->getRandom();
    if (_stochasticMovement>=rand)
        return false;
    return true;
}


double RtdpAlgo::getReward(const Point &refPoint)const {
    if(rewardDict->empty())
        return this->CollReward;
    if(auto pos = this->rewardDict->find(refPoint.expHash());pos==rewardDict->end())
        return this->GoalReward;
    else
        return pos->second;
}

void RtdpAlgo::learnRest() {
    double epsilon_reward = 0.00001;
    cout<<"print - dict goal\n";
    for(auto &item : *rewardDict) cout<<"{"<<item.first<<", "<<item.second<<"}\t";
    cout<<endl;
    Policy::learnRest();
    this->RTDP_util_object->resetQtable();
    bool the_same=true;
    if(rewardDict->empty())
        return;
    auto first_val = rewardDict->begin()->second;
    for(auto &item:*rewardDict){
        if(std::abs(first_val-item.second)>epsilon_reward){
            the_same=false;
        }
    }
    if(the_same)
    {
        for(auto &item:*rewardDict) this->rewardDict->clear();
    }
    else{
        for(auto &item:*rewardDict)
            if(item.second<epsilon_reward)
                item.second=epsilon_reward;
    }
    cout<<"print - dict goal\n";
    for(auto &item : *rewardDict) cout<<"{"<<item.first<<", "<<item.second<<"}\t";
    cout<<endl;
}

//double RtdpAlgo::expected_reward_rec(State *s, int index_policy, deque<Point> &my_stack) {
//    return 0;
//}





