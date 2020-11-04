//
// Created by eranhe on 09/08/2020.
//

#ifndef TRACK_RACING_SINGLEPATH_HPP
#define TRACK_RACING_SINGLEPATH_HPP

#include "Policy/Attacker/PathFinder.hpp"
#include "Policy/Attacker/StaticPolicy.hpp"
#include "Simulator.hpp"
#define ASSERT
#define DEBUG_DATA
typedef vector<pair<cell,vector<StatePoint>>> vector_p_path;
typedef unordered_map<keyItem ,arr> Qtable_;
typedef std::unique_ptr<Agent> unique_agnet;
typedef std::vector<containerFix> QtableItem;
typedef unordered_map<u_int64_t ,std::array<int,14>> map_dict;
typedef unordered_map<int64_t ,vector<cell>> q_Table;
typedef unordered_map<u_int64_t,cell> matrixP;
typedef pair<cell,vector<StatePoint>> Apath;
typedef vector<vector<StatePoint>> attacker_pathz;

class MatchingPath{

    Point world_size;
    u_int16_t upper_thershold;
    u_int16_t differ_upper=3;
public:
    explicit MatchingPath(const Point &grid_size,uint16_t threshold=300):world_size(grid_size),upper_thershold(threshold ){}

    std::vector<std::vector<u_int16_t>>  squarespace(std::vector<std::vector<Point>> attackerP,Point &&cube) {
        std::vector<std::vector<u_int16_t>> similarity_array;
        std::vector<u_int16_t> similarity_array_AVG;
        similarity_array.reserve(1);
        Point new_resolution = world_size / cube;
        differ_upper=attackerP.front().size()/3;
        //do
        std::for_each(attackerP[0].begin(), attackerP[0].end(), [&](Point &p) { p /= new_resolution; });
        similarity_array.emplace_back().push_back(0);
        bool is_in = false;
        for (int i = 1; i < attackerP.size(); ++i) {
            std::for_each(attackerP[i].begin(), attackerP[i].end(), [&](Point &p) { p /= new_resolution; });
            is_in = false;
            std::vector<u_int16_t> distance;
            for (auto &item : similarity_array) {
                u_int16_t min = 1000;
                for (auto pathID: item) {
                    auto d = matching_trajectories_differ(attackerP[pathID], attackerP[i],this->upper_thershold);
                    if (min > d) min = d;
                }
                distance.push_back(min);

            }
            if (auto min_elm = std::min_element(distance.begin(),distance.end()); *min_elm > differ_upper)
                similarity_array.emplace_back().push_back(i);
            else
                similarity_array[std::distance(distance.begin(),min_elm)].push_back(i);

        }

        for (auto &item_:similarity_array)
            cout << item_ << endl;

        return similarity_array;
    }
    void set_upper_threshold(u_int16_t t){this->upper_thershold=t;}

private:
    static u_int16_t matching_trajectories_differ(const std::vector<Point> &t2, const std::vector<Point> &t1,u_int16_t upper)
    {
        u_int16_t number_differ=0;
        auto max = t2;
        auto min =t1;
        if(t1.size()>t2.size())
        {
            max = t1;
            min =t2;
        }
        for(int k=0;k<min.size();++k)
        {
            bool is_match=false;
            for(int j=0;j<max.size();++j)
            {
                if(Point::distance_min_step(min[k],max[k])<upper) {
                    is_match = true;
                    break;
                }
            }
            if(is_match)
                continue;
            number_differ++;
        }
        return number_differ;
    }

};




