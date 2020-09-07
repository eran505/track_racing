//
// Created by ise on 18.11.2019.
//

#ifndef RACING_CAR_STATE_HPP
#define RACING_CAR_STATE_HPP

#include "util_game.hpp"
#include "Grid.hpp"

class State{



public:
    map<string const,Point> pos_dict;
    map<string const,Point> speed_dict;
    map<string const,int> budget_dict;
    Grid *g_grid;
    bool takeOff;
    State():g_grid(nullptr),takeOff(false){};
    virtual ~State();
    State(const State &other);

    void assignment( State &other);
    void assignment(const State &other, const string &id);
    double isGoal(string &idStr)const;
    bool isEndState(const std::string &idStr)const;
    //Setters and Getters

    std::unique_ptr<State> getAbstractionState(Point &abstractPoint);
    void getAllPos(vector<Point> &vec,const Point &abstractPoint)const;
    void getAllPos(vector<Point> &vec)const;
    void set_budget(const string& name_id,int budget_m){budget_dict[name_id]=budget_m;}
    int get_budget(const string& name_id)const {return this->budget_dict.at(name_id);} //#TODO: change it when using budget
    void set_speed(const string& name_id,const Point& speed_m){speed_dict[name_id]=speed_m;}
    [[nodiscard]] Point get_speed(const string& name_id)const{return speed_dict.at( name_id);}
    void set_position(const string& name_id,const Point& pos_m){pos_dict[name_id]=pos_m;}
    [[nodiscard]] const Point&  get_position_ref(const string &name_id)const { return getValue(pos_dict,name_id);}
    [[nodiscard]] const Point&  get_speed_ref(const string &name_id)const { return getValue(speed_dict,name_id);}
    set<string> is_collusion();
    virtual bool applyAction(const string &id, const Point &action, int max_speed);
    list<string> is_collusion(string &id_player);
    bool is_collusion(string &id_player,string &op_player)const;
    void getAllPosOpponent(vector<Point> &results,char team);
    std::ostream& operator<<(std::ostream &strm) {
        return strm <<this->to_string_state();
    }
    [[nodiscard]] u_int64_t  getHashValue()const;
    [[nodiscard]] u_int64_t  getHashValuePosOnly()const;

    [[nodiscard]] string to_string_state() const;
    [[nodiscard]] std::array<int,12> to_mini_string() const;
    void add_player_state(const string& name_id, Point m_pos, const Point *m_speed, int budget_b);
    void add_player_state(const string &name_id, const Point& m_pos, const Point& m_speed, int budget_b);

    vector<string> getIDs();

    [[nodiscard]] static const Point& getValue(const map<string const,Point>& map , const string &str_name)
    {
        if(auto pos = map.find(str_name); pos==map.end())
        {
            throw std::invalid_argument( "received missing value" );
        }else{ return pos->second;}

    }

    std::unique_ptr<State> getAbstractionState_inplace(Point &abstractPoint, State *out)const;
    void transform_state_inplace(const Point &trans);

    bool is_collusion_radius(const string &id_player, const string &op_player, const Point &window);

    bool applyAction_SEQ(const string &id, const Point &action);
};


#endif //RACING_CAR_STATE_HPP

