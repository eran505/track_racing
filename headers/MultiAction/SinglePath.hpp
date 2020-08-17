//
// Created by eranhe on 09/08/2020.
//

#ifndef TRACK_RACING_SINGLEPATH_HPP
#define TRACK_RACING_SINGLEPATH_HPP
#include "Policy/Attacker/PathFinder.hpp"
#include "Simulator.hpp"
typedef vector<pair<double,vector<StatePoint>>> vector_p_path;
typedef std::unique_ptr<Agent> unique_agnet;
typedef std::vector<containerFix> QtableItem;



class containerFixAggregator{
public:

    template <typename T,typename F>
    static std::vector<T> agg(const std::vector<T>& a, const std::vector<T>& b,const F& func)
    {
        assert(a.size() == b.size());

        std::vector<T> result;

        result.reserve(a.size());

        std::transform(a.begin(), a.end(), b.begin(),
                       std::back_inserter(result), [&](const auto& x1,const auto& x2){return func(x1,x2);});

        return result;
    }
    template <typename T>
    static std::vector<T> self_agg(const std::vector<T>& a,const double p)
    {
        std::vector<T> result;

        result.reserve(a.size());

        std::transform(a.begin(), a.end(),
                       std::back_inserter(result), [&](const auto& x2){return p*x2;});

        return result;
    }

    template<typename F>
    static std::unique_ptr<unordered_map<u_int64_t,arr>> merge_Q_tables(
            const unordered_map<u_int64_t,arr>* left ,
            const unordered_map<u_int64_t,arr>* right,
            F &agg_fun)
    {
        if(left == nullptr or right==nullptr)
            return nullptr;

        auto res = std::make_unique<unordered_map<u_int64_t,arr>>();
        std::for_each(right->begin(),right->end(),[&](const pair<u_int64_t,arr> &item){
            res->try_emplace(item.first,self_agg(item.second,1));
        });
        std::for_each(left->begin(),left->end(),[&](const pair<u_int64_t,arr> &item){
            if(auto pos = right->find(item.first); pos==right->end())
                res->try_emplace(item.first,self_agg(item.second,1));
            else
                res->operator[](item.first)=agg(item.second,pos->second,agg_fun);
        });
        cout<<"L: "<<left->size()<<"+ R: "<<right->size()<<" --> res: "<<res->size()<<endl;
        return res;
    }

    static auto vector_merge_containerFix_to_left(QtableItem* left,const QtableItem* right)
    {
        assert(left->size()==right->size());
        vector<qTbale_dict> res(right->size());
        //auto fun = [&](double x,double y)->double {return std::min(x,y);};
        auto fun = [&](double x,double y)->double {return x*1+y*0;};
        for(int i=0;i<left->size();++i)
        {
            left->operator[](i).q = merge_Q_tables(left->operator[](i).q.get()
                    ,right->operator[](i).q.get(),fun);

        }
        return true;
    }
};


class SinglePath{

    std::unique_ptr<Agent> _attacker;
    std::unique_ptr<Agent> _defender;
    std::unique_ptr<State> _start_state;
    std::unique_ptr<vector_p_path> all_paths = nullptr;
    std::unique_ptr<Agent> _parital_attacker = nullptr;
    configGame config;
    std::vector<shared_ptr<QtableItem>> list_Q;
public:
    SinglePath(configGame &conf, State *s,unique_agnet A,unique_agnet D):
            _attacker(std::move(A)),
            _defender(std::move(D)),
            _start_state(std::make_unique<State>(*s)),
            config(conf)
    {
        get_all_paths();
        cout<<"Path Number: "<<all_paths->size()<<endl;

        list_Q=std::vector<std::shared_ptr<QtableItem>>(all_paths->size());
        //train_on_all_path();
        //exit(0);
    }

    void main_functopn_genrator()
    {
        //std::reverse(all_paths->begin(),all_paths->end());
        int ctr=0;
        std::for_each(all_paths->begin(),all_paths->end(),
                      [&](const pair<double,vector<StatePoint>>& item){
            train_single_path(item.first,item.second,ctr);ctr++;
        });
        //cout<<this->list_Q.back()->back().q->size()<<endl;

        containerFixAggregator::vector_merge_containerFix_to_left(list_Q[1].get(),list_Q[0].get());

        eval_all_paths();
        cout<<endl;

    }
    void train_on_all_path()
    {
        cout<<"[eval policy]"<<endl;
        //_defender->evalPolicy();
        SimulationGame sim = SimulationGame(config,std::move(_attacker),
                                            std::move(_defender),_start_state.get());

        sim.main_loop();

        //_defender->trainPolicy();
    }
private:
    void train_single_path(const double path_probability, const std::vector<StatePoint>& path_states,int ctr)
    {

        Policy *ptr = new PathFinder(_attacker->get_max_speed()
                ,_attacker->get_id(),config.home,path_probability,path_states);
        auto naive_attacker=std::make_unique<Agent>(_attacker->getAllPositions_copy(),adversary,1);
        naive_attacker->setPolicy(ptr);

        get_policy_defender()->init_expder(config.levelz);
        apply_new_dico_q(ctr);
        SimulationGame sim = SimulationGame(config,std::move(naive_attacker),
                                            std::move(_defender),_start_state.get());

        sim.main_loop();

        _defender=std::move(sim.get_agnet_D());
        cout<<"endd"<<endl;
    }
    void eval_all_paths()
    {
        cout<<"[eval policy]"<<endl;
        _defender->evalPolicy();
        SimulationGame sim = SimulationGame(config,std::move(_attacker),
                                            std::move(_defender),_start_state.get());
        sim.main_loop();
        _defender=std::move(sim.get_agnet_D());
        _defender->trainPolicy();
        cout<<endl;
    }
    void get_all_paths()
    {
        all_paths = std::make_unique<vector<pair<double,vector<StatePoint>>>>();
        const PathFinder *ptr = get_policy_attcker();
        ptr->treeTraversal(_start_state.get(),all_paths.get());

    }
    auto get_policy_attcker() -> const PathFinder*
    {
        return dynamic_cast<const PathFinder*>(_attacker->getPolicy());
    }
    auto get_policy_defender() -> RtdpAlgo*
    {
        return dynamic_cast<RtdpAlgo*>(_defender->getPolicyInt());
    }
    void set_probability_df(double p)
    {
        get_policy_defender()->set_discounted_factor(p);
    }
    void apply_new_dico_q(int j)
    {
        list_Q[j]= get_policy_defender()->get_evalouater()->get_q_table();
    }



};


#endif //TRACK_RACING_SINGLEPATH_HPP
