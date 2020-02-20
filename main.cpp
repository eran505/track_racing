#include <iostream>
#include "headers/Grid.hpp"
#include "headers/Agent.hpp"
#include <list>
#include "headers/util_game.hpp"
#include "headers/MdpPlaner.hpp"
#include "headers/Game.hpp"
#include "headers/Policy/Dog.hpp"
#include "headers/graph/graph_util.hpp"
#include <utility>
#include <vector>
#include<algorithm>
#include "Policy/RtdpAlgo.hpp"
#include <chrono>
#include "serach/Astar.hpp"
#include "headers/graph/graph_dummy.hpp"
#include "Policy/PathPolicy.hpp"
#include "learning/neuralNet.h"
#include "learning/DeepRTDP.h"
#include "util/utilClass.hpp"
#include <random>
#include <headers/util/csvfile.hpp>
#include <torch/script.h> // One-stop header.

Grid * init_grid(configGame &conf);
MdpPlaner* init_mdp(Grid *g, configGame &conf);
void toCsv(string &pathFile, vector<vector<int>>* infoArr,vector<string> &labels);
Game* initGame(configGame& conf);
vector<vector<string>> readConfigFile(string &filePath);
void toCsvString(string pathFile,vector<string>* infoArr);

/*
 * TODO LIST:
 * 1. in the State class, i think that its enough
 * to say if there were coll, maybe change to boolean.
 *
 * 2. make non deterministic action for the agents
 *
 * 3. heuristic
 *
 * 4. change for the transition phase, save only the diff action,
 * reduce the number of copy constructor.
 *
 * 5. crate trans only for one branch, its dep only on pos_speed bad agent
 *
 */
typedef vector<tuple<Point*,double>> listPointWeighted;
typedef unsigned long ulong;
int main() {
//    char temp[256];
//    auto str_dir =  getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("");
//    cout<<str_dir<<endl;
// ************************************
//    auto nn = new neuralNet();
//    nn->start();
//
//    exit(0);
    // seeding the program
    
    //int seed = int( time(nullptr));
    int seed = 58852;
    cout<<"seed:\t"<<seed<<endl;
    srand(seed);
    auto arrPAth = splitStr(getExePath(),"/");
    string home = "/"+arrPAth[0]+"/"+arrPAth[1];
    int MaxInt = INT_MAX;
    //const string home="/home/ise";
    std::string pathCsv (home + "/car_model/config/con1.csv");
    std::string toCsvPath (home+ "/car_model/config_exp_1/");
    auto csvRows = readConfigFile(pathCsv);
    int ctrId=1;
    vector<string> labels={"ctr_round","ctr_wall","ctr_coll","ctr_at_goal"};
    for (int i=1; i<csvRows.size();++i)
    {
        string curToCsv;
        string curToCsvPolciy;
        auto row = csvRows[i];
        // size of Grid
        configGame conf(row);
        conf.home=home;
        string strId=row[0];
        cout<<"ID:\t"<<strId<<endl;
        curToCsv.append(toCsvPath);curToCsv.append("ID_");
        curToCsv.append(strId);curToCsv.append(".csv");

        curToCsvPolciy.append(toCsvPath);curToCsvPolciy.append("ID_");
        curToCsvPolciy.append(strId);curToCsvPolciy.append("_P.csv");

        auto resultsConfigI = initGame(conf);



        toCsv(curToCsv,resultsConfigI->info,labels);
        toCsv(curToCsvPolciy,resultsConfigI->guardEval,labels);
        ctrId++;
        //Agent::ctr_object = 0;
        delete (resultsConfigI);

    }


    return 0;
}

Game* initGame(configGame &conf ){
    auto g= init_grid(conf);
    //g->print_vaule();

    auto pPlaner = init_mdp(g,conf);

    pPlaner->set_grid(g);

    Game* my_game = new Game(pPlaner);
    //exit(0);
    cout<<"------LOOP GAME!!------"<<endl;

    my_game->startGame(100000);
    string nameFile="buffer_"+conf.idNumber+".csv";
    toCsvString(conf.home+"/car_model/exp/buffer/"+nameFile, my_game->buffer);


    //delete(my_game);
    //delete (info);
    cout<<"------END MAIN!!-----"<<endl;
    return my_game;
}

