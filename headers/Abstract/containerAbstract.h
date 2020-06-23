//
// Created by eranhe on 21/06/2020.
//

#ifndef TRACK_RACING_CONTAINERABSTRACT_H
#define TRACK_RACING_CONTAINERABSTRACT_H
#define assertMac
#include "util_game.hpp"
#include "Agent.hpp"
#include "State.hpp"
#include <cassert>
class containerAbstract{

    Point _offset;
    Point _abstract;
    Point _orgin;
    Point _divPoint;
    unordered_map<u_int32_t,std::shared_ptr<Agent>> _lagh;
    u_int32_t idxBigScope;
    u_int32_t  idx=0;
    bool absState=false;

    bool is_meet(const State* s)
    {
        auto l = s->getAllPos(this->_abstract);
#ifdef assertMac
        assert(l.size()==2);
#endif
        if (!(l[0]==l[1]))
            return false;

        getIndexMiniGrid(l[0]);
        cout<<"in\t:"<<idx<<endl;
        return true;
    }
    u_int32_t getIndexMiniGrid(const Point &statePos){
        auto row = statePos.array[0]*_divPoint[0];
        auto col = statePos.array[1]%_divPoint[1];
        cout<<"statePos:=>\t"<<col+row<<endl;
        idx=col+row;

    }

public:
    template<typename T = Point,
            typename = std::enable_if<!std::is_same_v<T,containerAbstract>>>
            containerAbstract(T &&offset,T &&ab,T &&orgin,u_int bigAbID)
    :_offset(std::forward<T>(offset)),_abstract(std::forward<T>(ab)),
    _orgin(std::forward<T>(orgin)),_divPoint(_orgin/_abstract),
    idxBigScope(bigAbID){}
    ~containerAbstract()
    {
        cout<<"~containerAbstract"<<endl;
    }
    Point& get_absPoint(){return _abstract;}
    Point& get_divPoint(){return _divPoint;}
    void insetToDict(u_int32_t x,std::shared_ptr<Agent> ptrA)
    {
        _lagh.try_emplace(x,ptrA);
    }
    unordered_map<u_int32_t,std::shared_ptr<Agent>>& get_lagh(){return this->_lagh;};
    bool get_absState()const{return absState;}
    Agent* get_agent(const State* s)
    {

        absState = false;
        if(!is_meet(s))
        {
            absState=true;
            return _lagh[idxBigScope].get();
        }
        if(auto pos = _lagh.find(idx); pos==_lagh.end())
        {
            cout<<s->to_string_state()<<endl;cout<<idx<<endl;cout<<absState<<endl;
            assert(false);
        }
        else
            return pos->second.get();
    }


};

#endif //TRACK_RACING_CONTAINERABSTRACT_H
