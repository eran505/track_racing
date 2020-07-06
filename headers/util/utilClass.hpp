//
// Created by ise on 5.1.2020.
//
#include <fstream>
#include "util_game.hpp"
#include <string_view>
#include <unistd.h>
#include <cstring>
#ifndef TRACK_RACING_UTILCLASS_HPP
#define TRACK_RACING_UTILCLASS_HPP

unordered_map<char,string> parser(char** view_str,int size_arr)
{
    unordered_map<char,string> argv_dict;
    auto i=1;
    while(i<size_arr)
    {
        char sign =  view_str[i][0];
        argv_dict.try_emplace(sign,view_str[++i]);
        ++i;
    }
    return argv_dict;
}
template <typename S, typename V>
auto calcMovingAvg(pair<vector<pair<S,V>>,V>& iItem)
{
    V res;
    std::for_each(iItem.second.begin(),iItem.second.end(),[&](const pair<pair<S,V>,V>& x ){
        res+=(x.first*x.second/iItem.second);
    });
    return res;
}

template<typename T = std::string>
std::vector<T> cut_first_appear(const std::vector<T> &l,const T& t)
{
    auto pos = std::find(l.begin(),l.end(),t);
    return std::vector<T> (l.begin(),pos);
}
std::string join(const std::vector<std::string> & sequence, const std::string & separator)
{
    std::string result;
    for(size_t i = 0; i < sequence.size(); ++i)
        result += sequence[i] + ((i != sequence.size()-1) ? separator : "");
    return result;
}
string getExePath()
{
    char result[ PATH_MAX ];
    ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    return string( result, (count > 0) ? count : 0 );
}

vector<string> splitStr(const string& str, const string& delim)
{
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    if (str.length()<1)
        return tokens;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

struct configGame{
    int _seed;
    Point sizeGrid ;
    Point abst = Point(0);
    string config;
    Point posAttacker;
    int maxA{};
    int maxD{};
    Point posDefender;
    vector<Point> gGoals ;
    vector<double> probGoals;
    vector<bool> goalTarget;
    int rRoutes;
    string idNumber;
    vector<Point> midPos;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;

public:

    string home;
    void initRandomNoise()
    {
        auto insetNoiseFunc = [&](Point &item){inset_noise_XY(item);};
        std::for_each(gGoals.begin(),gGoals.end(),insetNoiseFunc);
        inset_noise_XY(this->posAttacker);
        inset_noise_XY(this->posDefender);
    }
    explicit configGame(vector<string> &row,int seed):_seed(seed),generator(_seed)
    {

        distribution = std::uniform_int_distribution<int>(-1,1);
        idNumber=row[0];
        sizeGrid = Point(stoi(row[1]),stoi(row[2]),stoi(row[3]));
        posAttacker = vecToPoint(splitStr(row[4],"|"));
        posDefender = vecToPoint(splitStr(row[5],"|"));
        auto goalsVecPos = splitStr(row[6],"-");
        auto goalsVecWights = splitStr(row[7],"-");
        auto midVec = splitStr(row[9],"-");
        auto isGoal = splitStr(row[10],"-");
        stringToBool(isGoal);
        addGoal(goalsVecPos,goalsVecWights);
        rRoutes = stoi(row[8]);
        if (!midVec.empty()) addMidPoint(midVec);
        config = "";

    };
    double getRandom(){return distribution(generator);}
    void inset_noise_XY(Point &refPoint)
    {
        auto noiseX = distribution(generator);
        auto noiseY = distribution(generator);
        refPoint.array[1]=std::min(std::max(noiseY+refPoint.array[1],0),sizeGrid[1]-1);
        refPoint.array[0]=std::min(std::max(refPoint.array[0]+noiseX,0),sizeGrid[0]-1);
    }
    void getConfigNameFile(string& str)
    {
        config=str;
    }

protected:

    static Point vecToPoint(vector<string> arr)
    {
        if (Point::D == 3)
            return Point(stoi(arr[0]),stoi(arr[1]),stoi(arr[2]));
        if (Point::D == 2)
            return Point(stoi(arr[0]),stoi(arr[1]));
        throw std::invalid_argument( "No Matching Point Size [utilClass.hpp]" );
    }
    void addGoal(vector<string> &rGoalsVecPos,const vector<string>& rProbGoals){
        for(auto &item:rGoalsVecPos){
            auto arrString = splitStr(item,"|");
            this->gGoals.push_back(vecToPoint(arrString));
        }
        for(auto &item:rProbGoals){
            double pProb = stoi(item)/double(100);
            this->probGoals.push_back(pProb);
        }
    }
    void addMidPoint(const vector<string> &vecMid){
        for(auto &pointStr:vecMid)
        {
            if (pointStr.length()<2)
                continue;
            auto arrString = splitStr(pointStr,"|");
            this->midPos.push_back(vecToPoint(move(arrString)));
        }
    }
    void stringToBool(const vector<string> &vec)
    {
        for(const auto &strI : vec)
        {
            if (strI == "0" )
                goalTarget.push_back(false);
            else if(strI == "1")
                goalTarget.push_back(true);
            else
                throw;
        }
    }

};

#endif //TRACK_RACING_UTILCLASS_HPP
