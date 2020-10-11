//
// Created by eranhe on 09/08/2020.
//

#ifndef TRACK_RACING_SINGLEPATH_HPP
#define TRACK_RACING_SINGLEPATH_HPP

//#include <pstl/execution_defs.h>
#include "Policy/Attacker/PathFinder.hpp"
#include "Policy/Attacker/StaticPolicy.hpp"
#include "Simulator.hpp"
#define ASSERT
#define DEBUG_DATA
typedef vector<pair<double,vector<StatePoint>>> vector_p_path;
typedef unordered_map<keyItem ,arr> Qtable_;
typedef std::unique_ptr<Agent> unique_agnet;
typedef std::vector<containerFix> QtableItem;
typedef unordered_map<u_int64_t ,std::array<int,12>> map_dict;
typedef unordered_map<int64_t ,vector<double>> q_Table;
typedef unordered_map<u_int64_t,double> matrixP;
typedef pair<double,vector<StatePoint>> Apath;
typedef vector<vector<StatePoint>> attacker_pathz;
class heuristicContainer{
    std::vector<std::vector<Point>> lPaths;
    map_dict map_state;
    Rewards R=Rewards::getRewards();
    const Grid* G;
public:
    explicit heuristicContainer(const std::vector<std::vector<Point>>&& ptrPathsW,map_dict& map,const Grid* g):
            map_state(std::move(map)),G(g)
    {
        lPaths=ptrPathsW;
    }
    std::vector<double> get_heuristic_path(const int index,uint64_t ky_state) const
    {
        auto posD = get_D_point(ky_state);
        return fill_vector_H_value(posD,index);
    }
private:
    vector<double> fill_vector_H_value(const pair<Point,Point>& pos,int index_Qi)const {
        auto dicoAction = Point::getDictActionUniqie();
        std::vector<double> v(27);
        for (const auto &p: *dicoAction)
        {
            auto newPos = append_action(pos, p.second);
            if (G->is_wall(newPos)) {
                v[p.first] = R.WallReward;
                continue;
            }
            auto steps = to_closet_path_H_calc(index_Qi, newPos);
            v[p.first] = this->R.CollReward * std::pow(R.discountF, steps);
        }
        return v;
    }
    inline static Point append_action(const pair<Point,Point> &posD ,const Point& a)
    {
        return (posD.second+a)+posD.first;
    }
    pair<Point,Point> get_D_point(const int64_t ky) const
    {
        auto& arr = this->map_state.find(ky)->second;
        return {Point(arr[6],arr[7],arr[8]),Point(arr[9],arr[10],arr[11])};
    }
    int to_closet_path_H_calc(const u_int index,const Point& agnet_pos)const
    {
        return 0;
        int min_step = 100000;
        double min_dist = 100000;

        for(const auto& p : this->lPaths[index]){
            if(Point::distance(agnet_pos,p)<min_dist){
                min_dist=Point::distance(agnet_pos,p);
                min_step=Point::distance_min_step(agnet_pos,p);
            }
        }
#ifdef ASSERT
        assert(min_step<100000);
#endif
        return min_step;
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
                      const vector<double>& pVec,const heuristicContainer& h_con)
    {
        for(size_t j=0;j<pVec.size();++j)
        {

            for(const auto& item: *QVec[j])
            {

                if(big->find(item.first)!=big->end())
                    continue; // if the key is already in the table

                func4(big, item.first, QVec, pVec, h_con);
            }
        }
    }

    static auto agg_Q_tables(const vector<double>& pVec,const std::vector<std::unique_ptr<Qtable_>>& QVec,const heuristicContainer& h_con)
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
    static void func4(unordered_map<u_int64_t,vector<double>>* big, u_int64_t keyState,const std::vector<std::unique_ptr<Qtable_>>& QVec,
                      const vector<double>& pVec,const heuristicContainer& h_con){

        auto posBig = big->insert({keyState,vector<double>(27)}).first;

        for(size_t k=0;k<QVec.size();++k)
        {
            if(auto pos = QVec[k]->find(keyState);pos==QVec[k]->end())
            {
                const auto vec_i = h_con.get_heuristic_path(k,keyState);
                posBig->second = func3(vec_i,posBig->second,pVec[k]);

            }
            else{
                const auto& vec_i = pos->second;
                posBig->second = func3(vec_i,posBig->second,pVec[k]);

            }
        }
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
    void learn_by_goals()
    {
        get_all_paths();
        cout<<"learn_by_goals"<<endl;
        auto map_goals = map_path_by_goal();
        list_Q=std::vector<std::unique_ptr<Qtable_>>(map_goals.size());
        vector<double> pVec;
        int ctr=0;
        std::for_each(map_goals.begin(),map_goals.end(),
                      [&](const pair<u_int64_t, pair<vector<u_int16_t>, double>>& item){

                          train_single_path(get_relevant_pathz(item.second.first),get_relevant_prob(item.second.first,item.second.second),ctr);ctr++;
                          pVec.push_back(item.second.second);
                      });

        cout<<"[S]: "<<get_policy_defender()->getUtilRTDP()->get_dict_map().size()<<endl;
        heuristicContainer heurist_con(get_policy_attcker()->list_only_pos(),get_policy_defender()->getUtilRTDP()->get_dict_map(),this->_start_state->g_grid);
        std::unique_ptr<Qtable_> pytr = containerFixAggregator::agg_Q_tables(pVec,this->list_Q,heurist_con);
        this->set_all_Q_tavble(std::move(pytr));

        eval_all_paths();
        cout<<"[S]: "<<get_policy_defender()->getUtilRTDP()->get_dict_map().size()<<endl;
        cout<<endl;
    }
    void learn_all_path_at_once(){train_on_all_path();}
    void one_path_at_a_time()
    {
        get_all_paths();
        cout<<"one_path_at_a_time"<<endl;
        list_Q=std::vector<std::unique_ptr<Qtable_>>(all_paths.size());
        vector<double> pVec=prob_all_paths;
        int ctr=0;
        for(int c=0;c<all_paths.size();++c)
        {
            std::vector<std::vector<StatePoint>> l;
            l.push_back(all_paths[c]);
            train_single_path(std::move(l),std::vector<double>(1,1),c);
        }

        cout<<"[S]: "<<get_policy_defender()->getUtilRTDP()->get_dict_map().size()<<endl;
        auto& d =get_policy_defender()->getUtilRTDP()->get_dict_map();
        heuristicContainer heurist_con(get_policy_attcker()->list_only_pos(),d,this->_start_state->g_grid);
        std::unique_ptr<Qtable_> pytr = containerFixAggregator::agg_Q_tables(pVec,list_Q,heurist_con);
        this->set_all_Q_tavble(std::move(pytr));

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
