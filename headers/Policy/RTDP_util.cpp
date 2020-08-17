//
// Created by ise on 15.12.2019.
//


#include "RTDP_util.hpp"

RTDP_util::RTDP_util(int grid_size,string &mHome):home(mHome),last_entry() {
    this->hashActionMap=Point::getDictAction();
    size_mapAction = hashActionMap->size();
    HashFuction=[](const State *s){return s->getHashValue();};
}


double RTDP_util::applyNonAction(const State *s)
{
    if(_stochasticMovement==1)
        return R.CollReward;
    State oldState = State(*s);
    Point p(0);
    bool isWall = this->apply_action(&oldState,my_policy->id_agent,p,my_policy->max_speed);
    if(isWall)
        return R.WallReward;
    return R.CollReward;

}



void RTDP_util::heuristic(const State *s,keyItem entry_index)
{
    vector<State*> vec_q;
    auto oldState = new State(*s);

    // get the reward for action (0,0,0)
    double zero_move_reward = applyNonAction(s);

    for (const auto &item_action : *this->hashActionMap)
    {
        // apply action state and let the envirmont to roll and check the reward/pos
        Point *actionCur = item_action.second;
        double val;
        bool isWall = this->apply_action_SEQ(oldState,my_policy->id_agent,*actionCur,this->my_policy->max_speed);
        //bool isWall = this->apply_action(oldState,my_policy->id_agent,*actionCur,my_policy->max_speed);
       // std::cout << "CPU time used: " << time_elapsed_ms << " ms\n";
        if (isWall)
            val = this->R.WallReward*discountFactor*this->_stochasticMovement+
                    zero_move_reward*(1-this->_stochasticMovement);
        else{
            val=this->R.CollReward*discountFactor;
        }

        //cout<<"A:"<<actionCur->to_str()<<" val="<<val<<endl;
        oldState->assignment(*s,this->my_policy->id_agent);
        // insert to Q table

        this->set_value_matrix(entry_index,*actionCur,val);
    }
    delete(oldState);
}

double RTDP_util::rec_h(State *s,int index, double acc_probablity)
{
    double res_h=0;
    if (index+1==this->my_policy->tran.size()) {
        auto fin = compute_h(s)*acc_probablity;
//        cout<<fin<<endl;
        return fin;
    }
    auto old_state = State(*s);
    index++;
    auto res = my_policy->tran[index]->TransitionAction(s);
    // waring need to del the res (Pointer)
    for (int i = 0; i < res->size(); ++i)
    {
        auto pos = this->hashActionMap->find(res->operator[](i));
        auto action = pos->second;
        this->apply_action(s,my_policy->tran[index]->id_agent
                ,*action,my_policy->tran[index]->max_speed);
        res_h+=this->rec_h(s,index,acc_probablity*res->operator[](++i));
        s->assignment(old_state,my_policy->tran[index]->GetId());
    }
    return res_h;
}

void RTDP_util::add_entry_map_state(keyItem key,const State *s) {
    this->debugDict.insert({key,s->to_string_state()});
    // compute heuristic
    #ifdef VECTOR
    this->qTable->try_emplace(key,27);
    #endif
    this->heuristic(s,key);
    ctr_state++;

}

RTDP_util::~RTDP_util() {
    this->policyData();
    cout<<"state genrated:\t"<<ctr_state<<endl;
    cout<<"size_Q:\t"<<size_Q<<endl;
    std::for_each(hashActionMap->begin(),hashActionMap->end(),[](auto &item)
    {delete item.second;});
    delete(hashActionMap);

}

void RTDP_util::arg_max(arr &arr,vector<int>& listIdxs){
    double max = -1;
    max = *std::max_element(arr.begin(), arr.end());
    listIdxs.reserve(1);
    for (int i = 0; i < arr.size(); ++i) {
        if (arr.operator[](i)==max)
            listIdxs.emplace_back(i);
    }
}



int RTDP_util::get_state_argmax(const State *s) {
    //int argMax = -1;
    keyItem key = getStateKeyValue(s);

    auto &row = this->get_Q_entry_values(s, key);

//    if(this->my_policy->evalPolicy ) {
//        cout << "[state] " << s->to_string_state() << endl;
//        for (int i = 0; i < row.size(); ++i)
//            cout << "  [" << i << "]=" << row[i];
//        cout << endl;
//    }

    vector<int> argMax_list;
    arg_max(row, argMax_list);
    this->last_entry = key;
//    if(this->my_policy->evalPolicy ) {
//        cout<<"action:\t"<<argMax_list.front()<<endl;
//    }
//    if(s->to_string_state()=="0A_(16, 17, 2)_(2, 2, 0)_0|0D_(20, 19, 0)_(-1, -1, 0)_1|")
//    {
//        cout<<key<<endl;
//        cout<<qTable->at(key)<<endl;
//        cout<<argMax_list<<endl;
//    }
    return argMax_list.front();

}

