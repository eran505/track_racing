//
// Created by ERANHER on 5.10.2020.
//

#ifndef TRACK_RACING_STATICPOLICY_HPP
#define TRACK_RACING_STATICPOLICY_HPP

#include <utility>

#include "State.hpp"
#include "headers/Policy/Update_RTDP/PathMapper.hpp"
typedef std::vector<StatePoint> APath;
typedef std::vector<pair<std::vector<Point>,double>> seq_goals;
typedef std::vector<weightedPosition> seq_starting;
class StaticPolicy: public Policy{

    PathGenartor gen;
    std::unique_ptr<PathMapper<u_int32_t>>  mapper= nullptr;
public:
    StaticPolicy(const Point &gridSize,uint maxSpeed,State::agentEnum id,u_int16_t num_of_paths,std::string home_path,const seq_goals& goals_points,const seq_starting& starting_points,int seed=4)
    :Policy(maxSpeed,id,home_path,seed),gen(seed,gridSize,maxSpeed)
    {
        auto [list_pathz,probablity_list] = gen.geneate_path_loopV2(goals_points,starting_points,num_of_paths);
        make_mapper(list_pathz,probablity_list);
    }
    StaticPolicy(std::vector<APath> lPathz,std::vector<double> lProbabilites, const Point &gridSize,uint maxSpeed,State::agentEnum id,std::string home_path,int seed=4)
            :Policy(maxSpeed,id,home_path,seed),gen(seed,gridSize,maxSpeed)
    {
        make_mapper(std::move(lPathz),std::move(lProbabilites));
    }


    ~StaticPolicy() override = default;
    void reset_policy() override
    {
        mapper->random_choose_path(getRandom());
    }
    void policy_data()const override{

        string pathFile=this->home+"/car_model/debug/"+this->prefix_file_name+"_p.csv";

        //print Q table--------------------------------
        try{
            string nameFileCsv="Q.csv";
            int size_action = this->hashActionMap->size();
            csvfile csv(std::move(pathFile),";"); // throws exceptions!
            auto p_list = mapper->get_all_probabilites_ref();
            auto pathz_list = mapper->get_all_pathz_ref();
            for (int i = 0; i < p_list.size(); ++i) {
                csv<<"P:"+std::to_string(p_list[i]);
                for(const auto &item:pathz_list[i])
                    csv<<item;
                csv<<endrow;
            }
        }
        catch (const std::exception &ex){std::cout << "Exception was thrown: " << ex.what() << std::endl;}

    }
    Point get_action(State *s) override
    {
        assert(false);

    }
    void make_action(State *s,int jumps)override{
        auto StatePoint = this->mapper->get_next_actual_state(jumps);
        s->set_speed(this->get_id_name(),std::move(StatePoint.speed));
        s->set_position(this->get_id_name(),std::move(StatePoint.pos));

    }
    const vector<double>* TransitionAction(const State *s)const override{
        assert(false);
        return nullptr;
    }
    vector<pair<StatePoint,double>> weighted_next_partial_state(const State &s,uint jumps) override{
        //cout<<"s: "<<s.to_string_state()<<"]  ";
        return mapper->get_next_states(get_hash_state(s),jumps);
    }
    std::vector<std::vector<StatePoint>> get_copy_pathz()const
    {
        return mapper->get_all_pathz();
    }
    std::vector<double> get_copy_probabilities()const
    {
        return mapper->get_all_probabilites();
    }
    std::vector<std::vector<Point>> list_only_pos()const
    {
        return mapper->get_all_pos();
    }
private:
    void make_mapper(std::vector<APath> all_pathz, std::vector<double> porbablites_arr)
    {
        mapper=std::make_unique<PathMapper<u_int32_t>>(std::move(all_pathz),std::move(porbablites_arr));
    }
    u_int64_t get_hash_state(const State& s)const {
        //cout<<s->to_string_state()<<endl;
        auto hSpeed = s.get_speed(this->id_agent).hashConst(Point::maxSpeed);
        auto hPos = s.get_position_ref(this->id_agent).hashConst();
        u_int64_t EntryIndx = Point::hashNnN(hPos,hSpeed);
        return EntryIndx;
    }
};

#endif //TRACK_RACING_STATICPOLICY_HPP
