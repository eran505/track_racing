//
// Created by ise on 15.12.2019.
//

#include "learning/FeatureGen.h"
#include "RTDP_util.hpp"

RTDP_util::RTDP_util(int grid_size,string &mHome):home(mHome),last_entry() {
    this->hashActionMap=Point::getDictAction();
    size_mapAction = hashActionMap->size();
//    FeatureGen featurer (this->my_policy->id_agent,)
    HashFuction=[](const State *s){return s->getHashValue();};
}


double RTDP_util::applyNonAction(const State *s)
{
    return R.CollReward;
    //if(_stochasticMovement==1)
        //return R.CollReward;
//    State oldState = State(*s);
//    Point p(0);
//    bool isWall = this->apply_action(&oldState,my_policy->id_agent,p,my_policy->max_speed);
//    if(isWall)
//        return R.WallReward;
//    return R.CollReward;

}



void RTDP_util::heuristic(const State *s,keyItem entry_index)
{
    vector<State*> vec_q;
    auto oldState = State(*s);

    // get the reward for action (0,0,0)
    double zero_move_reward = applyNonAction(s);
    bool isDebug=false;

//    if(entry_index == 9739533163006043276ul){ isDebug=true;
//        cout<<s->to_string_state()<<endl;
//    }

   // cout<<"\t[H] S: "<<oldState.to_string_state()<<"\t[hash] "<<entry_index<<"\t";
   // cout<<s->to_string_state()<<endl;
    for (const auto &item_action : *this->hashActionMap)
    {
        // apply action state and let the envirmont to roll and check the reward/pos
        Point actionCur = *item_action.second;

        double val;
        bool isWall = this->apply_action_SEQ(&oldState,my_policy->id_agent,actionCur,this->my_policy->max_speed);
        //bool isWall = oldState.applyAction(my_policy->id_agent,actionCur,this->my_policy->max_speed);
        //cout<<item_action.first<<endl;
        int step = to_closet_path_H(oldState);

        //bool isWall = this->apply_action(oldState,my_policy->id_agent,*actionCur,my_policy->max_speed);
        //if(isWall)
        //    step=-1;
        if (isWall)
        {
            val = (this->R.WallReward)*std::pow(R.discountF,s->get_budget(this->my_policy->get_id_name()));
//            val = this->R.WallReward*R.discountF*this->_stochasticMovement+
//                    zero_move_reward*(1-this->_stochasticMovement);
        }
        else{
            val=(this->R.CollReward)*std::pow(R.discountF,step);
        }
        if (isDebug)
            cout<<"  ["<<item_action.first<<"]="<<" step: "<<step<<" [s]->"<<oldState.to_string_state()<<" val="<<val<<endl;
        oldState.assignment(s,this->my_policy->id_agent);
        // insert to Q table
        this->set_value_matrix(entry_index,*item_action.second,val);
    }
   //cout<<endl;
}

//double RTDP_util::rec_h(State *s,int index, double acc_probablity)
//{
//    double res_h=0;
//    if (index+1==this->my_policy->tran.size()) {
//        auto fin = compute_h(s)*acc_probablity;
////        cout<<fin<<endl;
//        return fin;
//    }
//    auto old_state = State(*s);
//    index++;
//    auto res = my_policy->tran[index]->TransitionAction(s);
//    // waring need to del the res (Pointer)
//    for (int i = 0; i < res->size(); ++i)
//    {
//        auto pos = this->hashActionMap->find(res->operator[](i));
//        auto action = pos->second;
//        this->apply_action(s,my_policy->tran[index]->id_agent
//                ,*action,my_policy->tran[index]->max_speed);
//        res_h+=this->rec_h(s,index,acc_probablity*res->operator[](++i));
//        s->assignment(old_state,my_policy->tran[index]->GetId());
//    }
//    return res_h;
//}

void RTDP_util::add_entry_map_state(keyItem key,const State *s) {
    #ifdef DD
    this->debugDict[key]=s->to_mini_string();
    #endif

    #ifdef VECTOR
    auto v  = this->qTable->try_emplace(key,27,R.CollReward);
    #endif
#ifdef HEURISTOC
    this->heuristic(s,key);
#endif HEURISTOC
    ctr_state++;

}

RTDP_util::~RTDP_util() {
    cout<<"[inconsistent] : "<<inconsistent<<endl;
    cout<<"state genrated:\t"<<this->qTable->size()<<endl;
    std::for_each(hashActionMap->begin(),hashActionMap->end(),[](auto &item)
    {delete item.second;});
    delete(hashActionMap);


}

void RTDP_util::arg_max(const arr &arr,vector<int>& listIdxs){

    double max = -1;
    max = *std::max_element(arr.begin(), arr.end());
    listIdxs.reserve(1);
    for (int i = 0; i < arr.size(); ++i) {
        if (arr.operator[](i)==max)
            listIdxs.emplace_back(i);
    }
}



