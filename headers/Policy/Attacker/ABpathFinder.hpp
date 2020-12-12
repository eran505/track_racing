//
// Created by ERANHER on 30.7.2020.
//

#ifndef TRACK_RACING_ABPATHFINDER_HPP
#define TRACK_RACING_ABPATHFINDER_HPP
#include "util_game.hpp"
#include "serach/Astar.hpp"
#include "util/Rand.hpp"
#include <cassert>
using AStar::StateSearch;



class Astar_util{

    AStar::Generator gen;
    int ctr=0;
public:
    Astar_util(u_int32_t maxSpeed,Point&& grid_size)
    :gen(maxSpeed,grid_size)
    {}

    vector<vector<AStar::StatePoint>> get_paht_a_b(AStar::StatePoint& source_,const AStar::StatePoint& target_){
        //cout<<"source:"<<source_<<" target:"<<target_<<endl;
        gen.findPath(source_,target_,false, true);
        //cout<<"done A*"<<endl;
        assert(!gen.get_deep_list_nodes_ref_const().empty());
        return gen.get_deep_list_nodes();
        //assert(!list_nodes.empty());
    }
    vector<AStar::StatePoint> get_path_a_b_Astar(AStar::StatePoint& source_,const AStar::StatePoint& target_,bool at_random=true){
        auto l = get_paht_a_b(source_,target_);
        int index=0;
        if(at_random)
            index = range_random(0,int(l.size()-1));
        assert(index<l.size());
        return l[index];
    }

};


class ABfinder{
    Randomizer randomizer_obj;
    Point GridSzie;
    double stho=0.7;
    u_int limt=4; //10
    u_int16_t MAX_SPEED=2;
    Point last_action;
    bool is_random=false;
    vector<AStar::StatePoint> seq_state;
    Astar_util Astar_util_object;
public:

    ABfinder(u_int64_t seed, const Point &gridSize, int max_speed):
            randomizer_obj(seed),
            GridSzie(gridSize),
            Astar_util_object(max_speed,Point(GridSzie))
    {}

    vector<AStar::StatePoint> get_pathz(const AStar::StatePoint &A,const AStar::StatePoint &B)
    {

        seq_state.clear();
        genarte_path(A,B);
        auto last_state = get_last_state();
        auto list_a_star_pathz = Astar_util_object.get_path_a_b_Astar(last_state,B);
        for(int j=int(list_a_star_pathz.size()-2);j>=0;--j)
            this->seq_state.emplace_back(list_a_star_pathz[j]);
        return  std::move(seq_state);
    }
private:
    const AStar::StatePoint& get_last_state()
    {
        return seq_state[seq_state.size()-1];
    }
    void genarte_path(const AStar::StatePoint &A,const AStar::StatePoint &B)
    {
        uint ctr=0;
        AStar::StatePoint cur = A;
        seq_state.emplace_back(cur);
        while(less_than_limit(cur,B))
        {

            bool bol=true;
            while(bol)
            {
                //cout<<"cur: {"<<cur.pos.to_str()<<"}, {"<<cur.speed.to_str()<<"}"<<"action="<<last_action.to_hash_str()<<endl;
                get_action_to_goal(cur,B);
                bol=!vaild_move(cur);
                assert(ctr++ < 10000);

            }
            seq_state.emplace_back(cur);

        }

    }
    void get_action_to_goal(AStar::StatePoint & cur,const AStar::StatePoint& Goal)
    {

        move_to_goal(cur,Goal);
        inset_noise();

    }
    [[nodiscard]] bool less_than_limit(const AStar::StatePoint& cur,const AStar::StatePoint& Goal)const
    {
        if(std::abs(Goal.pos[0]-cur.pos[0])<=limt and std::abs(Goal.pos[1]-cur.pos[1])<=limt)
            return false;
        return true;
    }
    bool inset_noise()
    {
        //cout<<"noise"<<endl;

        if(randomizer_obj.get_double()>stho)
        {
            for(int i=0;i<last_action.capacity;++i)
                last_action.array[i]=get_move_aixs_random(randomizer_obj.get_double());
            last_action.array[2]=0;
            return true;
        }
        return false;
    }
    void move_to_goal(AStar::StatePoint & cur,const AStar::StatePoint& Goal)
    {
        for(int k=0;k<last_action.capacity;++k)
        {
            last_action.array[k]=get_move_aixs(k,cur,Goal);
        }

    }
    static int get_action_in_limt(int i,const AStar::StatePoint &cur,const AStar::StatePoint& Goal)
    {

        if(cur.speed[i]>0)
            return -1;
        if(cur.speed[i]<0)
            return 1;
        else  return 0;
    }
    [[nodiscard]] int get_move_aixs(int i,const AStar::StatePoint &cur,const AStar::StatePoint& Goal)
    {
        //if(cur.pos[i]+limt > Goal.pos[i]) return -1;
        if((std::abs(cur.pos[i]- Goal.pos[i]) < limt && i<2) or (i==2 and cur.pos[i]==3))
        {
            return get_action_in_limt(i,cur,Goal);
        }
        if(stho<randomizer_obj.get_double()&& i<2)
            return get_move_aixs_random(randomizer_obj.get_double());
        if(i==2)
        {
            if(cur.pos[2]==this->GridSzie[2]-2 and  cur.speed[2]==0)
                return 0;
            if(cur.pos[2]==this->GridSzie[2]-2 and  cur.speed[2]>0)
                return -1;
            if(cur.pos[2]==1 and  cur.speed[2]==0)
                return 1;
            if(cur.speed[2]>=1)
                return 0;
            return 1;
        }
        auto m = Goal.pos[i] - (cur.pos[i]+cur.speed[i]);
        if (m>1)
            m=1;
        else if (m<-1)
            m=-1;
        return m;
    }
    void apply_action_StateSearch(const Point& action, AStar::StatePoint &cur) const
    {
        cur.speed+=action;
        cur.speed.change_speed_max(this->MAX_SPEED);
        cur.pos+=cur.speed;
    }
    bool vaild_move(AStar::StatePoint &cur)const
    {

        AStar::StatePoint cur_old(cur);
        apply_action_StateSearch(last_action,cur);
        if(is_out_bound(cur)){
            cur=cur_old;
            return false;
        }
        if(cur_old.pos==cur.pos)
            return false;
        if(cur.speed[0]==0 and cur.speed[1]==0 and cur.speed[2]==0)
        {
//            cur=cur_old;
//            return false;
        }
        return true;
    }
    [[nodiscard]] bool is_out_bound(const AStar::StatePoint &cur)const{
        if(cur.pos.any_ngative()) return true;
        if(!(this->GridSzie>cur.pos)) return true;
        if(!(this->GridSzie+1>cur.pos+cur.speed)) return true;
        if((cur.pos+cur.speed+1).any_ngative()) return true;
        if(cur.pos[2]+cur.speed[2]>=GridSzie[2]) return true;
        return false;
    }
    static int get_move_aixs_random(double d)
    {
        if(d<0.33)
            return -1;
        if(d<0.66)
            return 0;
        else
            return 1;
    }


};

#endif //TRACK_RACING_ABPATHFINDER_HPP