vector<double>* RTDP_util::get_probabilty(const State *s) {
    auto &row = this->get_Q_entry_values(s,getStateKeyValue(s));
    vector<int> argMax_list;
    arg_max(row,argMax_list);
    auto* l = new vector<double>();
    int prob = argMax_list.size();
    for (auto item: argMax_list){
        l->push_back(item);
        l->push_back(double(prob)/1);
    }
    return l;
}




double getMaxValueArrTmp( const double *arr, size_t sizeArr)
{
    double max=*arr;
    for (int i = 0; i < sizeArr; ++i) {
        if (max < *(++arr))
            max=*(arr);
    }
    return max;
}



Point RTDP_util::get_argmx_action(State *s) {
    //cout<<s->to_string_state()<<endl;
    int index_action = this->get_state_argmax(s);

    auto pos = this->hashActionMap->find(index_action);
    if (pos == this->hashActionMap->end())
        throw std::invalid_argument( "function::get_argmx_action Error" );
    // assert
    return *pos->second;
}

bool RTDP_util::apply_action(State *s,const string &id,Point &action,int max_speed)
{
    return s->applyAction(id, action, max_speed);
}

bool RTDP_util::apply_action_SEQ(State *s,const string &id,Point &action,int max_speed)
{

    int seq = s->get_budget(this->my_policy->id_agent);
    bool is_wall_agent=false;
    if(true)
    {
        for(int i=0;i<seq and !is_wall_agent ;++i)
            is_wall_agent = apply_action(s,id,action,max_speed);
        return is_wall_agent;
    }

}



double RTDP_util::compute_h(State *s) {
//    cout<<s->to_string_state()<<endl;
    char team = this->my_policy->id_agent[1];
    auto my_pos = s->get_position_ref(this->my_policy->id_agent);
    vector<Point> vec_pos;
    s->getAllPosOpponent(vec_pos,team);
    double min = s->g_grid->getSizeIntGrid();
    double posA = -1;
    for (auto & vec_po : vec_pos) {
        auto res = getMaxDistance(vec_po,my_pos);
        if (min>res)
        {
            min=res;
        }
    }
    int max_speed=-1;
    for (Policy* itemPolicy:*this->lTran) {
        max_speed = itemPolicy->max_speed;
    }
    min=min-max_speed;
    min = std::max(0.0,min);
    //min=min/double(this->my_policy->max_speed);
    auto res = this->R.CollReward*pow(discountFactor,min);
    //debug
    //cout<<"h(<"<<s->to_string_state()<<")="<<res<<endl;
    return res;
}
void RTDP_util::plusplus(){this->ctr_debug++;}

void RTDP_util::policyData() {
    //return;
    string pathFile=this->home+"/car_model/debug/"+std::to_string(ctr_debug);

    //csv action state-------------------------------
    try{
        string nameFileCsv="ActionMap.csv";
        csvfile csv(std::move(pathFile+nameFileCsv),";"); // throws exceptions!
        csv << "ID" << "Entry" << endrow;
        for(auto &item: *this->hashActionMap)
        {
            csv<<item.first<<item.second->to_str()<<endrow;
        }

    }
    catch (const std::exception &ex){std::cout << "Exception was thrown: " << ex.what() << std::endl;}



    //print Q table--------------------------------
    try{
        string nameFileCsv="Q.csv";
        int size_action = this->hashActionMap->size();
        csvfile csv(std::move(pathFile+nameFileCsv),";"); // throws exceptions!
        csv<<"id";
        for (int k = 0; k <size_action; ++k)
            csv<<k;
        csv<<endrow;
        for(auto &item1:*qTable)
        {
            csv<<item1.first;
            std::for_each(item1.second.begin(),item1.second.end(),[&](auto val)
            {
                csv<<val;
            });
            csv<<endrow;
        }


    }

    catch (const std::exception &ex){std::cout << "Exception was thrown: " << ex.what() << std::endl;}
    try{
        string nameFileCsv="dico.csv";
        csvfile csv(std::move(pathFile+nameFileCsv),";"); // throws exceptions!
        for(auto &item:debugDict)
        {
            csv<<item.first;
            csv<<item.second;
            csv<<endrow;
        }
        debugDict.clear();
    }
    catch (const std::exception &ex){std::cout << "Exception was thrown: " << ex.what() << std::endl;}

}

void RTDP_util::update_final_State(State *s, double val) {
    auto entryIndex= this->HashFuction(s);
    #ifdef VECTOR
    qTable->try_emplace(entryIndex,27);
    #endif
    for (auto &[i,p]: *this->hashActionMap)
    {
        set_value_matrix(entryIndex,*p,val);
    }

}

void RTDP_util::reduceMap() {

    for(auto &item:*qTable)
    {
        auto ArgMax = std::distance(item.second.begin(),std::max_element(item.second.begin(),item.second.end()));
        item.second.clear();
        vector<cell>().swap(item.second);
        item.second.shrink_to_fit();
        item.second.push_back(ArgMax);

    }

}

void RTDP_util::resetQtable() {
    qTable->clear();
}

