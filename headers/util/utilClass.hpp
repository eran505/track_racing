//
// Created by ise on 5.1.2020.
//
#include <fstream>
#include "util_game.hpp"
#include <unistd.h>
#ifndef TRACK_RACING_UTILCLASS_HPP
#define TRACK_RACING_UTILCLASS_HPP

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
    if (str.length()<2)
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

    Point sizeGrid ;
    Point posAttacker;
    Point posDefender;
    vector<Point> gGoals ;
    vector<float> probGoals;
    int rRoutes;
    string idNumber;
    vector<Point> midPos;

    explicit configGame(vector<string> row)
    {

        idNumber=row[0];
        sizeGrid = Point(stoi(row[1]),stoi(row[2]),stoi(row[3]));
        posAttacker = vecToPoint(std::move(splitStr(row[4],"|")));
        posDefender = vecToPoint(std::move(splitStr(row[5],"|")));
        auto goalsVecPos = splitStr(row[6],"-");
        auto goalsVecWights = splitStr(row[7],"-");
        auto midVec = splitStr(row[9],"-");
        addGoal(goalsVecPos,goalsVecWights);
        rRoutes = stoi(row[8]);
        if (!midVec.empty()) addMidPoint(midVec);


    };
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
            double pProb = stoi(item)/float(100);
            this->probGoals.push_back(pProb);
        }
    }
    void addMidPoint(const vector<string> &vecMid){
        for(auto &pointStr:vecMid)
        {
            auto arrString = splitStr(pointStr,"|");
            this->midPos.push_back(vecToPoint(move(arrString)));
        }
    }

};

#endif //TRACK_RACING_UTILCLASS_HPP
