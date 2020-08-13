//
// Created by ise on 15.12.2019.
//

#include "RtdpAlgo.hpp"
//#define PrinT

#include <utility>
RtdpAlgo::RtdpAlgo(int maxSpeedAgent, int grid_size,const string &agentID,string &home)
        : Policy("RTDP", maxSpeedAgent,agentID,home){
   this->RTDP_util_object = new RTDP_util(grid_size,home);
    this->RTDP_util_object->set_tran(&this->tran);
    this->RTDP_util_object->MyPolicy(this);
    set_mode_agent(2);
    this->stackStateActionIdx = std::make_shared<vector<pair<State,pair<u_int64_t,int>>>>();

}

void RtdpAlgo::set_mode_agent(int miniGrid)
{
    miniGrid=2;
    if(miniGrid==2)
        this->evaluationState = [this](State *s){return this->EvalState2(s);};
    else if(miniGrid==3)
        this->evaluationState = [this](State *s){return this->EvalState3(s);};
    else if(miniGrid==4)
        this->evaluationState = [this](State *s){return this->EvalState4(s);};
    else if(miniGrid==5)
    {
        abstraction_expnd=[&](State *s){this->transform_abstraction_A_inplace(s);};
        this->evaluationState = [this](State *s){return this->EvalState5(s);};
    }
    else this->evaluationState = [this](State *s){return this->EvalState(s);};

}

void RtdpAlgo::reset_policy() {
    this->empty_stack_update();
    this->evaluator->reset(this->getUtilRTDP());
    Policy::reset_policy();
}


const vector<double >* RtdpAlgo::TransitionAction(const State *s)const
{

    return this->RTDP_util_object->get_probabilty(s);
}

