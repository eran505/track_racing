//
// Created by ise on 5.1.2020.
//
#include <fstream>
#include "util_game.hpp"
#include <string_view>
#include <unistd.h>
#ifndef TRACK_RACING_UTILCLASS_HPP
#define TRACK_RACING_UTILCLASS_HPP


template <typename S, typename V>
auto calcMovingAvg(pair<vector<pair<S,V>>,V>& iItem)
{
    V res;
    std::for_each(iItem.second.begin(),iItem.second.end(),[&](const pair<pair<S,V>,V>& x ){
        res+=(x.first*x.second/iItem.second);
    });
    return res;
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
    int seed;
    Point sizeGrid ;
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
    explicit configGame(vector<string> &row):seed(0),generator(seed)
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
        refPoint.array[1]+=noiseY;
        refPoint.array[0]+=noiseX;
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