Grid * init_grid(configGame& conf){
    game_params m{};
    m.size=conf.sizeGrid;
    auto* listGoal = new list<Point*>();
    for (auto &refGoal:conf.gGoals)
        listGoal->push_back(new Point(refGoal));
    m.list_goals=listGoal;
    Grid *g = new Grid(m);
    return g;

}
MdpPlaner* init_mdp(Grid *g, configGame &conf){
    int maxSizeGrid = g->getPointSzie().array[0];
    int maxA=2+maxSizeGrid/10;
    int maxB=1+maxSizeGrid/10;

    auto startAdversary = new Point(conf.posAttacker);

    auto* pA1 = new Agent(startAdversary
            ,new Point(0,0,maxA)
            ,adversary,10);

    auto* pD2 = new Agent(new Point(conf.posDefender),
            new Point(0,0,0)
            ,gurd,10);


    ////////PATH POLICY///////////
    auto gloz_l = g->get_goals();
    Point p_sizer = g->getPointSzie();
    listPointWeighted endState;
    int ctr=0;
    for (Point *item:gloz_l) {
        endState.push_back({item,conf.probGoals[ctr]});
        ctr++;
    }
    listPointWeighted startState;
    startState.push_back({startAdversary,1});
    Policy *pGridPath =new  PathPolicy("SP",maxA, endState, startState, p_sizer,pA1->get_id()
            ,conf.midPos,conf.rRoutes);
    auto *tmp_pointer = dynamic_cast <PathPolicy*>(pGridPath);
    printf("number of state:\t %d",tmp_pointer->getNumberOfState());
    ////////PATH POLICY////////////


    //// init the RTDP algo
    /* If max speed is zero, the explict number of state is in the second place */
    vector<pair<int,int>> list_Q_data;
    list_Q_data.emplace_back(maxB,1);
    list_Q_data.emplace_back(0,tmp_pointer->getNumberOfState());


    //Policy *RTDP = new DeepRTDP("deepRTDP",maxB,rand(),pD2->get_id());
    Policy *RTDP = new RtdpAlgo("RTDP",maxB,g->getSizeIntGrid(),list_Q_data,pD2->get_id());
    RTDP->add_tran(pGridPath);
    pA1->setPolicy(pGridPath);
    pD2->setPolicy(RTDP);

    auto* s = new MdpPlaner();
    s->add_player(pA1);
    s->add_player(pD2);
    s->set_grid(g);
    s->set_state();


    return s;
}


void toCsv(string &pathFile, vector<vector<int>>* infoArr,vector<string> &labels){
    try
    {
        csvfile csv(std::move(pathFile),","); // throws exceptions!
        // Hearer
        size_t sizeLabels=labels.size();
        for (auto &label : labels)
            csv << label;
        csv<<endrow;
        // Data
        for (auto row:*infoArr)
        {
            for (size_t i = 0; i < sizeLabels ; ++i)
                csv << row[i];
            csv<< endrow;
        }
    }
    catch (const std::exception &ex)
    {
        std::cout << "Exception was thrown: " << ex.what() << std::endl;
    }
}

void toCsvString(string pathFile,vector<string>* infoArr){
    try
    {
        csvfile csv(std::move(pathFile),","); // throws exceptions!
        // Hearer
        size_t sizeVec = infoArr->size();
        unsigned int ctr=0;
        int lim = sizeVec-400000;
        // Data
        for (const auto& row:*infoArr)
        {

            if (ctr>lim)
            {
                csv << row;
                csv<< endrow;
            }
            ctr++;
        }
    }
    catch (const std::exception &ex)
    {
        std::cout << "Exception was thrown: " << ex.what() << std::endl;
    }
}

vector<vector<string>> readConfigFile(string &filePath){
    CSVReader reader(std::move(filePath),',');
    vector<vector<string>> rowsCsv = reader.getDataCSV();
//    for (auto &row : rowsCsv )
//    {
//        for (auto item: row)
//        {
//            cout<<item<<',';
//        }
//        cout<<endl;
//    }

    return rowsCsv;
}