#include <iostream>
#include "headers/Grid.hpp"
#include "headers/Agent.hpp"
#include <list>
#include "headers/util_game.hpp"
#include "headers/MdpPlaner.hpp"
#include "Abstract/RealTimeSimulation.hpp"
#include "headers/Game.hpp"
#include "headers/Policy/Dog.hpp"
#include "headers/graph/graph_util.hpp"
#include <memory>
#include <utility>
#include "Abstract/abstractionDiv.h"
#include <vector>
#include<algorithm>
#include "Abstract/FactoryAgent.hpp"
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

#include "learning/ReplayBuffer/SumTree.hpp"
#include "learning/ReplayBuffer/prioritizedExperienceReplay.hpp"
#define GOT_HERE std::cout << "At " __FILE__ ": " << __LINE__ << std::endl

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
using namespace std::chrono;
#include <cassert>
#include <iostream>
#include <string>

#include <string_view>
typedef unsigned long ulong;

int main() {
    GOT_HERE;
    int seed = 155139;
    //seed = 1587982523; //1895975606
    seed = int( time(nullptr));
    cout<<"seed:\t"<<seed<<endl;
    torch::manual_seed(seed);// #TODO: un-comment this line when doing deep learning debug
    srand(seed);
    auto arrPAth = splitStr(getExePath(),"/");
    string home = "/"+arrPAth[0]+"/"+arrPAth[1];
    string repo = "/"+arrPAth[0]+"/"+arrPAth[1]+"/"+arrPAth[2]+"/"+arrPAth[3]+"/"+arrPAth[4];
    int MaxInt = INT_MAX;
    //const string home="/home/ise";
    std::string pathCsv (home + "/car_model/config/con3.csv");
    std::string toCsvPath (home+ "/car_model/exp/out/");
    auto csvRows = readConfigFile(pathCsv);
    int ctrId=1;
    vector<string> labels={"ctr_round","ctr_wall","ctr_coll","ctr_at_goal","ctr_open"};

    const std::string exeFilePath (repo+"/bash/clean.sh "+pathCsv);

    system(exeFilePath.c_str() );

    for (int i=1; i<csvRows.size();++i)
    {
        cout << "  rand(): " << rand() << '\n';
        cout<<"seed:\t"<<seed<<endl;
        string curToCsv;
        string curToCsvPolciy;
        auto row = csvRows[i];
        // size of Grid
        configGame conf(row);
        conf.home=home;
        conf.seed=seed;
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

    my_game->startGame(5000000);
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
    auto listGoal =vector<Point>();
    for (auto &refGoal:conf.gGoals)
        listGoal.push_back(std::move(refGoal));
    m.list_goals=listGoal;
    Grid *g = new Grid(m);
    g->setTargetGoals(conf.goalTarget);
    return g;

}
MdpPlaner* init_mdp(Grid *g, configGame &conf){
    int maxSizeGrid = g->getPointSzie().array[0];
    int maxA=2;   //TODO:: change it to plus one !!!!!!!!!!!!!!!!!!!!!!!
    int maxD=1;
    conf.maxD=maxD;
    conf.maxA=maxA;

    // make game info
    shared_ptr<unordered_map<string,string>> gameInfo_share = std::make_shared<unordered_map<string,string>>();
    auto [it, result] = gameInfo_share->emplace("ID",conf.idNumber);
    assert(result);
    //auto gameInfo = new unordered_map<string,string>();
    //gameInfo->insert({"ID",conf.idNumber});


    std::vector<weightedPosition> listPointAttacker;
    std::vector<weightedPosition> listPointDefender;
    listPointAttacker.emplace_back(Point(0,0,0),std::move(conf.posAttacker),1.0);
    listPointDefender.emplace_back(Point(0,0,0),std::move(conf.posDefender),1.0);


    auto* pA1 = new Agent(listPointAttacker
            ,adversary,10);

    auto* pD2 = new Agent(listPointDefender
            ,gurd,10);


    ////////PATH POLICY///////////
    auto* lStartingPointGoal = new std::vector<std::pair<float,Point>>();
    auto gloz_l = g->get_goals();
    Point p_sizer = g->getPointSzie();
    int ctr=0;
    for (const auto &item:gloz_l) {
        lStartingPointGoal->push_back({conf.probGoals[ctr], item});
        ctr++;
    }


    auto* s = new MdpPlaner(conf.seed);
    s->add_player(pA1);
    s->add_player(pD2);
    s->set_grid(g);
    s->set_state();

    //////// PATH POLICY ////////////
    Policy *pGridPath =new  PathPolicy("SP", maxA, lStartingPointGoal, listPointAttacker,
                                       p_sizer, pA1->get_id()
            , conf.midPos, conf.home, conf.rRoutes, nullptr);
    auto *tmp_pointer = dynamic_cast <PathPolicy*>(pGridPath);
    printf("number of state:\t %d\n",tmp_pointer->getNumberOfState());
    std::unique_ptr<State> tmp = std::make_unique<State>(State(*s->get_cur_state()));
    Point abPoint(7,7,1);
    tmp_pointer->treeTraversal(tmp.get(),conf.idNumber,&abPoint);
    pA1->setPolicy(pGridPath);

    auto* z = new AbstractCreator(tmp_pointer,conf.sizeGrid,abPoint,conf.seed);
    z->initializeSimulation(conf,listPointDefender);
    auto *rl = new rtSimulation(abPoint,conf.sizeGrid,z->getLAgents(),pA1,s->get_cur_state(),pD2);
    rl->simulation();
    //////// RTDP POLICY ////////
    /* If max speed is zero, the explict number of state is in the second place */
//    vector<pair<int,int>> list_Q_data;
//    list_Q_data.emplace_back(maxD,1);
//    list_Q_data.emplace_back(0,tmp_pointer->getNumberOfState());
//
//    //Policy *RTDP = new DeepRTDP("deepRTDP",maxD,rand(),pD2->get_id(), gloz_l.size(),conf.home,0,gameInfo_share);
//    Policy *RTDP = new RtdpAlgo("RTDP",maxD,g->getSizeIntGrid(),list_Q_data,pD2->get_id(),conf.home,gameInfo_share);
//    //auto* ab = new abstractionDiv(g->getPointSzie(),Point(5),tmp_pointer);
//
//    RTDP->add_tran(pGridPath);
//    pA1->setPolicy(pGridPath);
//    pD2->setPolicy(RTDP);


    exit(0);

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
        for (const auto &row:*infoArr)
        {
            for (size_t i = 0; i < row.size() ; ++i)
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
        const int upper = 500000;
        size_t sizeVec = infoArr->size();
        unsigned int ctr=0;
        unsigned int lim=0;
        //if (sizeVec > upper) lim = sizeVec-400000;
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
    CSVReader reader(filePath,',');
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