Point RtdpAlgo::get_action(State *s)
{
    //return the argmax action in the given state row
    Point action = this->RTDP_util_object->get_argmx_action(s);

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

    //if(ctrInFun%400000==0) cout<<"QTable updates: "<<this->RTDP_util_object->get_update_ctr()<<endl;
    ctrInFun++;


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
    this->update(s,action,entry);
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

//double RtdpAlgo::bellman_update(State *s, Point &action) {
    //_stochasticMovement=1;
//    auto stateCur = new State(*s);
//    // generate all possible next state with the probabilities
//    vector <pair<State*,double>> state_tran_q;
//    //cout<<"stateCur:\t"<<stateCur->to_string_state()<<endl;
//    if(abstract)
//    {
//        //this->transform_abstraction_D(stateCur);
//        //cout<<"[abstract] stateCur:\t"<<stateCur->to_string_state()<<endl;
//    }
//
//
//    this->applyActionToState(stateCur, action);
//    if(_stochasticMovement!=1)
//    {
//        auto zeroState = new State(*s);
//        //this->transform_abstraction_D(zeroState);
//        //auto slideAction = s->get_speed(this->id_agent)*-1;
//        auto slideAction=Point(0);
//        this->applyActionToState(zeroState,slideAction);
//        state_tran_q.emplace_back(zeroState,1-_stochasticMovement);
//        state_tran_q.emplace_back(stateCur,_stochasticMovement);
//
//    }else{
//        state_tran_q.emplace_back(stateCur,1);
//    }
//
//
//    for (Policy *item_policy: tran)
//    {
//        vector <pair<State*,double>> state_tran_q_tmp;
//        for (const auto &value: state_tran_q)
//        {
//
//            double probability = value.second;
//
//            // Warning MUST del the Tran vec (options_actions)!!!
//
//            auto options_actions = item_policy->TransitionAction(value.first);
//            for(std::vector<double>::size_type i = 0; i != options_actions->size(); i++) {
//                // copy state
//                auto *new_state = new State(*value.first);
//                // take the action
//                auto pos = this->hashActionMap->find(options_actions->operator[](i));
//                if ( pos == this->hashActionMap->end()) {
//                    throw std::invalid_argument("Action index is invalid");;
//                } else {
//                    Point *actionI = pos->second;
//                    item_policy->applyActionToState(new_state,*actionI);
//
//                    //this->abstraction_expnd(new_state);
//
//                    state_tran_q_tmp.emplace_back(new_state,options_actions->operator[](++i)*probability);
//                }
//            }
//            //delete(options_actions);
//            //del the state that were alloc on the heap
//            delete(value.first);
//        }
//        state_tran_q=state_tran_q_tmp;
//    }


    //double x = this->UpdateCalc(list_state);

//    return x;
//}
tuple<double,bool> RtdpAlgo::EvalState2(State *s)
{
    if (s->g_grid->is_wall(s->get_position_ref(this->id_agent)))
    {
        return {R.WallReward,true};
    }
    if (s->is_collusion(this->id_agent,this->cashID))
    {
        return {R.CollReward,true};
    }
    if (auto x = s->isGoal(this->cashID);x>=0)
        return {R.GoalReward*x,true};
    return {0,false};
}

tuple<double,bool> RtdpAlgo::EvalState(State *s) {

    if (s->isGoal(this->cashID)>=0) {
        return {R.GoalReward,true};
    } else if (s->g_grid->is_wall(s->get_position_ref(this->GetId()))){
        return {R.WallReward,true};
    }else if(s->isEndState(this->cashID))
    {
        return {0,true};
    }
    else{
            auto res = s->is_collusion(this->id_agent);
            if (!res.empty())
            {
                return {R.CollReward,true};
            }
    }
    return {0,false};
}

tuple<double,bool> RtdpAlgo::EvalState3(State *s) {


    if (s->g_grid->is_wall(s->get_position_ref(this->GetId()))){
        return {R.WallReward,true};
    }
    auto res = s->is_collusion(this->id_agent,this->cashID);
    if (res){
        if(get_lastPos()==s->get_position_ref(this->id_agent))
            return {getReward(s->get_position_ref(this->id_agent)),true};
    }
    if (s->isGoal(this->cashID)>=0) {
        return {R.GoalReward, true};
    }
    if(s->isEndState(this->cashID)){
            return {0,true};
    }
    return {0,false};
}

tuple<double,bool> RtdpAlgo::EvalState4(State *s) {

    double epsilon=0;
    if (s->g_grid->is_wall(s->get_position_ref(this->GetId()))){
        return {R.WallReward,true};
    }
    auto res = s->is_collusion(this->id_agent,this->cashID);
    if (res){
        if(get_lastPos()==s->get_position_ref(this->id_agent))
            if(s->get_speed_ref(this->id_agent)==zero_action)
                return {getReward(s->get_position_ref(this->id_agent)),true};
    }
    if (s->isGoal(this->cashID)>=0) {
        return {R.GoalReward, true};
    }
    if(s->isEndState(this->cashID)){
        return {0+epsilon,true};
    }
    return {0+epsilon,false};
}

tuple<double,bool> RtdpAlgo::EvalState5(State *s) {

    double epsilion=-0.1;
    if (s->g_grid->is_wall(s->get_position_ref(this->GetId()))){
        return {R.WallReward,true};
    }
    auto res = s->is_collusion_radius(this->id_agent,this->cashID,this->window);
    if (res){
        if(window.sum()-3==0)
            return {this->R.CollReward,true};
        Point last = ((get_lastPos()-offset)/abs);
        if(last==s->get_position_ref(this->id_agent))
            return {this->R.CollReward,true};
    }
    if (s->isGoal(this->cashID)>=0) {
        return {R.GoalReward, true};
    }
    if(s->isEndState(this->cashID)){
        return {0+epsilion,true};
    }
    //passBy
    if(!(s->get_position_ref(this->id_agent)>=s->get_position_ref(this->cashID)))
        return {R.GoalReward,true};
    return {0+epsilion,false};
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
        #ifdef PrinT
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
    #ifdef PrinT
    cout<<"action="<<action.to_str()<<" | ";
    #endif
    auto val = this->bellman_update(s,action);
    #ifdef PrinT
    cout<<" ["<<entryMatrix<<", "<<action.hashMeAction(Point::actionMax)<<"]="<<val<<endl;
    #endif
    //cout<<s->to_string_state()<<" H="<<entryMatrix<<" A="<<action.to_hash_str()<<"\tval="<<val<<endl;
    this->RTDP_util_object->set_value_matrix(entryMatrix,action,val);
}

void RtdpAlgo::inset_to_stack(State *s,Point &action,u_int64_t state_entry)
{
    stackStateActionIdx->push_back({State(*s),{state_entry,action.hashMeAction(Point::actionMax)}});
    ctr_stack++;
}
void RtdpAlgo::inset_to_stack_abs(State *s,Point &action,u_int64_t state_entry)
{
    stackStateActionIdx->push_back({this->transform_abstraction_DA(s),{state_entry,action.hashMeAction(Point::actionMax)}});
    ctr_stack++;
}

void RtdpAlgo::empty_stack_update() {
    if(this->stack_backup.is_empty()) return;
    this->stack_backup.pop();

    while(!this->stack_backup.is_empty()) {
        auto& item = this->stack_backup.pop();
        this->evaluator->change_scope_(&item.state);
        this->update(&item.state,item.action, item.entryID);
    }
    this->stack_backup.clear();
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

//double RtdpAlgo::expected_reward_rec(State *s, int index_policy, deque<Point> &my_stack) {
//    return 0;
//}