class heuristicContainer{
    std::vector<std::vector<Point>> lPaths;
    map_dict map_state;
    Rewards R=Rewards::getRewards();
    const Grid* G;
    const short maxSpeed=1;
    std::unique_ptr<unordered_map<unsigned int,Point>>  dicoAction = Point::getDictActionUniqie();
public:
    template<typename D >
    explicit heuristicContainer(std::vector<std::vector<Point>>&& ptrPathsW, D &&map,const Grid* g):
            map_state(std::forward<D>(map)),G(g)
    {
        lPaths=ptrPathsW;
    }
    map_dict&& get_map_dict(){cout<<"remove dict states"<<endl;std::move(map_state);}
    std::vector<cell> get_heuristic_path(uint64_t ky_state) const
    {
        bool debug_print=false;
//        if(ky_state==1253741093158468514ul)
//            debug_print=true;
        auto posD = get_D_point(ky_state);
        auto vec =  fill_vector_H_value(posD.first,posD.second,debug_print);
        return vec;
    }
private:
    vector<cell> fill_vector_H_value(pair<Point,Point>& pos,pair<short,short> info_state,bool debug_print)const {
        std::vector<cell> v(27,R.CollReward);
        #ifndef HEURISTOC
        return v;
        #endif
   //     cout<<"D"<<pos.first.to_str()<<"_"<<pos.second.to_str()<<"_j="<<info_state.second<<"_t="<<info_state.first<<endl;
        for (const auto &p: *dicoAction)
        {
            auto newPos = apply_action_sq(pos.first,pos.second,p.second,info_state.second,maxSpeed);

            auto steps = to_closet_path_H_calc( newPos,info_state.first+info_state.second);
            if(debug_print)
                cout<<p.first<<"] steps: "<<steps<<" [s]-"<<newPos.to_str()<<"  "<<info_state.first<<":"<<info_state.second<<endl;
            if (G->is_wall(newPos)) {
                //cout<<"D"<<newPos.to_str()<<"_"<<endl;
                v[p.first] = (this->R.WallReward)*std::pow(R.discountF,steps);

                continue;
            }
            v[p.first] = this->R.CollReward * std::pow(R.discountF, steps);
        }
        return v;
    }
    inline static Point append_action(const pair<Point,Point> &posD ,const Point& a)
    {
        return (posD.second+a)+posD.first;
    }
    pair<pair<Point,Point>,pair<short,short>> get_D_point(const int64_t ky) const
    {
        auto& arr = this->map_state.find(ky)->second;
       // for(short i : arr) cout<<i<<" ";
       // cout<<endl;
        return {{Point(arr[6],arr[7],arr[8]),Point(arr[9],arr[10],arr[11])},{arr[12],arr[13]}};
    }
    int to_closet_path_H_calc(const Point& agnet_pos,int start_point)const
    {
#ifdef H_ZERO
        return 0;
#endif
        int min_step = 1000;
        for(const auto& path : this->lPaths){
            if(start_point+1>=path.size()){
                continue;
            }
            auto end = path.begin()+start_point+2;
//            auto end = path.end();
            //int ctr=0;
            for(auto iter = path.begin()+1+start_point;iter!=path.begin()+2+start_point;iter++)
            {
                if (auto dif = Point::distance_min_step(agnet_pos, *iter);dif < min_step) {
                    min_step = dif;
                }
                //ctr+=2;
            }
        }
        return min_step/3;
    }

    static int distance_H(const Point& Ap , const Point& Dp) {
        auto d = Point::distance_min_step(Ap,Dp);
        return d/3;
    }

    static Point apply_action_sq(const Point& pos ,const Point &speed,const Point &action,int jumps,int max_speed)
    {

        Point speed_tmp = speed;
        Point pos_tmp = pos;
        for (int k=0;k<jumps and k < 2 ;++k)
        {
            speed_tmp+=action;
            speed_tmp.change_speed_max(max_speed);
            pos_tmp+=speed_tmp;

        }
        if(jumps-2>0)
        {
            speed_tmp*=(jumps-2);
            pos_tmp+=speed_tmp;
            //speed.change_speed_max(max_speed);
        }
        return pos_tmp;
    }
    void print_dict_state()
    {
        for (const auto& item :map_state) cout<<item.first<<endl;
    }
};

