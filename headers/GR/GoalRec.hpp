//
// Created by eranhe on 4/19/21.
//

#ifndef TRACK_RACING_GOALREC_HPP
#define TRACK_RACING_GOALREC_HPP
#include "util_game.hpp"
#define MAX_SPEED_E 2
#define MAX_SPEED_P 1

struct NodeG{
    Point pos;
    NodeG* parent;
    vector<std::unique_ptr<NodeG>> child;
    vector<pair<Point,std::vector<u_int16_t>>> goal_list;
    vector<double> goal_likelihood;
    int min_step=std::numeric_limits<int>::max();;

    NodeG():pos(0),parent(nullptr),child(vector<std::unique_ptr<NodeG>>()),
    goal_list(vector<pair<Point,std::vector<u_int16_t>>>()){}
    explicit NodeG(const Point& p,const Point& g,u_int16_t id_path,double likelihood):pos(p),parent(nullptr),child(vector<std::unique_ptr<NodeG>>()),
            goal_list(vector<pair<Point,std::vector<u_int16_t>>>()){
        this->goal_list.push_back({g,{id_path}});
        this->goal_likelihood.push_back(likelihood);
    }

    template<typename P = Point>
    void add_goal(P&& g,u_int16_t id_path,double likelihood)
    {
        auto it = std::find_if( goal_list.begin(), goal_list.end(),
                                [&g](const std::pair<Point, std::vector<u_int16_t>>& element){ return element.first == g;} );

        if (it==goal_list.end()) {
            goal_list.push_back({std::forward<P>(g), {id_path}});
            goal_likelihood.push_back(likelihood);

        }
        else {
            it->second.push_back(id_path);
            auto index_number = std::distance(goal_list.begin(), it);
            assert(index_number>=0);
            goal_likelihood[index_number]+=likelihood;
            assert(goal_likelihood[index_number]>=0 and goal_likelihood[index_number]<=1 );

        }
    }
};

class GoalRecognition{
    std::vector<std::vector<Point>> all_pathz;
    std::unique_ptr<NodeG> root;
    NodeG* curr_ptr;
    std::vector<u_int32_t> min_step_path;
    Point my_loction;
    std::vector<double> probabilities;
    std::default_random_engine rng;
    bool start_move=false;
public:

    explicit GoalRecognition(int _seed):root(std::make_unique<NodeG>()),my_loction(-1),rng(_seed)
    {
        cout<<root->pos.to_str()<<endl;
        curr_ptr=root.get();

    }
    void set_my_location(const Point& p);

    void load_agent_paths(const std::vector<std::vector<Point>> &pathz,vector<double> &&path_probabilties);

    void add_path(const std::vector<Point> &l_path,u_int16_t  id_path,NodeG* cur_root);

    static NodeG* search_node(const vector<std::unique_ptr<NodeG>> &continer,const Point& p);

    void printTree();

    void reset_ptr()
    {
        curr_ptr= root.get();
        start_move=false;
        //set_my_location(this->my_loction);
    }

    void search_tree(const Point &p, NodeG* ptr);
    bool is_stay_inplace(const Point &evader);
    size_t make_decsion(const Point &evader);
    template< typename C>
    u_int32_t get_the_most_likely_path(std::vector<C> idx_list);
    void populate_distances();

    template< typename V>
    static size_t get_max_index_random(std::vector<V> list_vec,std::default_random_engine &rng);

    Point do_action(const Point &evader );

};


void GoalRecognition::load_agent_paths(const std::vector<std::vector<Point>> &pathz,vector<double> &&path_probabilties) {
    probabilities = std::move(path_probabilties);
    all_pathz = pathz;
    for(size_t i=0; i < pathz.size();++i)
        GoalRecognition::add_path(pathz[i],u_int16_t(i),root.get());

    printTree();

}



void GoalRecognition::add_path(const std::vector<Point> &l_path,const u_int16_t id_path,NodeG* cur_root) {

    NodeG* node = cur_root;
    for (size_t i=0;i<l_path.size();++i)
    {
        if (auto res = search_node(node->child,l_path[i]); res == nullptr){
            auto& bPtr = node->child.emplace_back(std::make_unique<NodeG>(l_path[i],l_path.back(),id_path,probabilities[id_path]));
            node = bPtr.get();
        }
        else{
            res->add_goal(l_path.back(),id_path,probabilities[id_path]); // add goal if need
            node = res;

        }
        if (node->min_step>l_path.size()-i)
            node->min_step=int(l_path.size()-i);

    }

}

NodeG *GoalRecognition::search_node(const vector<std::unique_ptr<NodeG>> &continer, const Point &p) {
    for (const auto &item : continer){
        if(item->pos==p)
            return item.get();
    }
    return nullptr;
}

