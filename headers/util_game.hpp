//
// Created by ise on 14.11.2019.
//

#ifndef RACING_CAR_UTIL_GAME_HPP
#define RACING_CAR_UTIL_GAME_HPP

#include <string>
#include <list>
#include <iostream>
#include <vector>
#include <climits>
#include <unordered_map>
#include <random>
#include <cmath>
#include <map>
#include <algorithm>
#include <set>
#include <memory>
#include <queue>
using std::vector;
using std::map;
using std::unordered_map;
using std::cout;
using std::list;
using std::ios_base;using std::multimap;
using std::pair;using std::shared_ptr;
using std::reverse_iterator;using std::tuple;
using std::ostream;using std::set;
using std::string;
using std::endl;
using std::queue;


enum Section{
    adversary = (int)'A',
    gurd   = (int)'D'
};


class Point{

public:
    enum D_point{
        D = 3, actionMax=3, maxSpeed=5
    };
    ~Point() = default;
    Point(int x);
    Point(int x,int y);
    Point(int x,int y,int z);
    std::string to_str() const {
        std::string str_to_string;
        str_to_string+="(";
        for ( int i = 0; i < this->capacity-1; i++) {
            str_to_string += std::to_string((this->array[i]));
            str_to_string += ", ";
        }
        str_to_string+= std::to_string(*(array + this->capacity-1));
        str_to_string+=')';
        return str_to_string;
    }
    string to_hash_str() const {
        string str;
        for (int i = 0; i < this->capacity; ++i) {
            str += std::to_string(this->operator[](i));
        }
        return str;
    }
    friend bool operator>(const Point &other1,const Point &other2)
    {
        for (int i = 0; i <other1.capacity; ++i)
            if (other1.array[i]<=other2.array[i])
                return false;
        return true;
    }
    friend bool operator>=(const Point &other1,const Point &other2)
    {
        for (int i = 0; i <other1.capacity; ++i)
            if (other1.array[i]<other2.array[i])
                return false;
        return true;
    }
    int multi(){
        int res=1;
        for (int i = 0; i < this->capacity; ++i) {
            res*=this->array[i];
        }
        return res;
    }
    bool operator<(const Point &other)
    {
        for (int i = 0; i < this->capacity; ++i) {
            if(other[i]<=this->array[i])
                return false;
        }
        return true;
    }
    int sumPoint() const
    {
        int sum=0;
        for (int i = 0; i < this->capacity; ++i) {
            sum+=this->array[i];
        }
        return sum;
    }
    int capacity=D_point::D;
    int array[D_point::D]{};
    Point(){ //printf("POINT_EMPTY_CON\n");
    };
    Point(const Point &other):capacity(other.capacity)
    {
        //printf("COPY_CON POINT\n  ");
        for (int i = 0; i < this->capacity; ++i) {
            this->array[i]=other.array[i];
        }
    }

    Point& operator=(const Point &other){
        //printf("operator= Point \n  ");
        for (int i = 0; i < this->capacity; ++i) {
            this->array[i]=other.array[i];
        }
        return *this;
    }

    Point operator+(const Point &other)const{
        Point ans(0);
        for (int i = 0; i < this->capacity; ++i) {
            ans.array[i]=this->array[i]+other.array[i];
        }
        return ans;
    }
    Point operator*(const Point &other)const{
        Point ans(0);
        for (int i = 0; i < this->capacity; ++i) {
            ans.array[i]=this->array[i]*other.array[i];
        }
        return ans;
    }

    void operator+=(const Point &other){

        for (int i = 0; i < this->capacity; ++i) {
            this->array[i]=this->array[i]+other.array[i];
        }

    }
    bool pairIsEqule(pair<int,int> data)
    {
        if(this->array[0]!=data.first or this->array[1]!=data.second)
            return false;
        return true;
    }
    int hash2D() const
    {
        return  (53 + (array[0])) * 53 + (array[1]);
    }
//    int hash3D(){
//        return  ((53 +  array[0]) * 53 +  array[1]) * 53 + array[2];
//    }

    Point operator-(const Point* other) const{
        Point newp(0);
        for (int i = 0; i < this->capacity; ++i) {
            newp.array[i]=this->array[i]-other->array[i];
        }
        return newp;
    }

    Point operator%(const Point& other) const
    {
        Point p(0);
        for (int i = 0; i < this->capacity; ++i) {
            p.array[i]=this->array[i]%other.array[i];
        }
        return p;
    }