class containerFixAggregator{
public:
    template <typename T>
    static std::vector<T> agg(const std::vector<T>& a, const std::vector<T>& b)
    {
        assert(a.size() == b.size());

        std::vector<T> result;

        result.reserve(a.size());

        std::transform(a.begin(), a.end(), b.begin(),
                       std::back_inserter(result), [&](const auto& x1,const auto& x2){return x1+x2;});

        return result;
    }

    template <typename T>
    static std::vector<T> self_agg(const std::vector<T>& a,double p)
    {
        std::vector<T> result;

        result.reserve(a.size());

        std::transform(a.begin(), a.end(),
                       std::back_inserter(result), [&](const auto& x2){return p*x2;});

        return result;
    }

    static void func2(Qtable_* big,const std::vector<std::unique_ptr<Qtable_>>& QVec,
                      const vector<cell>& pVec,const heuristicContainer& h_con)
    {
        for(size_t j=0;j<pVec.size();++j)
        {
            //cout<<"j:"<<j<<endl;
            for(const auto& item: *QVec[j])
            {

                if(big->find(item.first)!=big->end())
                    continue; // if the key is already in the table

                func4(big, item.first, QVec, pVec, h_con);
            }

        }
    }

    static auto agg_Q_tables(const vector<cell>& pVec,const std::vector<std::unique_ptr<Qtable_>>& QVec,const heuristicContainer& h_con)
    {
        assert(pVec.size()==QVec.size());
        u_int size_of_abstraction = QVec.front()->size();
        std::unique_ptr<Qtable_> big = std::make_unique<Qtable_>();
        func2(big.get(),QVec,pVec,h_con);
        cout<<"[ DONE ]"<<endl;
        return big;
    }
    template<typename T>
    static auto func3(const vector<T>& vec_i , const vector<T>& vec_big, double p){
        const auto _vecP = self_agg(vec_i,p);
        return agg(vec_big,_vecP);
    }
    static void func4(unordered_map<u_int64_t,vector<cell>>* big, u_int64_t keyState,const std::vector<std::unique_ptr<Qtable_>>& QVec,
                      const vector<cell>& pVec,const heuristicContainer& h_con){

        int occur =0;
        auto posBig = big->insert({keyState,vector<cell>(27)}).first;
        vector<cell> h_value = h_con.get_heuristic_path(keyState);


        for(size_t k=0;k<QVec.size();++k)
        {

            if(auto pos = QVec[k]->find(keyState);pos==QVec[k]->end())
            {
                if(h_value.empty())
                    h_value = h_con.get_heuristic_path(keyState);
                posBig->second = func3(h_value,posBig->second,pVec[k]);

            }
            else{
                const auto& vec_i = pos->second;
                posBig->second = func3(vec_i,posBig->second,pVec[k]);
                occur++;

            }
        }
        auto b = assert_func(posBig->second,h_value,keyState,occur);

    }
    static bool assert_func(const vector<cell>& v,const vector<cell>& H_v,u_int64_t key,int occur)
    {
        double ep=0.000000001;
        for (int i = 0; i < v.size(); ++i) {
            if(v[i]<0 and H_v[i]<0) continue;
            if(int(v[i])-int(H_v[i])>ep)
            {
                cout<<key<<"<-key "<<i<<" mix:"<<v[i]<<" h:"<<H_v[i]<<" diff: "<<v[i]-H_v[i]<<" occur "<<occur<<endl;
                //return true;
                //assert(false);
                //return true;
            }

        }
        return false;
    }
};

typedef RtdpAlgo PolicyD;
typedef StaticPolicy PolicyA;
class SinglePath{

