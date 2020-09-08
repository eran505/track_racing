//
// Created by eranhe on 09/08/2020.
//

#ifndef TRACK_RACING_SINGLEPATH_HPP
#define TRACK_RACING_SINGLEPATH_HPP
#include "Policy/Attacker/PathFinder.hpp"
#include "Simulator.hpp"
#define ASSERT
typedef vector<pair<double,vector<StatePoint>>> vector_p_path;
typedef std::unique_ptr<Agent> unique_agnet;
typedef std::vector<containerFix> QtableItem;
typedef unordered_map<u_int64_t ,std::array<int,12>> map_dict;
typedef unordered_map<int64_t ,vector<double>>* q_Table;
typedef unordered_map<u_int64_t,double> matrixP;

class heuristicContainer{
    std::vector<std::vector<Point>> lPaths;
    map_dict map_state;
    Rewards R=Rewards::getRewards();
    const Grid* G;
public:
    explicit heuristicContainer(const vector_p_path* ptrPathsW,map_dict& map,const Grid* g):
            map_state(std::move(map)),G(g)
    {
        lPaths.reserve(ptrPathsW->size());
        std::for_each(ptrPathsW->begin(),ptrPathsW->end(),[&](const pair<double,vector<StatePoint>>& item){
            auto &cur_path_list = lPaths.emplace_back();
            std::transform(item.second.begin(),
                           item.second.end(),
                           std::back_inserter(cur_path_list),
                           [](const StatePoint& p){return p.pos;});
        });

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

    static void func2(QtableItem* big,const std::vector<shared_ptr<QtableItem>>& QVec,
                      const vector<double>& pVec,const heuristicContainer& h_con,u_int indexAbs)
    {
        big->operator[](indexAbs).upper=QVec.front()->operator[](indexAbs).upper;
        big->operator[](indexAbs).lower=QVec.front()->operator[](indexAbs).lower;
        big->operator[](indexAbs).step=QVec.front()->operator[](indexAbs).step;
        unordered_map<u_int64_t,vector<double>>* bigQPtr = big->operator[](indexAbs).q.get();
        for(size_t j=0;j<pVec.size();++j)
        {
            for(const auto& item: *QVec[j]->operator[](indexAbs).q)
            {
                if(big->operator[](indexAbs).q->find(item.first)!=big->operator[](indexAbs).q->end())
                    continue; // if the key is already in the table

                func4(bigQPtr, item.first, QVec, pVec, h_con, indexAbs);
            }
        }
    }

    static auto agg_Q_tables(const vector<double>& pVec,const std::vector<shared_ptr<QtableItem>>& QVec,const heuristicContainer& h_con)
    {
        assert(pVec.size()==QVec.size());
        u_int size_of_abstraction = QVec.front()->size();
        shared_ptr<std::vector<containerFix>> big = std::make_shared<std::vector<containerFix>>(size_of_abstraction);
        for(int i=0;i<size_of_abstraction;++i)
        {
            func2(big.get(),QVec,pVec,h_con,i);
        }
        cout<<"[ DONE ]"<<endl;
        return big;
    }
    template<typename T>
    static auto func3(const vector<T>& vec_i , const vector<T>& vec_big, double p){
        const auto _vecP = self_agg(vec_i,p);
        return agg(vec_big,_vecP);
    }
    static void func4(unordered_map<u_int64_t,vector<double>>* big, u_int64_t keyState,const std::vector<shared_ptr<QtableItem>>& QVec,
                      const vector<double>& pVec,const heuristicContainer& h_con,int abstQ){

        for(size_t k=0;k<QVec.size();++k)
        {
            auto posBig = big->find(keyState);
            if(posBig==big->end())
            {
                posBig = big->insert({keyState,vector<double>(27)}).first;
            }
            if(auto pos = QVec[k]->operator[](abstQ).q->find(keyState);pos==QVec[k]->operator[](abstQ).q->end())
            {
                const auto vec_i = h_con.get_heuristic_path(k,keyState);
                posBig->second = func3(vec_i,posBig->second,pVec[k]);//
            }
            else{
                const auto& vec_i = pos->second;
                posBig->second = func3(vec_i,posBig->second,pVec[k]);
            }
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

        cout<<"[S]: "<<get_policy_defender()->getUtilRTDP()->get_dict_map().size()<<endl;
        heuristicContainer heurist_con(all_paths.get(),get_policy_defender()->getUtilRTDP()->get_dict_map(),this->_start_state->g_grid);
        shared_ptr<std::vector<containerFix>> pytr = containerFixAggregator::agg_Q_tables(pVec,list_Q,heurist_con);
        this->set_all_Q_tavble(std::move(pytr));

        eval_all_paths();
        cout<<"[S]: "<<get_policy_defender()->getUtilRTDP()->get_dict_map().size()<<endl;
        cout<<endl;

    }
    void train_on_all_path()
    {
        cout<<"[eval policy]"<<endl;
       // _defender->evalPolicy();
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

        //sim.get_agnet_D()->getPolicy()->policy_data();
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
        //sim.get_agnet_D()->getPolicy()->policy_data();
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
    void set_all_Q_tavble(shared_ptr<std::vector<containerFix>>&& ptr)
    {
        get_policy_defender()->get_evalouater()->set_Q_table_all(std::move(ptr));
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
