//
// Created by ise on 18.11.2019.
//

#ifndef RACING_CAR_STATE_HPP
#define RACING_CAR_STATE_HPP

#include "util_game.hpp"
#include "Grid.hpp"


class State{

public:
    enum agentEnum :short{A=0,D=1,LAST=2};

    std::array<Point,4> dataPoint; // [A_POS,A_SPEED,D_POS,D_SPEED]
    std::array<short,2> budgets{};
    Grid *g_grid = nullptr;
    bool takeOff = false;

    [[nodiscard]] const Point& get_speed_ref(agentEnum agent_id)const{return dataPoint[agent_id*2+1];}
    [[nodiscard]] Point get_speed(agentEnum agent_id)const{return dataPoint[agent_id*2+1];}
    void set_speed(agentEnum agent_id,const Point& p){dataPoint[agent_id*2+1]=p;}

    [[nodiscard]] const Point& get_position_ref(agentEnum agent_id)const{return dataPoint[agent_id*2];}
    void set_position(agentEnum agent_id,const Point& p){dataPoint[agent_id*2]=p;}
    [[nodiscard]] Point get_position(agentEnum agent_id)const{return dataPoint[agent_id*2];}


    [[nodiscard]] short get_budget(agentEnum agent_id)const{return budgets[agent_id];}
    void set_budget(agentEnum agent_id,short b){budgets[agent_id]=b;}




    State():g_grid(nullptr),takeOff(false){};
    State(const State &other) = default;


    void assignment( State &other);
    void assignment( const State *other,agentEnum idname);
    [[nodiscard]] double isGoal(agentEnum idStr)const;
    [[nodiscard]] bool isEndState(agentEnum idStr)const;
    //Setters and Getters

    //void getAllPos(vector<Point> &vec,const Point &abstractPoint)const;
    void getAllPos(vector<Point> &vec)const;
//    void set_budget(const string& name_id,int budget_m){budget_dict[name_id]=budget_m;}
//    int get_budget(const string& name_id)const {return this->budget_dict.at(name_id);} //#TODO: change it when using budget
//    void set_speed(const string& name_id,const Point& speed_m){speed_dict[name_id]=speed_m;}
//    [[nodiscard]] Point get_speed(const string& name_id)const{return speed_dict.at( name_id);}
//    void set_position(const string& name_id,const Point& pos_m){pos_dict[name_id]=pos_m;}
//    [[nodiscard]] const Point&  get_position_ref(const string &name_id)const { return getValue(pos_dict,name_id);}
//    [[nodiscard]] const Point&  get_speed_ref(const string &name_id)const { return getValue(speed_dict,name_id);}
    //set<string> is_collusion();
    virtual bool applyAction(agentEnum id, const Point &action, int max_speed);
    [[nodiscard]] bool is_collusion(agentEnum id_player,agentEnum op_player)const;
    //void getAllPosOpponent(vector<Point> &results,char team);
    std::ostream& operator<<(std::ostream &strm) const {
        return strm <<this->to_string_state();
    }
    [[nodiscard]] u_int64_t  getHashValue()const;
    [[nodiscard]] u_int64_t  getHashValuePosOnly()const;

    [[nodiscard]] string to_string_state() const;
    [[nodiscard]] std::array<int,12> to_mini_string() const;
    void add_player_state(agentEnum name_id, const Point& m_pos, const Point& m_speed, short budget_b);


    [[nodiscard]] static const Point& getValue(const map<string const,Point>& map , const string &str_name)
    {
        if(auto pos = map.find(str_name); pos==map.end())
        {
            throw std::invalid_argument( "received missing value" );
        }else{ return pos->second;}

    }



    bool is_collusion_radius(agentEnum id_player, agentEnum op_player, const Point &window);

    void add_player_state(agentEnum name_id, const Point &m_pos, const Point *m_speed, int budget_b);
};



#endif //RACING_CAR_STATE_HPP

