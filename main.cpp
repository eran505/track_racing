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
#include "Fix/fixSimulation.hpp"
#include <headers/util/csvfile.hpp>
#include "Policy/Attacker/PathFinder.hpp"
#include "MultiAction/SinglePath.hpp"
//#include <torch/script.h> // One-stop header.
#include "MultiAction/Simulator.hpp"
#include "learning/ReplayBuffer/SumTree.hpp"
#include "learning/ReplayBuffer/prioritizedExperienceReplay.hpp"

const char *  getConfigPath(int argc, char** argv);
Grid * init_grid(configGame &conf);
MdpPlaner* init_mdp(Grid *g, configGame &conf);
void toCsv(string &pathFile, vector<vector<int>>* infoArr,vector<string> &labels);
Game* initGame(configGame& conf);
vector<vector<string>> readConfigFile(string &filePath);
void toCsvString(string pathFile,vector<string>* infoArr);
void toCSVTemp(string pathFile, vector<string> &data);
void FixAbstGame(configGame &conf, std::unique_ptr<Agent> policyA,std::unique_ptr<Agent> policyD, State *s,int lev_number);
void getConfigPath(int argc, char** argv,configGame &conf);
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
//using namespace std::chrono;
#include <cassert>
#include <iostream>
#include <string>

#include <string_view>
typedef unsigned long ulong;

int main(int argc, char** argv) {

    GOT_HERE;
    auto dict_argv = parser(argv,argc);
    int seed = 1594198815;//1594198815;
    //seed = 1594198815;//1594198815;
    //seed = int( time(nullptr));
    //torch::manual_seed(seed);// #TODO: un-comment this line when doing deep learning debug
    srand(seed);
    auto arrPAth = splitStr(getExePath(),"/");
    string f = "eran"; string sep = "/";
    string home = join(cut_first_appear(arrPAth,f),sep);
    home = sep+home;
    f = "track_racing";
    string repo = join(cut_first_appear(arrPAth,f),sep);
    string pathCsv;
    pathCsv  = home + "/car_model/config/con21.csv";
    std::string toCsvPath (home+ "/car_model/exp/out/");
    auto csvRows = readConfigFile(pathCsv);
    int ctrId=1;
    vector<string> labels={"ctr_round","ctr_wall","ctr_coll","ctr_at_goal","ctr_open"};

    const std::string exeFilePath ("/"+repo+"/track_racing/bash/clean.sh "+pathCsv);
    try{ system(exeFilePath.c_str() );} catch (int e) {cout<<"try and catch"<<endl;}

    for (int i=1; i<csvRows.size();++i)
    {
        string curToCsv;
        string curToCsvPolciy;
        auto row = csvRows[i];
        // size of Grid

        configGame conf(row,seed);
        //conf.inset_data(parser(argv,argc));
        srand(conf._seed);
        getConfigPath(argc,argv,conf);
        //conf.initRandomNoise(); // inset random noise (-1,1) XY
        conf.home=home;
        cout<<"seed:\t"<<conf._seed<<endl;
        cout<<"levels:\t"<<conf.levelz<<endl;
        string strId=row[0];
        cout<<"ID:\t"<<strId<<endl;

        curToCsv.append(toCsvPath);curToCsv.append("ID_");
        curToCsv.append(strId);curToCsv.append(".csv");

        curToCsvPolciy.append(toCsvPath);curToCsvPolciy.append("ID_");
        curToCsvPolciy.append(strId);curToCsvPolciy.append("_P.csv");



        auto resultsConfigI = initGame(conf);



        //toCsv(curToCsv,resultsConfigI->info,labels);
        //toCsv(curToCsvPolciy,resultsConfigI->guardEval,labels);
        ctrId++;
        //Agent::ctr_object = 0;
        delete (resultsConfigI);


    }


    return 0;
}