int RTDP_util::get_state_argmax(const State *s) {
    keyItem key = getStateKeyValue(s);
    this->last_entry = key;
    auto &row = this->get_Q_entry_values(s, key);

//    cout << "[state] " << s->to_string_state() << endl;
//    for (int i = 0; i < row.size(); ++i)
//        cout << "[" << i << "]=" << row[i];
//    cout << endl;


#ifdef PRINT
    cout<<endl;
    for(int i=0;i<row.size();++i) cout<<" ["<<i<<"]="<<row[i];
    cout<<endl;
#endif
    vector<int> argMax_list;
    arg_max(row, argMax_list);
    std::shuffle(argMax_list.begin(),argMax_list.end(),this->my_policy->generator);
    return argMax_list.front();
//      return std::distance(row.begin(),std::max_element(row.begin(), row.end()));



    //return argMax_list.front();

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
    this->steo_takken+=s->get_budget(this->my_policy->get_id_name());
    int index_action = this->get_state_argmax(s);

    auto pos = this->hashActionMap->find(index_action);
    if (pos == this->hashActionMap->end())
        throw std::invalid_argument( "function::get_argmx_action Error" );
    // assert
    return *pos->second;
}



bool RTDP_util::apply_action_SEQ(State *s,State::agentEnum id,Point &action,int max_speed)
{

    //int seq = s->get_budget(this->my_policy->id_agent);
    //bool is_wall_agent=false;
    return s->applyAction( id, action, max_speed,s->get_budget(this->my_policy->id_agent));
}



void RTDP_util::plusplus(){this->ctr_debug++;}

void RTDP_util::policyData() {
    //return;
    string pathFile=this->home+"/car_model/debug/";
    cout<<"[write] Qtable"<<endl;
    try{
        string nameFileCsv="Q.csv";
        int size_action = this->hashActionMap->size();
        csvfile csv(pathFile+nameFileCsv,";"); // throws exceptions!
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
        string nameFileCsv="map.csv";
        csvfile csv(pathFile+nameFileCsv,";"); // throws exceptions!
        for(auto &item:debugDict)
        {
            csv<<item.first;
            for(int i=0;i<item.second.size();i++) csv<<item.second[i];
            csv<<endrow;
        }
        //debugDict.clear();
    }
    catch (const std::exception &ex){std::cout << "Exception was thrown: " << ex.what() << std::endl;}

#ifdef LAST_STATE_DEBUG
    try{
        string nameFileCsv="Last_States.csv";
        csvfile csv(pathFile+nameFileCsv,";"); // throws exceptions!
        for(auto &item:state_policy_dict)
        {
            csv<<item.first;
            for(int i=0;i<12;i++) csv<<item.second.first[i];
            csv<<item.second.second;
            csv<<endrow;
        }
        state_policy_dict.clear();
    }
    catch (const std::exception &ex){std::cout << "Exception was thrown: " << ex.what() << std::endl;}
#endif
}

void RTDP_util::update_final_State(State *s, cell val) {
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


int RTDP_util::to_closet_path_H(const State &s)
{
   // return distance_H(s);
#ifdef H_ZERO
    return 0;
#endif
    steo_takken=s.get_budget(this->my_policy->cashID);
    //cout<<"\n[Td] "<<steo_takken<<" [Ta] "<<s.get_budget(this->my_policy->cashID)<<"\t[s] "<<s.to_string_state()<<endl;
    //assert(steo_takken==s.get_budget(this->my_policy->cashID));
    const auto& pos_def = s.get_position_ref(this->my_policy->id_agent);
    auto seq_action_jump =s.get_budget(this->my_policy->get_id_name());
    auto res = to_closet_path_H_calc(pos_def,seq_action_jump);
    return res;
}

int RTDP_util::to_closet_path_H_calc(const Point& agnet_pos,int jumps)
{
    int min_step=1000;
    int attacker_step=0;
    int max_step_attacker =-1;
    for(const auto& path : this->l_p_H)
    {
        if(steo_takken+jumps+1>=path.size()){
            continue;
        }
        attacker_step=0;
        //
        auto end = path.begin()+steo_takken+jumps+2;
        //end = path.end();
       // int ctr=0;
       auto s_start=path.begin()+steo_takken+jumps;
       for(auto iter = s_start;iter!=end;iter++)
       {
            if (auto dif = Point::distance_min_step(agnet_pos, *iter);dif < min_step) {
                min_step = dif;
            }
            //ctr+=2;
       }

    }
    return int(min_step/3);

}

int RTDP_util::distance_H(const State &s) const {
    return 10000;
}
