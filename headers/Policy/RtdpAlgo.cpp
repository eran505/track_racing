//
// Created by ise on 15.12.2019.
//

#include "RtdpAlgo.hpp"

#include <utility>
RtdpAlgo::RtdpAlgo(int maxSpeedAgent, int grid_size,State::agentEnum agentID,string &home)
        : Policy(maxSpeedAgent,agentID,home){
   this->RTDP_util_object = new RTDP_util(grid_size,home);
    this->RTDP_util_object->set_tran(&this->tran);
    this->RTDP_util_object->MyPolicy(this);
    this->stackStateActionIdx = std::make_shared<vector<pair<State,pair<u_int64_t,int>>>>();

}



void RtdpAlgo::reset_policy() {
    this->empty_stack_update();
    this->evaluator->reset(this->getUtilRTDP());
    Policy::reset_policy();
}


const vector<double >* RtdpAlgo::TransitionAction(const State *s)const
{

    //return nullptr;
    return this->RTDP_util_object->get_probabilty(s);
}

Point RtdpAlgo::get_action(State *s)
{
    //return the argmax action in the given state row

    Point action = this->RTDP_util_object->get_argmx_action(s);


    //if(ctrInFun%400000==0) cout<<"QTable updates: "<<this->RTDP_util_object->get_update_ctr()<<endl;



    u_int64_t entry=this->RTDP_util_object->last_entry;
    //update state action
    // set the max speed in the Z coordinate at the when taking off

    //cout<<Point(0).hashMeAction(Point::actionMax)<<endl;
    if (!s->takeOff) {
        if (action.hashMeAction(Point::actionMax) != zeroIndexAction)
             { s->takeOff = true; }
    }
    //inset to stack for backup update
    this->stack_backup.inset_to_stack({State(*s),Point(action),entry});

    //TODO: inset the state action tuple to the stack to update at the end of the episode
    //std::clock_t c_start = std::clock();
    //this->update(s,action,entry);
    //std::clock_t c_end = std::clock();
    //double time_elapsed_ms = 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC;
    //std::cout << "CPU time used: " << time_elapsed_ms << " ms\n";
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
    expnder->clean();
    return evaluator->calculate(expnder->expnad_state(s,action));
}

cell RtdpAlgo::bellman_updateV2(State *s, Point &action) {
    //expnder->clean();
    #ifdef PRINT
    cout<<"[bellman]"<<s->to_string_state()<<endl;
    #endif
    do_SEQ(s,action);
    return evaluator->calculateV2_back(expnder->expand_state_other(s),*s);
}

Point RtdpAlgo::get_lastPos() {
    assert(false);
    return Point(0);
}

double RtdpAlgo::UpdateCalc(const vector<pair<State *, double>>& state_tran_q) {
    double res=0;
    for (auto &item:state_tran_q)
    {

        auto [val,isSndState] = this->evaluationState(item.first);
        #ifdef PRINT
        cout<<item.first->to_string_state()<<"  val:"<<val<<" emd:"<<isSndState<<" p="<<item.second<<"\t";
        #endif
        if (!isSndState)
            val = this->RTDP_util_object->get_max_valueQ(item.first);
        res+=val*item.second*this->RTDP_util_object->discountFactor;
        delete(item.first);
    }
    return res;
}

void RtdpAlgo::update(State *s, Point &action,u_int64_t entryMatrix)
{

    auto val = this->bellman_updateV2(s,action);
    #ifdef PRINT
    cout<<" [update] Q["<<entryMatrix<<", "<<action.hashMeAction(Point::actionMax)<<"]="<<val<<endl;
    cout<<s->to_string_state()<<" H="<<entryMatrix<<" A="<<action.to_hash_str()<<"\tval="<<val<<endl;
    #endif
    this->RTDP_util_object->set_value_matrix(entryMatrix,action,val);
}

void RtdpAlgo::inset_to_stack(State *s,Point &action,u_int64_t state_entry)
{
    stackStateActionIdx->push_back({State(*s),{state_entry,action.hashMeAction(Point::actionMax)}});
    ctr_stack++;
}
//void RtdpAlgo::inset_to_stack_abs(State *s,Point &action,u_int64_t state_entry)
//{
//    stackStateActionIdx->push_back({this->transform_abstraction_DA(s),{state_entry,action.hashMeAction(Point::actionMax)}});
//    ctr_stack++;
//}

void RtdpAlgo::empty_stack_update() {
    if(this->stack_backup.is_empty()) return;
    //if(evalPolicy) return;
    //this->stack_backup.pop();
    #ifdef PRINT
    this->stack_backup.print_stak();
    #endif
    while(!this->stack_backup.is_empty()) {
        auto& item = this->stack_backup.pop();
        //this->evaluator->change_scope_(&item.state);
        this->update(&item.state,item.action, item.entryID);
    }
    this->stack_backup.clear();
    this->getUtilRTDP()->reset_takken_stpe_ctr();
}

void RtdpAlgo::policy_data() const {
#ifdef OUTDATA
    this->RTDP_util_object->policyData();
    //this->RTDP_util_object->plusplus();
#endif
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
        return this->R.CollReward;
    if(auto pos = this->rewardDict->find(refPoint.expHash());pos==rewardDict->end())
        return this->R.GoalReward;
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
    cout<<"print - dict goal\n";
    if(the_same)
    {
        for(auto &item:*rewardDict) item.second=R.CollReward;
        return;
    }

    double min_val = min_element(rewardDict->begin(), rewardDict->end(),
                [](const auto& l, const auto& r) { return l.second < r.second; })->second;

    std::for_each(rewardDict->begin(),rewardDict->end(),[&](auto &item){item.second=item.second+std::abs(min_val)+epsilon_reward;});

    double res = accumulate(rewardDict->begin(), rewardDict->end(), (double)0.0,
                          [&](double prior, const pair<u_int32_t , double > p) -> double {
                              return prior+p.second;});

    std::for_each(rewardDict->begin(),rewardDict->end(),[&](auto &item){item.second=item.second/res;});

    //DEBUG
    //std::for_each(rewardDict->begin(),rewardDict->end(),[&](auto &item){item.second=-1;});
    //rewardDict->operator[](33122146222978)=1;
    //DEBUG

    for(auto &item : *rewardDict) cout<<"{"<<item.first<<", "<<item.second<<"}\t";
    cout<<endl;
}
void RtdpAlgo::do_SEQ(State *s,const Point& a)
{
    auto atmp=Point(a);
    s->applyAction(this->id_agent,atmp,this->max_speed,int(s->get_budget(this->get_id_name())));

}

//double RtdpAlgo::expected_reward_rec(State *s, int index_policy, deque<Point> &my_stack) {
//    return 0;
//}





