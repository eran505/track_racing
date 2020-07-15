//
// Created by eranhe on 12/07/2020.
//

#ifndef TRACK_RACING_FIXABSTRACTLEVEL_HPP
#define TRACK_RACING_FIXABSTRACTLEVEL_HPP
#include "util_game.hpp"
#include "State.hpp"
#include "containerFix.hpp"

typedef std::vector<double> arr;
typedef std::unique_ptr<unordered_map<u_int64_t,arr>> qTbale_dict;

class fixAbstractLevel{
    Point _tolerance;
    Point _mini_gird_size;
    Point _window;
    Point _orignal_size;
    Point _grid_size;
    unordered_map<u_int32_t,std::vector<u_int32_t>> window_map;
    unordered_map<u_int32_t,std::vector<u_int32_t>> key_to_window;
    unordered_map<u_int32_t,containerFix> block_info;
    bool highest_level=false;
    u_int32_t block_max_size=-1; //TODO: init the bolck max size
    u_int32_t current_entry=0;


    std::unique_ptr<unordered_map<u_int64_t,arr>> _talbeQ=std::make_unique<unordered_map<u_int64_t,arr>>();
    std::function<u_int32_t (const State *s)> state_to_entry= nullptr;

public:
    const Point& get_offset(){return block_info[current_entry].offset;}
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
             _orignal_size(orignal_size),
             _grid_size(_orignal_size/mini_grid_szie)
    {

        if(first)
            state_to_entry=[&](const State *s){ return 0u;};
        else state_to_entry=[&](const State *s){ return this->state_to_entry_impl(s);};
        preprocessing_window_map();

        cout<<"_orignal_size"<<_orignal_size.to_str()<<endl;
        cout<<"_grid_size"<<_grid_size.to_str()<<endl;
        cout<<"window"<<_window.to_str()<<endl;
        cout<<"_mini_gird_size"<<_mini_gird_size.to_str()<<endl;
        cout<<"===="<<endl;

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
    Point get_grid(const Point &position_D,const Point &position_A) const
    {
        return key_to_point(get_intersection(Point_to_key(position_A,
                _grid_size),Point_to_key(position_D,_grid_size)),_grid_size);
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

    u_int32_t get_intersection(u_int32_t A, u_int32_t D) const
    {
        auto& vecA = this->key_to_window.at(A);
        auto& vecD = this->key_to_window.at(D);
        std::vector<u_int32_t > l;
        for(auto &item:vecA) {
            if (std::find(vecD.begin(), vecD.end(), item) != vecD.end()) l.push_back(item);
        }
        return *std::min_element(l.begin(),l.end());
    }
    void preprocessing_window_map()
    {
        // input: Point
        // output: map key: gridID -> all the window that within the id
        insert_keys_window_map();
        auto vec_action_window = get_all_option_match_window(_window);
        u_int32_t max_id = _grid_size.accMulti();
        for (auto i=0 ;i<max_id;++i )
        {

            Point cur_point(i/_grid_size[0],i%_grid_size[1],i/(_grid_size[1]*_grid_size[0]));
            for (const auto& item_action: vec_action_window)
            {
                auto p = item_action+cur_point;
                if(p.out_of_bound(_grid_size))
                    continue;
                if(auto pos = window_map.find(Point_to_key(p,_grid_size));pos!=window_map.end())
                {
                    pos->second.push_back(i);
                }

            }

        }
        key_to_window = map_transpose(window_map);
    }
    static vector<Point> get_all_option_match_window(const Point &window)
    {
        std::vector<Point> l;
        auto p = (window)*-1;
        for (int x=0;x>p[0];--x)
            for(int y=0;y>p[1];--y)
                for(int z=0;z>p[2];--z)
                    l.emplace_back(x,y,z);
        return l;
    }
    void insert_keys_window_map()
    {
        auto p = (_window)-1;
        for (int x=0;x<(_grid_size[0]-p[0]);++x)
            for(int y=0;y<(_grid_size[1]-p[1]);++y)
                for(int z=0;z<(_grid_size[2]-p[2]);++z)
                {
                    u_int32_t key = x*_grid_size[0]+y%_grid_size[1]+z/(_grid_size[0]*_grid_size[1]);
                    window_map.try_emplace(key);
                }
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
