//
// Created by eranhe on 21/06/2020.
//

#ifndef TRACK_RACING_CONTAINERABSTRACT_H
#define TRACK_RACING_CONTAINERABSTRACT_H
//#define assertMac
#include "util_game.hpp"
#include "Agent.hpp"
#include "State.hpp"
//#include <cassert>
class containerAbstract{

    Point _offset;
    Point _abstract;
    Point _orgin;
    Point _divPoint;
    unordered_map<u_int32_t,std::shared_ptr<Agent>> _lagh;
    u_int32_t idxBigScope;
    u_int32_t  idx=0;
    bool absState=false;

    bool isOnTheSameGrid(const State *s)
    {
        vector<Point> l;
        s->getAllPos(l,this->_abstract);
        assert(l.size()==2);
        if(!(l[0]==l[1])){
            return false;
        }
        else{
            auto row = l[0].array[0]*_divPoint.array[0];
            auto col = l[0].array[1]%_divPoint.array[1];
            idx=col+row;
            return true;
        }

    }

    inline u_int32_t getIndexMiniGrid(const Point &p){
        auto row_i = p.array[0]*_divPoint.array[0];
        auto col_i = p.array[1]%_divPoint.array[1];
        idx=col_i+row_i;

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
    void eval(){for(auto &item:_lagh)item.second->evalPolicy();}
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
        if(!isOnTheSameGrid(s))
        {
            absState=true;
            return _lagh[idxBigScope].get();
        }
        if(auto pos = _lagh.find(idx); pos==_lagh.end())
        {
            cout<<"\t not found\t";
            return _lagh[idxBigScope].get();
            //assert(false);
        }
        else
            return pos->second.get();
    }



};

#endif //TRACK_RACING_CONTAINERABSTRACT_H