    Point& operator-=(const Point* other){

        for (int i = 0; i < this->capacity; ++i) {
            this->array[i]=this->array[i]-other->array[i];
        }
        return *this;
    }
    Point& operator-=(const Point& other){

        for (int i = 0; i < this->capacity; ++i) {
            this->array[i]=this->array[i]-other.array[i];
        }
        return *this;
    }
    bool any_bigger_equle(const Point &other){
        for (int i = 0; i < this->capacity; ++i) {
            if (this->array[i]<=other.array[i])
                return true;
        }
        return false;
    }
    bool any_bigger(const Point &other){
        for (int i = 0; i < this->capacity; ++i) {
            if (this->array[i]>other.array[i])
                return true;
        }
        return false;
    }
    [[nodiscard]] bool out_of_bound(const Point &boundLower,const Point &boundUpper) const{
        for (int i = 0; i < this->capacity; ++i) {
            if (this->array[i]>=boundUpper.array[i] or
            this->array[i]<boundLower.array[i])
                return true;
        }
        return false;
    }
    [[nodiscard]] bool out_of_bound(const Point &bound) const{
        for (int i = 0; i < this->capacity; ++i) {
            if (this->array[i]>=bound.array[i] or
                this->array[i]<0)
                return true;
        }
        return false;
    }
    int sum() const
    {
        int ans = 0;
        for (int i = 0; i < this->capacity; ++i) ans+=this->array[i];
        return ans;
    }
    vector<double>* getFeature()
    {
        auto* vectorI = new vector<double>();
        for (int i = 0; i < this->capacity; ++i)
            vectorI->push_back(this->array[i]);

        return vectorI;
    }
    [[nodiscard]] bool any_ngative()const {
        for (int i = 0; i < this->capacity; ++i) {
            if (this->array[i]<0)
                return true;
        }
        return false;
    }
    void operator/=(const Point &x){
        for (int i = 0; i < this->capacity; ++i) {
            array[i]/=x[i];
        }
    }
    Point operator/(const Point &x)const{
        Point res(0);
        for (int i = 0; i < this->capacity; ++i) {
            res.array[i]=array[i]/x[i];
        }
        return res;
    }
    [[nodiscard]] int accMulti()const{
        int acc=1;
        for (int i = 0; i < this->capacity; ++i) {
            acc*=this->array[i];
        }
        return acc;
    }

    [[nodiscard]] Point AbsPoint() const
    {
        Point tmp(0);
        for (int i = 0; i < this->capacity; ++i)
            tmp.array[i]=abs(this->array[i]);
        return tmp;
    }

    bool isBiggerAbsOne()
    {
        for (int i = 0; i < this->capacity; ++i)
        {
            if(abs(array[i])>1)
                return true;
        }
        return false;
    }
    void change_speed_max(int absoult_max){
        for (int i = 0; i < this->capacity; ++i) {
            if (this->array[i]>absoult_max){
                this->array[i]=absoult_max;
                continue;
            }
            if(this->array[i]*-1>absoult_max)
                this->array[i]=absoult_max*-1;
        }
    }
    bool operator== (const Point &other)const;
    bool is_equal(const Point *other)const;
    int operator[](int index)const;

    static vector<Point> getAllAction(vector<Point> &action_list){
        action_list.clear();
        if (Point::D==2){
            for (int i = -1; i < 2; ++i)
                for (int j = -1; j < 2; ++j)
                    action_list.emplace_back(i,j);
        }else if(Point::D==3){
            for (int i = -1; i < 2; ++i)
                for (int j = -1; j < 2; ++j)
                    for (int k = -1; k < 2; ++k)
                        action_list.emplace_back(i,j,k);}
        return action_list;
    }

    static vector<Point> getAllAction2(vector<Point> &action_list,int x=-1){
        action_list.clear();
        if (Point::D==2){
            for (int i = -1; i < 2; ++i)
                for (int j = -1; j < 2; ++j)
                    action_list.emplace_back(i,j);
        }else if(Point::D==3){
            for (int i = -1; i < 2; ++i)
                for (int j = -1; j < 2; ++j)
                    for (int k = -1; k < 2; ++k)
                    {
                        if (x==0)
                            action_list.emplace_back(0,j,k);
                        if (x==1)
                            action_list.emplace_back(i,0,k);
                        if (x==2)
                            action_list.emplace_back(i,j,0);
                    }

                        }
        return action_list;
    }