void GoalRecognition::printTree()
{
    cout<<"----Tree---"<<endl;
    auto q = std::queue<NodeG*>();
    auto lv_q = std::queue<int>();
    auto map_d = std::unordered_map<u_int64_t,short>();
    int _level;
    q.push(this->root.get());
    lv_q.push(0);
    while(!q.empty())
    {

        _level = lv_q.front();
        lv_q.pop();
        NodeG* cur_node = q.front();
        q.pop();
        cout<<"lv:"<<_level<<"\t";
        for (auto& item :cur_node->child) {
            cout<<"[";

            std::for_each(item->goal_list.begin(),item->goal_list.end(),[](const pair<Point,std::vector<u_int16_t>>& g){
                std::string s;
                if (g.second.empty())
                    s="";
                else {
                    s = std::accumulate(g.second.begin() + 1, g.second.end(), std::to_string(g.second[0]),
                                                    [](const std::string &a, int b) {
                                                        return a + ',' + std::to_string(b);
                                                    });
                }
                cout<<g.first[1]<<" {"<<s<<"} :";
                });

            q.push(item.get());
            cout<<"]";
            lv_q.push(++_level);
        }
        if (cur_node->child.empty())
            cout<<"[/]";


        cout<<endl;
    }
    cout<<_level<<endl;
}

void GoalRecognition::search_tree(const Point &p, NodeG* ptr) {
    if(ptr->pos==p) {
        curr_ptr = ptr;
        return;
    }
    for (const auto& ele : ptr->child)
    {
        search_tree(p,ele.get());
    }

}





size_t GoalRecognition::make_decsion(const Point &evader ) {
    this->search_tree(evader,curr_ptr);
    //assert(evader==curr_ptr->pos);

    vector<u_int32_t > most_likely_paths;

    double max_prob=0;
    size_t ctr=0;
    size_t max_indx=0;
    size_t likely_path=0;

    if (curr_ptr->goal_list.size()>1)
        max_indx = get_max_index_random(curr_ptr->goal_likelihood,rng);

    if (curr_ptr->goal_list[max_indx].second.size()>1) {
        std::vector<u_int16_t> list_of_paths = curr_ptr->goal_list[max_indx].second;
        likely_path = get_the_most_likely_path(list_of_paths);
    }

    cout << "goal_likly:" << curr_ptr->goal_list[max_indx].first.to_str() << endl;
    cout<<"likely_path:"<<likely_path<<endl;


    for(auto& [g,list_idx]: curr_ptr->goal_list)
    {
        cout<<"Goal:\t"<<g.to_str()<<"\n{";
        cout<<"P(Goal):\t"<<curr_ptr->goal_likelihood[ctr]<<"\n{";
        for (auto idx_i : list_idx)
        {
            cout<<idx_i<<",";
        }
        cout<<"}\n";
        ctr++;
    }
    return likely_path;
}

void GoalRecognition::populate_distances() {
    assert(my_loction.sum()>0);
    for(const auto& path_i : this->all_pathz)
        this->min_step_path.push_back(getMaxDistance(path_i[path_i.size()-2], this->my_loction));
}
void GoalRecognition::set_my_location(const Point& p)
{
    my_loction=p;
    if (!start_move)
        this->populate_distances();
}

template<typename V>
size_t GoalRecognition::get_max_index_random(vector<V> list_vec, std::default_random_engine &rng) {
    std::vector<u_int32_t> l;
    auto it = std::max_element(list_vec.begin(),list_vec.end());
    size_t index_it = std::distance(list_vec.begin(),it);
    l.push_back(index_it);
    std::for_each(it,list_vec.end(),[&](V &val){
        if (val==*it)
            l.push_back(index_it);
        index_it++;
    });
    if (l.size()==1)
        return l.front();
    std::shuffle(std::begin(l), std::end(l), rng);
    return l.front();
}

template< typename C>
u_int32_t GoalRecognition::get_the_most_likely_path(std::vector<C> idx_list) {
    std::vector<double> l;
    std::transform(idx_list.begin(), idx_list.end(), std::back_inserter(l),
                   [this](u_int32_t c) -> double { return probabilities[c]; });
    return idx_list[get_max_index_random(l,this->rng)];
}

bool GoalRecognition::is_stay_inplace(const Point &evader) {
    this->search_tree(evader,curr_ptr);
    size_t min_step = curr_ptr->min_step;
    assert(evader==curr_ptr->pos);
    std::vector<size_t> relevent_pathz;
    for (const auto& [g,list_idx]:curr_ptr->goal_list){
        for (const auto id_pathz : list_idx )
            if (this->min_step_path[id_pathz]<(curr_ptr->min_step+MAX_SPEED_E))
                return false;
    }
    return true;

}

Point GoalRecognition::do_action(const Point &evader) {
    if (!start_move){
        if (is_stay_inplace(evader))
            return Point(0);
        else
            start_move=true;
    }
    auto path = this->make_decsion(evader);
    auto goal = this->all_pathz[path][this->all_pathz[path].size()-2];
    Point dif = goal-my_loction;
    dif.change_speed_max(MAX_SPEED_P);
    return dif;
}


#endif //TRACK_RACING_GOALREC_HPP
