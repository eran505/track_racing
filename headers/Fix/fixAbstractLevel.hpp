//
// Created by eranhe on 12/07/2020.
//

#ifndef TRACK_RACING_FIXABSTRACTLEVEL_HPP
#define TRACK_RACING_FIXABSTRACTLEVEL_HPP
#include "util_game.hpp"
#include "State.hpp"
#include "containerFix.hpp"
#include "Grid_transition.h"
typedef std::vector<double> arr;
typedef std::unique_ptr<unordered_map<u_int64_t,arr>> qTbale_dict;

class fixAbstractLevel{
    u_int32_t ctr_insertion=0;
    Point _tolerance;
    Point _mini_gird_size;
    Point _upper_window=Point(0);
    Point _window;
    Point _orignal_size;
    Point _offset=Point(0);
    Point _grid_size;
    Point _window_mini_gird_size;
    GridTransition tran_grid;
    unordered_map<u_int32_t,containerFix> block_info;
    bool highest_level=false;
    u_int32_t block_max_size=-1; //TODO: init the bolck max size
    u_int32_t current_entry=0;
    u_int32_t _key=0;
    std::unique_ptr<Grid> _gird = std::make_unique<Grid>();

    std::unique_ptr<unordered_map<u_int64_t,arr>> _talbeQ=std::make_unique<unordered_map<u_int64_t,arr>>();
    std::function<u_int32_t (const State *s)> state_to_entry= nullptr;

public:
    template<typename P=Point>
    void set_window(P &&point){_window=std::forward<P>(point);}
    Point get_window(){return _window;}
    containerFix & get_relevent_abstraction(const Point& D, const Point &A) {
        return get_containerFix(_key);
    }
    const Point& get_offset(){return _offset;}
    void set_max_block(u_int32_t n){this->block_max_size=n;}
    void set_gird_in_Grid_transition(Grid *g){tran_grid.set_grid_all_goalz(g);}
    fixAbstractLevel()= default;
    const Point& get_window_tol()const{return _window;}
    const Point& get_Point_abstract()const{return _mini_gird_size;}
    fixAbstractLevel(const Point &mini_grid_szie,const Point& window,const Point& orignal_size,const Point &gird_size,bool first)
            :_mini_gird_size(mini_grid_szie),
             _window(window),
             _orignal_size(orignal_size),
             _grid_size(_orignal_size/mini_grid_szie),
             _tolerance(gird_size)
    {

        tran_grid =  GridTransition(mini_grid_szie,gird_size,orignal_size,window);
        if(first)
            state_to_entry=[&](const State *s){ return 0u;};
        else {
            state_to_entry=[&](const State *s){ return this->state_to_entry_impl(s);};;
        }

        _window_mini_gird_size = _window*_mini_gird_size;
        cout<<"_orignal_size"<<_orignal_size.to_str()<<endl;
        cout<<"_grid_size"<<_grid_size.to_str()<<endl;
        cout<<"window"<<_window.to_str()<<endl;
        cout<<"_mini_gird_size"<<_mini_gird_size.to_str()<<endl;
        cout<<"tol="<<gird_size.to_str()<<endl;
        cout<<"===="<<endl;

    }
    u_int32_t get_ctr_insert() const{return ctr_insertion;}
    void window_generator(const Point& up_window_size){
        _upper_window=up_window_size;
        tran_grid.preprocessing();
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
    containerFix& insert_containerFix(u_int32_t num_id){
        ctr_insertion++;
        auto [obj,bool_flag] = this->block_info.try_emplace(num_id,containerFix());
        auto [l,u] = tran_grid.cur_bound;
        cout<<"[G] low:"<<l.to_str()<<", up:"<<u.to_str()<<endl;
        obj->second.G->set_upperBound(this->_tolerance);
        obj->second.upper=u;
        obj->second.lower=l;
        tran_grid.inset_goalz(obj->second.G.get(),u,l,this->_mini_gird_size);
        return obj->second;

    }
    bool is_collison(const Point &a,const Point &d)
    {
        Point abs_a = a/this->_mini_gird_size;
        Point abs_d = d/this->_mini_gird_size;
        Point dif = (abs_d - abs_a).AbsPoint();
        return equle_or_less(dif,_window);
    }
    bool is_collision_by_distance(const Point &a,const Point &d)
    {
        Point dif = (a - d).AbsPoint();
        if(dif<_window_mini_gird_size)
            return true;
        return false;
    }
    void change_entry_mini_grid(const Point &position_D,const Point &position_A)
    {
        auto res_KEY = tran_grid.get_info_max(position_A,position_D);
        this->_key=res_KEY;


    }
    Grid* get_grid_and_change_entry(const Point &position_D,const Point &position_A,const Point& offset)
    {
        change_entry_mini_grid(position_D,position_A);
        auto Gptr = get_grid();
        _offset=block_info[_key].lower;
        cout<<" key:"<<_key<<" ";
        return Gptr;
    }
    Grid* get_grid()
    {
        return get_containerFix(_key).G.get();

    }
    void transform_point_inplcae(Point &point)
    {
        return (point-=_offset)/=_mini_gird_size;
    }
    Point transform_point(const Point &point)
    {
        return (point-_offset)/_mini_gird_size;
    }
    auto& get_ref_containerFix()
    {
        if(auto pos = this->block_info.find(current_entry);pos==this->block_info.end())
            assert(false);
        return this->block_info.operator[](current_entry);
    }

    void return_dict(qTbale_dict&& q){get_containerFix(_key).q=std::move(q);}

    qTbale_dict&& get_dict(){return std::move(get_containerFix(_key).q);}

private:
    u_int32_t state_to_entry_impl(const State *s)
    {
        std::vector<Point> l;
        s->getAllPos(l,this->_mini_gird_size);

    }

    static u_int32_t Point_to_key(const Point &p,const Point &grid)
    {
        u_int32_t tmp= p[0]*grid[0]+p[1]%grid[1]+p[2]/(grid[0]*grid[1]);
        return tmp;
    }
    static Point key_to_point(int key,const Point &grid)
    {
        return Point(key/grid[0],key%grid[0],key/(grid[0]*grid[1]));
    }
    auto point_to_bounds(const Point &p)
    {
        Point lower = (p*_mini_gird_size);
        Point upper = lower+_mini_gird_size*_tolerance;
        return std::pair<Point,Point>{std::move(lower),std::move(upper)};
    }
    std::pair<Point,Point> key_to_bounds(u_int32_t key)
    {

        return point_to_bounds(key_to_point(key,_grid_size));

    }
    containerFix& get_containerFix(u_int32_t key_point)
    {
        if(auto pos = block_info.find(key_point);pos==block_info.end())
        {
            return insert_containerFix(key_point);
        } else return pos->second;
    }
    static bool equle_or_less(const Point &one, const Point &other) {
        for (auto i = 0; i < one.capacity; ++i) {
            if (one[i] >= other[i])
                return false;
        }
        return true;
    }
};



#endif //TRACK_RACING_FIXABSTRACTLEVEL_HPP