    std::unique_ptr<Agent> _attacker;
    std::unique_ptr<Agent> _defender;
    std::unique_ptr<State> _start_state;
    std::vector<std::vector<StatePoint>> all_paths ;
    std::vector<double> prob_all_paths ;
    std::unique_ptr<Agent> _parital_attacker = nullptr;
    configGame config;
    std::vector<std::unique_ptr<Qtable_>> list_Q;

public:
    SinglePath(configGame &conf, State *s,unique_agnet A,unique_agnet D):
            _attacker(std::move(A)),
            _defender(std::move(D)),
            _start_state(std::make_unique<State>(*s)),
            config(conf)
    {}
    void learn_all_path_at_once()
    {
        get_all_paths();
        auto mp = MatchingPath(this->config.sizeGrid,config.eval_mode);
        auto ids_vector = mp.squarespace(this->get_policy_attcker()->list_only_pos(),Point(10,10,1));
        auto l_list = sort_pathz_by_ids(ids_vector);
        if(l_list.size()==1){
            train_on_all_path();
            return;
        }
        list_Q=std::vector<std::unique_ptr<Qtable_>>(ids_vector.size());
        vector<cell> pVec;
        int ctr=0;
        std::for_each(l_list.begin(),l_list.end(),
                      [&](const pair<vector<u_int16_t>, double>& item){

                          train_single_path(get_relevant_pathz(item.first),get_relevant_prob(item.first,item.second),ctr);ctr++;
                          pVec.push_back(item.second);
                      });
        cout<<"[S]: "<<get_policy_defender()->getUtilRTDP()->get_dict_map().size()<<endl;
        heuristicContainer heurist_con(get_policy_attcker()->list_only_pos(),get_policy_defender()->getUtilRTDP()->get_dict_map(),this->_start_state->g_grid);
        std::unique_ptr<Qtable_> pytr = containerFixAggregator::agg_Q_tables(pVec,this->list_Q,heurist_con);
        this->set_all_Q_tavble(std::move(pytr));

        //get_policy_defender()->getUtilRTDP()->inset_move_dict_map(heurist_con.get_map_dict());
        cout<<"[][][][][][]"<<endl;
        eval_all_paths();

        cout<<"[S]: "<<get_policy_defender()->getUtilRTDP()->get_dict_map().size()<<endl;
        cout<<endl;
    }
    void train_on_all_path()
    {

        add_H(_attacker.get(),_defender.get());
        SimulationGame sim = SimulationGame(config,std::move(_attacker),
                                            std::move(_defender),_start_state.get());

        sim.main_loop();

        #ifdef DEBUG_DATA
        sim.get_agents_data_policy();
        #endif
        //_defender->trainPolicy();
    }
private:

    void train_single_path(std::vector<std::vector<StatePoint>>&& path_states,std::vector<double>&& prob,int ctr)
    {
        //config.levelz=2;

        Policy *ptr = new StaticPolicy(path_states,prob,config.sizeGrid,_attacker->get_max_speed(),_attacker->get_id(),config.home,config._seed);//->,,,config.home,);
        auto naive_attacker=std::make_unique<Agent>(_attacker->getAllPositions_copy(),_attacker->get_name_id(),adversary,1);
        naive_attacker->setPolicy(ptr);

        _defender->getPolicyInt()->clear_tran();
        _defender->getPolicyInt()->add_tran(ptr);

        get_policy_defender()->init_expder(config.levelz);

        add_H(naive_attacker.get(),_defender.get());

        SimulationGame sim = SimulationGame(config,std::move(naive_attacker),
                                            std::move(_defender),_start_state.get());

        sim.main_loop();
        //sim.get_agnet_D()->getPolicy()->policy_data();
        _defender=std::move(sim.get_agnet_D());
        apply_new_dico_q(ctr);
        // return the last q table
        get_policy_defender()->returnAllQ();
        cout<<"endd"<<endl;
    }
    void eval_all_paths()
    {
        cout<<"[eval policy]"<<endl;
        //_defender->evalPolicy();
        _defender->getPolicyInt()->clear_tran();
        _defender->getPolicyInt()->add_tran(_attacker->getPolicyInt());
        get_policy_defender()->init_tran();

        get_policy_defender()->get_first_Q();

        add_H(_attacker.get(),_defender.get());

        SimulationGame sim = SimulationGame(config,std::move(_attacker),
                                            std::move(_defender),_start_state.get());
        sim.main_loop();
        #ifdef DEBUG_DATA
        sim.get_agents_data_policy();
        #endif

        _defender=std::move(sim.get_agnet_D());
        _defender->trainPolicy();

        cout<<endl;
    }
    void get_all_paths()
    {

        all_paths = get_policy_attcker()->get_copy_pathz();
        prob_all_paths = get_policy_attcker()->get_copy_probabilities();

    }

