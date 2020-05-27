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
    void assignment(State &other, const string &id);
    float isGoal(string &idStr);
    bool isEndState(std::string &idStr);
    //Setters and Getters

    State * getAbstractionState(Point &abstractPoint);
    [[nodiscard]] vector<Point> getAllPos(const Point &abstractPoint)const;
    void set_budget(const string& name_id,int budget_m){budget_dict[name_id]=budget_m;}
    int get_budget(const string& name_id){ return budget_dict[name_id];}
    void set_speed(const string& name_id,const Point& speed_m){speed_dict[name_id]=speed_m;}
    Point get_speed(const string& name_id){return speed_dict[name_id];}
    void set_position(const string& name_id,const Point& pos_m){pos_dict[name_id]=pos_m;}
    const Point&  get_position(const string &name_id){ return pos_dict[name_id];}
    set<string> is_collusion();
    virtual bool applyAction(const string &id, const Point &action, int max_speed);
    list<string> is_collusion(string &id_player);
    bool is_collusion(string &id_player,string &op_player);
    void getAllPosOpponent(vector<Point> &results,char team);
    std::ostream& operator<<(std::ostream &strm) {
        return strm <<this->to_string_state();
    }
    [[nodiscard]] u_int64_t  getHashValue()const;
    [[nodiscard]] u_int64_t  getHashValuePosOnly()const;

    [[nodiscard]] string to_string_state() const;

    void add_player_state(const string& name_id, Point m_pos, const Point *m_speed, int budget_b);
    void add_player_state(const string &name_id, const Point& m_pos, const Point& m_speed, int budget_b);

    vector<string> getIDs();
};


#endif //RACING_CAR_STATE_HPP
