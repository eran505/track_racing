////
//// Created by eranhe on 1/10/21.
////
//
//#ifndef TRACK_RACING_FEATUERZER_HPP
//#define TRACK_RACING_FEATUERZER_HPP
//
//#include "util_game.hpp"
////#include <torch/script.h>
//#include <utility>
//
//
//struct experienceTensor{
//    torch::Tensor _state;
//    torch::Tensor _next_state;
//    torch::Tensor _action;
//    torch::Tensor _reward;
//    torch::Tensor _done;
//public:
//    experienceTensor(torch::Tensor&& s,torch::Tensor&&  s_next,torch::Tensor&&  reward,torch::Tensor&&  done,torch::Tensor&&  a):
//            _state(std::move(s)),
//            _next_state(std::move(s_next)),
//            _action(std::move(a)),
//            _reward(std::move(reward)),
//            _done(std::move(done))
//
//    {}
//    void to_string()const
//    {
//        cout<<"_action: "<<_action<<endl;
//        cout<<"_reward: "<<_reward<<endl;
//        cout<<"_done: "<<_done<<endl;
//    }
//    experienceTensor()=default;
//};
//
//struct elementItem{
//
//    u_int64_t hash=0;
//    experienceTensor exp;
//
//public:
//
//    float_t td_error=0;
//
//    bool operator<(const struct elementItem& other) const{
//        return td_error < other.td_error;
//    }
//    elementItem()=default;
//    elementItem(torch::Tensor &&s ,u_int64_t s_id , torch::Tensor &&s_next , u_int64_t s_next_id ,torch::Tensor &&action,torch::Tensor &&reward,torch::Tensor &&done)
//    :hash(getHash(s_id,s_next_id, action.item().toLong()))
//    ,exp(std::move(s),
//         std::move(s_next),
//         std::move(reward),
//         std::move(done),
//         std::move(action)
//         )
//    {
//            if(exp._done.item().toInt()>0)
//                td_error=1;
//    }
//
//    [[nodiscard]] u_int64_t hashValue()const {return hash;}
//
//    friend ostream& operator<<(ostream& os,const elementItem &td)
//    {
//        return os<<"(h)"<<td.hash;
//    }
//
//    static u_int64_t getHash(const u_int64_t s ,const u_int64_t s_next_id ,u_int64_t action)
//    {
//        u_int64_t seed =action;
//        seed ^= size_t(s) + 0x9e3779b9 + (seed << 7u) + (seed >> 2u);
//        seed ^= size_t(s_next_id) + 0x9e3779b9 + (seed << 7u) + (seed >> 2u);
//        return seed;
//    }
//
//
//
//
//};
//
//
//
//class Featuerzer{
//    std::unique_ptr<unordered_map<unsigned int,Point>> action_dico = Point::getDictActionUniqie();
//    std::vector<int> _norm;
//    vector<float> grid_size;
//    std::vector<Point> goalz;
//    int N=13; // inital vector state_size
//    int input=22; // input size vector
//public:
//    int get_input_size() const{return input;}
//    Featuerzer(std::vector<int> norm,const Point& grid_size_point,std::vector<Point> l_goals):_norm(std::move(norm))
//    {
//        grid_size.push_back(grid_size_point[0]);
//        grid_size.push_back(grid_size_point[1]);
//        grid_size.push_back(grid_size_point[2]);
//        goalz=std::move(l_goals);
//    }
//
//    std::pair<torch::Tensor,u_int64_t> get_featers(std::vector<int> &&state)
//    {
//        auto h_id = get_hash(state,state.size());
//        auto options = torch::TensorOptions().dtype(torch::kFloat32);
//        torch::Tensor state_tensor = torch::zeros({input},options);
//
//        for(int i=0;i<N;i++)
//            state_tensor[i]=state[i]/float(_norm[i]);
//
//        int idx=N;
//        idx += inset_to_tensor(state_tensor,get_bound_D(state),idx); // 3
//        idx += inset_to_tensor(state_tensor,get_goal_D(state),idx); // 3 * G
//        idx += inset_to_tensor(state_tensor,get_diiff_A_D(state),idx); // 3
//
//        std::pair<torch::Tensor,u_int64_t> results(std::move(state_tensor),h_id);
//
//        return results;
//    }
//
//private:
//    static u_int64_t get_hash(const std::vector<int> &state_rep,size_t size)
//    {
//        u_int64_t id_h=state_rep.front();
//        std::for_each(state_rep.begin()+1,state_rep.begin()+size,[&](int i){
//            id_h += u_int64_t(i) + 0x9e3779b9 + (id_h << 7u) + (id_h >> 2u);
//        });
//        return id_h;
//    }
//    std::vector<float> get_diiff_A_D(const std::vector<int> &state)
//    {
//        return {float(state[0]-state[6])/this->grid_size[0],
//                float(state[1]-state[7])/grid_size[1],float(state[2]-state[8])/this->grid_size[2]};
//    }
//    std::vector<float> get_bound_D(const std::vector<int> &state)
//    {
//        return {(grid_size[0]-state[6])/grid_size[0],(grid_size[1]-state[7])/grid_size[1]
//                ,(grid_size[2]-state[8])/grid_size[2]};
//    }
//
//    std::vector<float> get_goal_D(const std::vector<int> &state)
//    {
//        vector<float> l(goalz.size()*3);
//        int ctr=0;
//        for(const auto& item_goal: goalz)
//        {
//            l[ctr++]= float(item_goal[0]-state[6])/(grid_size[0]);
//            l[ctr++]= float(item_goal[1]-state[7])/float(grid_size[1]);
//            l[ctr++]= float(item_goal[2]-state[8])/float(grid_size[2]);
//        }
//        return l;
//    }
//
//
//
//
//    static int inset_to_tensor(torch::Tensor& t,vector<float> vec,int idx_s)
//    {
//        for (int i = 0; i < vec.size(); ++i) {
//            t[idx_s+i]=vec[i];
//        }
//        return vec.size();
//    }
//
//public:
//    Point int_to_point_action(int i)
//    {
//        return action_dico->at(i);
//    }
//    Point int_to_point_action_2d(int i)
//    {
//        if (i==0)
//            return Point(0,0,0);
//        if(i==1)
//            return Point(0,1,0);
//        if(i==2)
//            return Point(1,0,0);
//        if(i==3)
//            return Point(-1,0,0);
//        if(i==4)
//            return Point(0,-1,0);
//    }
//
//};
//
//#endif //TRACK_RACING_FEATUERZER_HPP