    auto get_policy_attcker() -> const PolicyA*
    {
        return dynamic_cast<const PolicyA*>(_attacker->getPolicy());
    }
    auto get_policy_defender() -> PolicyD*
    {
        return dynamic_cast<PolicyD*>(_defender->getPolicyInt());
    }
    void set_probability_df(double p)
    {
        get_policy_defender()->set_discounted_factor(p);
    }
    void apply_new_dico_q(int j)
    {
        //list_Q[j]=get_policy_defender()->get_evalouater()->get_q_table();
        list_Q[j]=std::move(get_policy_defender()->getUtilRTDP()->get_q_table());
    }
    void set_all_Q_tavble(std::unique_ptr<Qtable_>&& ptr)
    {
        get_policy_defender()->getUtilRTDP()->set_q_table(std::move(ptr));
    }
    static void add_H(Agent* a , Agent* d)
    {
        auto rtdp = dynamic_cast<PolicyD*>(d->getPolicyInt());
        auto pathfinder = dynamic_cast<const PolicyA*>(a->getPolicy());
        vector<vector<Point>> l = pathfinder->list_only_pos();
        rtdp->getUtilRTDP()->l_p_H=l;

    }
    auto map_path_by_goal() -> std::unordered_map<u_int64_t, pair<vector<u_int16_t>, double>> {
        std::unordered_map<u_int64_t, pair<vector<u_int16_t>, double>> map_goal;
        for (auto k = 0; k < all_paths.size(); ++k)
        {
            u_int64_t key = all_paths[k].back().pos.expHash();
            if (auto pos = map_goal.find(key); pos == map_goal.end()) {
                auto iter = map_goal.try_emplace(key);
                iter.first->second.first.push_back(k);
                iter.first->second.second = prob_all_paths.operator[](k);
            } else {
                pos->second.first.push_back(k);
                pos->second.second += prob_all_paths[k];
            }
        }
//        for(auto &it:map_goal){
//            cout<<it.first<<": "<<it.second.first<<" : "<<it.second.second<<endl;
//        }
        return map_goal;
    }
    std::vector<pair<vector<u_int16_t>, double>> sort_pathz_by_ids(std::vector<std::vector<u_int16_t>>& idz)
    {
        std::vector<pair<vector<u_int16_t>, double>> l;
        l.reserve(idz.size());
        double acc = 0;
        for(auto& list_idz:idz)
        {
            acc=0;
            for( auto _id : list_idz)
            {
                acc+=this->prob_all_paths[_id];
            }
            l.emplace_back(std::move(list_idz),acc);
        }
        return l;
    }

    template<typename K>
    std::vector<double> get_relevant_prob(std::vector<K> l,double sum)
    {
        std::vector<double> ans;
        ans.reserve((l.size()));
        for (auto item:l)
        {
            double p = prob_all_paths[item];
            ans.push_back(p/sum);
        }
        return ans;
    }
    template<typename K>
    std::vector<vector<StatePoint>> get_relevant_pathz(std::vector<K> l)
    {
        std::vector<vector<StatePoint>> ans;
        ans.reserve((l.size()));
        for (auto item:l)
        {
            ans.push_back(this->all_paths[item]);
        }
        return ans;
    }
};



#endif //TRACK_RACING_SINGLEPATH_HPP
