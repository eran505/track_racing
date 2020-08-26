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

typedef unordered_map<u_int64_t,double> matrixP;


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
    static std::vector<T> self_agg(const std::vector<T>& a,double p)
    {
        std::vector<T> result;

        result.reserve(a.size());

        std::transform(a.begin(), a.end(),
                       std::back_inserter(result), [&](const auto& x2){return p*x2;});

        return result;
    }

    template<typename F>
    static std::unique_ptr<unordered_map<u_int64_t,arr>> merge_Q_tables(
            matrixP& p_matrix,
            const unordered_map<u_int64_t,arr>* left ,double l,
            const unordered_map<u_int64_t,arr>* right,double r,
            F &agg_fun)
    {
        if(left == nullptr or right==nullptr)
            return nullptr;

        auto res = std::make_unique<unordered_map<u_int64_t,arr>>();

        if(l != 1.0){
            std::for_each(left->begin(),left->end(),[&](const pair<u_int64_t,arr> &item){p_matrix[item.first]+=l;
                //res->try_emplace(item.first,self_agg(item.second,l));
                assert(p_matrix[item.first]<=1.0);
            });}
        

        std::for_each(right->begin(),right->end(),[&](const pair<u_int64_t,arr> &item){
            res->try_emplace(item.first,self_agg(item.second,r));
            p_matrix[item.first]+=r;
            assert(p_matrix[item.first]<=1.0);
        });
        std::for_each(left->begin(),left->end(),[&](const pair<u_int64_t,arr> &item){
            if(auto pos = res->find(item.first); pos==res->end())
                res->try_emplace(item.first,self_agg(item.second,l));
            else
                res->operator[](item.first)=agg(self_agg(item.second,l),pos->second,agg_fun);
        });
        cout<<"L: "<<left->size()<<"+ R: "<<right->size()<<" --> res: "<<res->size()<<endl;
        return res;
    }
    static auto vector_merge_containerFix_left(QtableItem* left,QtableItem* right,double pLeft,double pRight,std::vector<matrixP>& m)
    {
        assert(left->size()==right->size());
        vector<qTbale_dict> res(right->size());
        //auto fun = [&](double x,double y)->double {return std::max(x,y);};
        auto fun = [&](double x,double y)->double {return x+y;}; //x=0.25
        for(int i=0;i<left->size();++i)
        {
            left->operator[](i).q = merge_Q_tables(m[i],left->operator[](i).q.get(),pLeft
                    ,right->operator[](i).q.get(),pRight,fun);

        }
        return true;
    }
    static auto agg_Q_tables(const vector<double>& pVec,const std::vector<shared_ptr<QtableItem>>& QVec)
    {
        assert(pVec.size()==QVec.size());
        std::vector<matrixP> vec_matrix_p(QVec.front()->size());
        size_t last=pVec.size()-1;
        double acc=pVec[last];
        for(int i=0;i<QVec.size()-1;++i)
        {
            vector_merge_containerFix_left(QVec[last].get(),QVec[i].get(),acc,pVec[i],vec_matrix_p);
            acc=1;
        }
        simplify_p_Q(vec_matrix_p,QVec[last].get());
        cout<<"[DONE]"<<endl;
    }
    static void simplify_p_Q(const std::vector<matrixP>& l_p,QtableItem* ptr_Q)
    {
        for (int i = 0; i < l_p.size(); ++i) {
            std::for_each(l_p[i].begin(),l_p[i].end(),[&](const pair<u_int64_t,double> &item){
                auto pos = ptr_Q->operator[](i).q->find(item.first);
                assert(pos!=ptr_Q->operator[](i).q->end());
                std::for_each(pos->second.begin(),pos->second.end(),[&](auto num){
                    cout<<num<<"->"<<item.second<<"->";
                    num=num*(1/item.second);
                    cout<<num<<"\n";
                    assert(num<=1.0);
                });
            });
        }
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

    }
    void learn_all_path_at_once(){train_on_all_path();}
    void one_path_at_a_time()
    {
        std::reverse(all_paths->begin(),all_paths->end());
        vector<double> pVec;
        int ctr=0;
        std::for_each(all_paths->begin(),all_paths->end(),
                      [&](const pair<double,vector<StatePoint>>& item){
            cout<<"P: "<<item.first<<endl;
            train_single_path(item.first,item.second,ctr);ctr++;
                          pVec.push_back(item.first);

        });

        containerFixAggregator::agg_Q_tables(pVec,list_Q);

        eval_all_paths();
        cout<<endl;

    }
    void train_on_all_path()
    {
        cout<<"[eval policy]"<<endl;
        //_defender->evalPolicy();
        add_H(_attacker.get(),_defender.get());
        SimulationGame sim = SimulationGame(config,std::move(_attacker),
                                            std::move(_defender),_start_state.get());

        sim.main_loop();
        sim.get_agnet_D()->getPolicy()->policy_data();

        //_defender->trainPolicy();
    }
private:
    void train_single_path(const double path_probability, const std::vector<StatePoint>& path_states,int ctr)
    {
        //config.levelz=2;
        Policy *ptr = new PathFinder(_attacker->get_max_speed()
                ,_attacker->get_id(),config.home,path_probability,path_states);
        auto naive_attacker=std::make_unique<Agent>(_attacker->getAllPositions_copy(),adversary,1);
        naive_attacker->setPolicy(ptr);

        _defender->getPolicyInt()->clear_tran();
        _defender->getPolicyInt()->add_tran(ptr);

        get_policy_defender()->init_expder(config.levelz);
        apply_new_dico_q(ctr);

        add_H(naive_attacker.get(),_defender.get());

        SimulationGame sim = SimulationGame(config,std::move(naive_attacker),
                                            std::move(_defender),_start_state.get());

        sim.main_loop();

        sim.get_agnet_D()->getPolicy()->policy_data();
        _defender=std::move(sim.get_agnet_D());
        // return the last q table
        get_policy_defender()->returnAllQ();
        cout<<"endd"<<endl;
    }
    void eval_all_paths()
    {
        cout<<"[eval policy]"<<endl;
        _defender->evalPolicy();
        _defender->getPolicyInt()->clear_tran();
        _defender->getPolicyInt()->add_tran(_attacker->getPolicyInt());
        get_policy_defender()->init_tran();

        get_policy_defender()->get_first_Q();

        add_H(_attacker.get(),_defender.get());

        SimulationGame sim = SimulationGame(config,std::move(_attacker),
                                            std::move(_defender),_start_state.get());
        sim.main_loop();
        sim.get_agnet_D()->getPolicy()->policy_data();
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

    void add_H(Agent* a , Agent* d)
    {
        auto rtdp = dynamic_cast<RtdpAlgo*>(d->getPolicyInt());
        auto pathfinder = dynamic_cast<const PathFinder*>(a->getPolicy());
        vector<vector<Point>> l = pathfinder->get_point_path_H(this->_start_state.get());
        rtdp->getUtilRTDP()->l_p_H=l;

    }


};


#endif //TRACK_RACING_SINGLEPATH_HPP