Game* initGame(configGame &conf ){
    auto g = init_grid(conf);
    //g->print_vaule();

    auto pPlaner = init_mdp(g,conf);
    return nullptr;
    pPlaner->set_grid(g);

    Game* my_game = new Game(pPlaner);
    //exit(0);
    cout<<"------LOOP GAME!!------"<<endl;

    my_game->startGame(6000000);
    string nameFile="buffer_"+conf.idNumber+".csv";
    //toCsvString(conf.home+"/car_model/exp/buffer/"+nameFile, my_game->buffer);



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


    auto pA1 = std::make_unique<Agent>(listPointAttacker
            ,adversary,0);

    auto pD2 = std::make_unique<Agent>(listPointDefender
            ,gurd,0);


    ////////PATH POLICY///////////
    auto lStartingPointGoal = std::vector<std::pair<vector<Point>,double>>();
    auto gloz_l = g->getAllGoalsData();
    assert(gloz_l.size()==conf.probGoals.size());
    for(int i=0;i<conf.probGoals.size();++i)
    {
        auto& ref_pos = lStartingPointGoal.emplace_back();
        ref_pos.first.emplace_back(gloz_l[i].second);
        ref_pos.second=conf.probGoals[i];
    }

    auto* s = new MdpPlaner(conf._seed);
    s->add_player(pA1.get());
    s->add_player(pD2.get());
    s->set_grid(g);
    s->set_state();

    //////// PATH POLICY ////////////
//    Policy *pGridPath =new  PathPolicy("SP", maxA, lStartingPointGoal, listPointAttacker,
//                                       p_sizer, pA1->get_id()
//            , conf.midPos, conf.home, conf.rRoutes, nullptr);
//    auto *tmp_pointer = dynamic_cast <PathPolicy*>(pGridPath);
//    printf("number of state:\t %d\n",tmp_pointer->getNumberOfState());
//    std::unique_ptr<State> tmp = std::make_unique<State>(State(*s->get_cur_state()));
//
//

    Policy *pGridPath = new PathFinder("PathFinder",maxA,pA1->get_id(),conf.home,
                                       lStartingPointGoal,listPointAttacker,
                                        g->getPointSzie(),conf._seed,conf.rRoutes);


    //////// RTDP POLICY ////////
    //Policy *RTDP = new DeepRTDP("deepRTDP",maxD,rand(),pD2->get_id(), gloz_l.size(),conf.home,0,gameInfo_share);
    Policy *RTDP = new RtdpAlgo(maxD,g->getSizeIntGrid(),pD2->get_id(),conf.home);

    conf.levelz=2;
    int level_num=conf.levelz;

    RTDP->add_tran(pGridPath);
    pA1->setPolicy(pGridPath);
    pD2->setPolicy(RTDP);
    auto *rtdp_ptr = dynamic_cast <RtdpAlgo*>(RTDP);
    rtdp_ptr->init_expder(level_num);
    FixAbstGame(conf,std::move(pA1),std::move(pD2),s->get_cur_state(),level_num);
    return s;
}

void FixAbstGame(configGame &conf, std::unique_ptr<Agent> policyA,std::unique_ptr<Agent> policyD, State *s,int lev_number)
{
    auto single = SinglePath(conf,s,std::move(policyA),std::move(policyD));
    single.main_functopn_genrator();
    //auto sim = SimulationGame(conf, std::move(policyA),std::move(policyD),s);
    //sim.main_loop();
    //exit(0);
}

void toCSVTemp(string pathFile, vector<string> &data)
{
    try
    {
        csvfile csv(std::move(pathFile),";"); // throws exceptions!

        // Data
        for (string &i : data)
            csv << i;
        csv<< endrow;

    }
    catch (const std::exception &ex)
    {
        std::cout << "Exception was thrown: " << ex.what() << std::endl;
    }
}

void toCsv(string &pathFile, vector<vector<int>>* infoArr,vector<string> &labels){
    try
    {
        csvfile csv(std::move(pathFile),","); // throws exceptions!
        // Hearer
        for (auto &label : labels)
            csv << label;
        csv<<endrow;
        // Data
        for (const auto &row:*infoArr)
        {
            for (int i : row)
                csv << i;
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

void getConfigPath(int argc, char** argv,configGame &conf)
{
    if(argc>1)
    {
        u_int32_t seed = std::stoi(string(argv[1]));
        u_int32_t  lev = std::stoi(string(argv[2]));
        conf.levelz = lev;
        conf._seed=seed;
    }

}