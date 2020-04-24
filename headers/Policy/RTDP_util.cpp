//
// Created by ise on 15.12.2019.
//


#include "RTDP_util.hpp"

RTDP_util::RTDP_util(int grid_size,vector<pair<int,int>>& max_speed_and_budget,string &mHome):home(mHome) {
    this->hashActionMap=Point::getDictAction();
    this->set_up_Q(grid_size,max_speed_and_budget);
    this->mapState= new map<string,unsigned int>();
    size_mapAction = hashActionMap->size();
}


void RTDP_util::set_up_Q(int grid_size, vector<pair<int,int>>& max_speed_and_budget) {
    int size_action = this->hashActionMap->size();
    int size_player=max_speed_and_budget.size();
    double state_number_overall = 1;
    for(auto &item : max_speed_and_budget){
        auto max_speed = item.first;
        if (max_speed==0)
            state_number_overall *=item.second;
        else
            state_number_overall *= pow(max_speed*2+1,int(Point::D))*(grid_size+1)*item.second;
    }

    printf("\nstate_number_overall:\t%lf\n",state_number_overall);
    this->size_Q=int(state_number_overall*0.9);

    if (size_Q>22000000)
        size_Q=23000000;
    cout<<"\nsize_Q= "<<size_Q<<endl;
    this->qTable = new double*[int(size_Q)]; // dynamic array (size 10) of pointers to int
    for (int i = 0; i < size_Q; ++i)
        this->qTable[i] = new double[size_action];
    cout<<qTable<<endl;
    cout<<"end allocation"<<endl;
}


int RTDP_util::get_state_index_by_string(State *s_state) {
    string s = s_state->to_string_state();
    auto it = this->mapState->find(s);
    if (it != this->mapState->end()){
        return it->second;
    }else{
        return this->add_entry_map_state(s,s_state);
    }
    return 0;
}
void RTDP_util::heuristic(State *s,int entry_index)
{
    vector<State*> vec_q;
    auto oldState = new State(*s);
    //cout<<s->to_string_state()<<endl;

    for (const auto &item_action : *this->hashActionMap)
    {
        // apply action state and let the envirmont to roll and check the reward/pos
        Point *actionCur = item_action.second;
        double val;
        bool isWall = this->apply_action(oldState,my_policy->id_agent,*actionCur,my_policy->max_speed);
        if (isWall)
            val = this->wallReward*discountFactor;
        else{
            //val = this->compute_h(oldState);
            val=this->collReward*discountFactor;
//            if (s->takeOff == false)
//            {
//                val=10;
//            }

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

unsigned int RTDP_util::add_entry_map_state(string &basicString,State *s) {
    // compute heuristic
    this->heuristic(s,this->ctr_state);

    // add to state_map
    this->mapState->insert({basicString,ctr_state});
    //this->ctr_state++;
    if (ctr_state+1>=this->size_Q)
        ctr_state=0;
    return ctr_state++;

}

RTDP_util::~RTDP_util() {
    this->policyData();
    cout<<"state genrated:\t"<<ctr_state<<endl;
    cout<<"size_Q:\t"<<size_Q<<endl;
    cout<<qTable<<endl;
    //Free each sub-array
    for(int i = 0; i < this->size_Q; ++i) {
       // cout<<"i="<<std::to_string(i)<<endl;
        double* currentIntPtr = qTable[i];
        delete(currentIntPtr);
    }
    //Free the array of pointers
    delete[] qTable;
    for(auto & it : *this->hashActionMap) {
        delete(it.second);
    }
    delete(mapState); // bug when try free also
    delete(hashActionMap);

}

vector<int> arg_max(const double arr[],int size ){
    double max = -1;
    max = *std::max_element(arr, arr+size);
    vector<int> l;
    for (int i = 0; i < size; ++i) {
        if (arr[i]==max)
            l.push_back(i);
    }
    return l;
}

int RTDP_util::get_state_argmax(State *s) {
    int argMax;
    int entry_state = this->get_state_index_by_string(s);
    auto row = this->qTable[entry_state];
    vector<int> argMax_list = arg_max(row,this->hashActionMap->size());
    int size = argMax_list.size();
    if (size>1)
    {
        argMax = argMax_list[ctr_random%size];
        ctr_random = ++ctr_random%this->hashActionMap->size();
    } else
        argMax = argMax_list[0];
    this->last_entry = entry_state;
    return argMax;
}

vector<float>* RTDP_util::get_probabilty(State *s) {
    int argMax;
    int entry_state = this->get_state_index_by_string(s);
    auto row = this->qTable[entry_state];
    vector<int> argMax_list = arg_max(row,this->size_mapAction);
    auto* l = new vector<float>();
    int prob = argMax_list.size();
    for (auto item: argMax_list){
        l->push_back(item);
        l->push_back(float(prob)/1);
    }
    return l;
}


double RTDP_util::get_value_state_max(State *s_state) {
    int entry_state = this->get_state_index_by_string(s_state);
    auto arr = this->qTable[entry_state];
   // double max = getMaxValueArrTmp(arr, size_mapAction);
    double max = *std::max_element(arr, arr + size_mapAction);;
    return max;
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

double RTDP_util::compute_h(State *s) {
//    cout<<s->to_string_state()<<endl;
    char team = this->my_policy->id_agent[1];
    auto my_pos = s->get_position(this->my_policy->id_agent);
    vector<Point> vec_pos;
    s->getAllPosOpponent(vec_pos,team);
    double min = s->g_grid->getSizeIntGrid();
    double posA = -1;
    for (int i = 0; i < vec_pos.size(); ++i) {
        auto res = getMaxDistance(vec_pos[i],my_pos);
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
    auto res = this->collReward*pow(discountFactor,min);
    //debug
    //cout<<"h(<"<<s->to_string_state()<<")="<<res<<endl;
    return res;
}

void RTDP_util::policyData() {
    //return;
    string pathFile=this->home+"/car_model/debug/";

    // csv map state-----------------------------
    try{
        string nameFileCsv="StateMap.csv";
        csvfile csv(std::move(pathFile+nameFileCsv),","); // throws exceptions!
        csv << "ID" << "Entry" << endrow;
        for(auto &item: *this->mapState)
        {
            csv<<item.first<<item.second<<endrow;
        }
    }
    catch (const std::exception &ex){std::cout << "Exception was thrown: " << ex.what() << std::endl;}

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
        for (int i = 0; i < this->ctr_state; ++i) {
            csv<<i;
            for (int j = 0; j <size_action; ++j){
//                cout<<"["<<i<<", "<<j<<"]="<<this->qTable[i][j]<<endl;
                csv<<this->qTable[i][j];
            }
            csv<<endrow;
        }

    }
    catch (const std::exception &ex){std::cout << "Exception was thrown: " << ex.what() << std::endl;}





}

