//
// Created by eranhe on 12/07/2020.
//

#ifndef TRACK_RACING_FIXABSTRACTLEVEL_HPP
#define TRACK_RACING_FIXABSTRACTLEVEL_HPP
#include "util_game.hpp"
#include "State.hpp"
#include "containerFix.hpp"

class fixAbstractLevel{
    Point _tolerance;
    Point _mini_gird_size;
    Point _window;
    Point _grid_size;
    Point _orignal_size;

    unordered_map<u_int32_t,containerFix> block_info;
    bool highest_level=false;
    u_int32_t block_max_size=-1; //TODO: init the bolck max size
    u_int32_t current_entry=-1;

    typedef std::vector<double> arr;
    typedef std::unique_ptr<unordered_map<u_int64_t,arr>> qTbale_dict;
    std::unique_ptr<unordered_map<u_int64_t,arr>> _talbeQ=std::make_unique<unordered_map<u_int64_t,arr>>();
    std::function<u_int32_t (const State *s)> state_to_entry= nullptr;

public:
    void set_max_block(u_int32_t n){this->block_max_size=n;}
    fixAbstractLevel()= default;
    const Point& get_tolerance()const{return _window;}
    const Point& get_Point_abstract()const{return _mini_gird_size;}
    explicit fixAbstractLevel(const Point &mini_gird_size)
    :_mini_gird_size(mini_gird_size)
    ,block_info()
    {}
    fixAbstractLevel(const Point &mini_grid_szie,const Point& window)
    :_mini_gird_size(mini_grid_szie),
    _window(window)
    {}
    fixAbstractLevel(const Point &mini_grid_szie,const Point& window,const Point& orignal_size,bool first)
            :_mini_gird_size(mini_grid_szie),
             _window(window),
             _orignal_size(orignal_size)
    {

        if(first)
            state_to_entry=[&](const State *s){ return 0u;};
        else state_to_entry=[&](const State *s){ return this->state_to_entry_impl(s);};

    }
    Point get_abstract_point(const Point& original_position)const{
        return original_position/_mini_gird_size;
    }
    u_int32_t get_entry_mini_grid(const Point &p)const
    {
        return this->_mini_gird_size[0]*p[0]+p[1]+(this->_mini_gird_size[1]*
        this->_mini_gird_size[0])*p[2];
    }
    void state_transformition(const State *s,State *trans)
    {
        s->getAbstractionState_inplace(_mini_gird_size,trans);
    }
    void inset_containerFix(u_int32_t num_id){
        auto [obj,bool_flag] = this->block_info.try_emplace(num_id,containerFix());

        obj->second.G= nullptr;
    }
    qTbale_dict&& get_dict(const State *s)
    {
        return std::move(this->_talbeQ);
    }

    auto& get_ref_containerFix()
    {
        if(auto pos = this->block_info.find(current_entry);pos==this->block_info.end())
            assert(false);
        return this->block_info.operator[](current_entry);
    }
    void return_dict(qTbale_dict&& q){this->_talbeQ=std::move(q);}
    qTbale_dict&& get_dict(){return std::move(this->_talbeQ);}

private:
    u_int32_t state_to_entry_impl(const State *s)
    {
        std::vector<Point> l;
        s->getAllPos(l,this->_mini_gird_size);

    }

//    pair<Point,Point> mini_gird_ID_to_bounds(u_int32_t grid_id)
//    {
//        u_int32_t x = (grid_id%(divPoint[0]*divPoint[1]))/divPoint[0];
//        u_int32_t  y = grid_id%divPoint[0];
//        u_int32_t  z = grid_id/(divPoint[1]*divPoint[0]);
//
//        return Point(x,y,z);
//    }
};



#endif //TRACK_RACING_FIXABSTRACTLEVEL_HPP
