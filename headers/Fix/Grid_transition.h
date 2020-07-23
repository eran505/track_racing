//
// Created by eranhe on 20/07/2020.
//

#ifndef TRACK_RACING_GRID_TRANSITION_H
#define TRACK_RACING_GRID_TRANSITION_H
#include "util_game.hpp"

typedef pair<u_int64_t,pair<Point,Point>> grid_info;
class GridTransition{

    Point _mini_grid_size;
    Point _window;
    std::unordered_map<u_int32_t ,std::vector<u_int32_t >> _mini_gird_belonging;
    std::unordered_map<u_int32_t ,std::vector<u_int32_t >> _mapping_id_to_grid;
    Point _real_size;
    Point _mini_grid_size_big;
    Point _grid_size;
    std::vector<pair<double,Point>> all_goalz;

public:
    pair<Point,Point> cur_bound;
    GridTransition()= default;

    GridTransition(const Point& abstrct_mode,
            const Point &grid_size,const Point& real_size,const Point& window):
    _mini_grid_size(abstrct_mode),
    _window(window),
    _real_size(real_size),
    _grid_size(grid_size)
    {
        _mini_grid_size_big=_real_size/_mini_grid_size;
        this->preprocessing();
        cur_bound={Point(0),_mini_grid_size*_grid_size};
    }


    u_int64_t get_info_max(const Point &A,const Point &D)
    {
        int keyId = get_max_intersection(A,D);
        auto bounds = point_to_bounds(key_to_point(keyId,_mini_grid_size_big));
        cur_bound=bounds;
        return keyId;
    }
    u_int64_t  get_info_min(const Point &A,const Point &D)
    {
        int keyId = get_min_intersection(A,D);
        auto bounds = point_to_bounds(key_to_point(keyId,_mini_grid_size_big));
        cur_bound=bounds;
        return keyId;
    }

    u_int64_t  get_info_dist(const Point &A,const Point &D)
    {
        int keyId = hash_point(A);
        auto bounds = point_to_bounds_dist(A,D);
        cur_bound=bounds;
        return keyId;
    }


    void preprocessing()
    {
        preprocessing_window_map();
    }
    void set_grid_all_goalz(Grid *g){all_goalz=g->getAllGoalsData();}
    void inset_goalz(Grid *g,const Point &upper,const Point &low,const Point &abs)
    {

        for(auto& item: all_goalz)
        {
            if(item.second.out_of_bound(low,upper))
                continue;
            Point absGoal = (item.second-low)/abs;
            g->insert_goal(absGoal,item.first);
        }
    }
private:
    void get_intersection(const Point& A, const Point& D,vector<u_int32_t> &l) const {
        auto &vecA = this->_mini_gird_belonging.at(point_transform(A));
        auto &vecD = this->_mini_gird_belonging.at(point_transform(D));

        for (auto &item:vecA) {
            if (std::find(vecD.begin(), vecD.end(), item) != vecD.end()) l.push_back(item);
        }
    }
    void preprocessing_window_map()
    {
        // input: Point
        // output: map key: gridID -> all the window that within the id

        insert_keys_window_map(_mapping_id_to_grid);
        _mini_gird_belonging = map_transpose(_mapping_id_to_grid);
        cout<<"";
    }
    void insert_keys_window_map(std::unordered_map<u_int32_t,std::vector<u_int32_t>>& map)
    {
        Point scale_size = _real_size/_mini_grid_size;
        for(int x=0 ; x<scale_size[0];++x)
            for(int y=0 ; y<scale_size[1];++y)
                for(int z=0 ; z<scale_size[2];++z)
                {
                    int acc=0;
                    auto p = Point(x,y,z);
                    for(u_int16_t i=0;i<3;i++)
                        if(p[i]+_grid_size[i]-1 >= scale_size[i])
                            acc+=1;
                    if(acc>0)
                        continue;
                    u_int32_t key = point_to_key_id(p,scale_size);
                    auto vec_point = get_window_size_id(p,_grid_size);
                    auto vec_id = chagne_point_to_key(vec_point,scale_size);
                    map.try_emplace(key,vec_id);
                }
    }
    static u_int32_t point_to_key_id(const Point& p, const Point &grid)
    {
        return p[0]*grid[0]+p[1]%grid[1]+p[2]/(grid[0]*grid[1]);

    }
    static Point key_to_point(int key,const Point &grid)
    {
        return Point(key/grid[0],key%grid[0],key/(grid[0]*grid[1]));
    }
    static std::vector<Point> get_window_size_id(const Point &p,const Point& gird)
    {
        std::vector<Point> l;
        for(int x=0 ; x<gird[0];++x)
            for(int y=0 ; y<gird[1];++y)
                for(int z=0 ; z<gird[2];++z)
                {
                    l.emplace_back(p[0]+x,p[1]+y,p[2]+z);
                }
        return l;
    }
    static std::vector<u_int32_t> chagne_point_to_key(std::vector<Point> &l,const Point &gird)
    {
        std::vector<u_int32_t> vec(l.size());
        for(size_t i=0;i<vec.size();++i)
            vec[i]=point_to_key_id(l[i],gird);
        return vec;
    }

    u_int32_t point_transform(const Point &p)const
    {
        return  point_to_key_id(p/_mini_grid_size,this->_real_size/_mini_grid_size);

    }

    static u_int64_t hash_point(const Point &p){
        return p.expHash();
    }

    template<typename P>
    std::pair<Point,Point> point_to_bounds(P &&p)
    {
        Point lower = (p*_mini_grid_size);
        Point upper = lower+_mini_grid_size*this->_grid_size;
        return std::pair<Point,Point>{std::move(lower),std::move(upper)};
    }

    std::pair<Point,Point> point_to_bounds_dist(const Point& D,const Point& A)
    {
        Point upper = A+_mini_grid_size*_grid_size;
        if(D.out_of_bound(A,upper))
            assert(false);
        return {A,upper};
    }

    int get_max_intersection(const Point& A, const Point& D)
    {
        std::vector<u_int32_t > l;
        get_intersection(A,D,l);
        if(l.empty())
            return -1;
        cout<<"intersection {";
        std::for_each(l.begin(),l.end(),[](auto x){cout<<","<<x;});
        cout<<"}";
        cout<<endl;
        return *std::max_element(l.begin(),l.end());
    }
    int get_min_intersection(const Point& A, const Point& D)
    {
        std::vector<u_int32_t > l;
        get_intersection(A,D,l);
        if(l.empty())
            return -1;
        return *std::min_element(l.begin(),l.end());
    }

};


#endif //TRACK_RACING_GRID_TRANSITION_H