    [[nodiscard]] u_int64_t hashConst(int offset=0)const {

        auto h=hashNnN(array[0]+offset,array[1]+offset);
        for (int i = 2; i < capacity; ++i) {
            h=hashNnN(h,this->array[i]+offset);
        }
        return h;
    }
    static unordered_map<int,Point*>* getDictAction();
    Point operator-(const Point &other) const
    {
        Point newP;
        for (int i = 0; i < capacity; ++i) {
            newP.array[i]=this->array[i]-other.array[i];
        }
        return newP;
    }
    unsigned int hashMeAction(int max){
        unsigned int h=0;
        int append=int(pow(int(actionMax),this->capacity))/2;
        for (int i = 0; i < this->capacity; ++i)
            h+=this->array[capacity-1-i]*int(pow(max,i));
        return h+append;
    }
    Point operator*(int x) const{
        Point tmp(*this);
        for (int i = 0; i < capacity; ++i) {
            tmp.array[i]*=x;
        }
        return tmp;
    }
    bool isOK(){
        bool ok = true;
        for (int i = 0; i < this->capacity; ++i)
            if (array[i]>1 or array[i]<-1)
                return false;
        return ok;
    }
    bool is_negative()
    {
        for (int i = 0; i < this->capacity; ++i)
            if (array[i]<0)
                return true;
        return false;
    }
    [[nodiscard]] u_int64_t expHash()const {
        std::size_t seed = capacity;
        for(auto& i : array) {
            seed ^=  (i * 2654435761) + 2654435769 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }

    [[nodiscard]] u_int64_t expHash(u_int64_t seed)const {
        for(auto& i : array) {
            seed ^=  (i * 2654435761) + 2654435769 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
    [[nodiscard]] u_int64_t expHash(const Point& other)const {
        u_int64_t seed=capacity;
        for(auto& i : array) {
            seed ^=  (i * 2654435761) + 2654435769 + (seed << 6) + (seed >> 2);
        }
        for(auto& j : other.array) {
            seed ^=  (j * 2654435761) + 2654435769 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
    unsigned int hashMeMAX(int max){
        unsigned int h=0;
        //int append=int(pow(int(actionMax),this->capacity))/2;
        for (int i = 0; i < this->capacity; ++i)
            h+=this->array[capacity-1-i]*int(pow(max,i));
        return h;
    }

    static u_int64_t hashNnN(u_int64_t x,u_int64_t y)
    {

        return ( 0.5*double( (x+y) * (x+y+1) ) + y ) ;
    }
};


int getMaxDistance(const Point &a , const  Point &b );
int getMaxDistancePos(const Point &a , const  Point &b );
int range_random(int min, int max); //range : [min, max)
vector<double> getTopK(int k,vector<double> &vec);
//template<typename T>
//std::ostream& print(std::ostream &out, T const &val) {
//    return (out << val << " ");
//}
//
//template<typename T1, typename T2>
//std::ostream& print(std::ostream &out, std::pair<T1, T2> const &val) {
//    return (out << "{" << val.first << " " << val.second << "} ");
//}
//
//template<template<typename, typename...> class TT, typename... Args>
//std::ostream& operator<<(std::ostream &out, TT<Args...> const &cont) {
//    for(auto&& elem : cont) print(out, elem);
//    return out;
//}

template <typename T>

bool Contains( std::vector<double>& Vec, const double Element )
{
    if (std::find(Vec.begin(), Vec.end(), Element) != Vec.end())
        return true;

    return false;
}

template<typename K=uint32_t>
auto map_transpose(const std::unordered_map<K,std::vector<K>> &dico)
{
    unordered_map<K,std::vector<K>> map;
    for (auto [k,v]:dico)
    {
        for(auto &item:v)
        {
            if(auto pos = map.find(item);pos!=map.end())
                pos->second.push_back(k);
            else
                map[item]={k};
        }
    }
    return map;
}

struct weightedPosition{
    Point speedPoint;
    Point positionPoint;
    double weightedVal;
    weightedPosition(const Point& speed_,const Point& pos_, double p)
    :speedPoint(speed_),positionPoint(pos_),weightedVal(p){}
    bool operator== (const weightedPosition &other)const
    {
        if(this->positionPoint==other.positionPoint)
            if(this->speedPoint==other.speedPoint)
                return true;
        return false;
    }

    u_int64_t getHash() const{
        return this->speedPoint.expHash(this->positionPoint.expHash());
    }

    void operator= (const weightedPosition &other)
    {
        this->positionPoint=other.positionPoint;
        this->speedPoint=other.speedPoint;
        this->weightedVal=other.weightedVal;
    }
    weightedPosition(const weightedPosition &other)
    :speedPoint(other.speedPoint),
    positionPoint(other.positionPoint),
    weightedVal(other.weightedVal){}
    weightedPosition()
    :positionPoint(0),speedPoint(0),weightedVal(-1)
    {}

};



#endif //RACING_CAR_UTIL_GAME_HPP
