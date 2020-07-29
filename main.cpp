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
void FixAbstGame(configGame &conf, Policy* policyA,Policy *policyD, std::vector<weightedPosition>& listPointAttacker,
                 std::vector<weightedPosition>& listPointDef, State *s);
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
    int seed = 1595421605;//1594198815;

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
    pathCsv  = home + "/car_model/config/con32.csv";
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
        conf.inset_data(parser(argv,argc));
        srand(conf._seed);
        //conf.initRandomNoise(); // inset random noise (-1,1) XY
        conf.home=home;
        cout<<"seed:\t"<<conf._seed<<endl;
        cout<<"evla_mode:\t"<<conf.eval_mode<<endl;
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


    auto* pA1 = new Agent(listPointAttacker
            ,adversary,0);

    auto* pD2 = new Agent(listPointDefender
            ,gurd,0);


    ////////PATH POLICY///////////
    auto* lStartingPointGoal = new std::vector<std::pair<double,Point>>();
    auto gloz_l = g->get_goals();
    Point p_sizer = g->getPointSzie();
    int ctr=0;
    for (const auto &item:gloz_l) {
        lStartingPointGoal->push_back({conf.probGoals[ctr], item});
        ctr++;
    }


    auto* s = new MdpPlaner(conf._seed);
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


    //    Point abPoint8(8,8,1);
//    Point abPoint2 = Point(2,2,1);
//    Point abPoint4 = Point(4,4,1);
//    Point abPoint6 = Point(6,6,1);
//    Point abPoint12 = Point(12,12,1);
//    Point abPoint3 = Point(3,3,1);
//
//    tmp_pointer->treeTraversal(tmp.get(),conf.idNumber,&abPoint8);
//    pA1->setPolicy(pGridPath);
//
//    vector<Point> absList = {abPoint4};
//
//
//    for(const auto& absItem: absList)
//    {
//        auto* z = new AbstractCreator(tmp_pointer,conf.sizeGrid,{absItem,abPoint2},conf._seed);
//
//        z->factory_containerAbstract(conf,listPointDefender);
//        auto *rl = new rtSimulation(conf.sizeGrid,pA1,s->get_cur_state(),pD2);
//        rl->setContiner(z->get_con());
//        rl->simulationV2();
//        auto res = rl->getTrackingDataString();
//        res.push_back(conf.idNumber);
//        res.push_back(std::to_string(conf._seed));
//        res.push_back(std::to_string(conf.rRoutes));
//
//        res.push_back(z->get_abstraction_tostring());
//
//        res.push_back(conf.sizeGrid.to_str());
//        res.push_back(conf.gGoals.front().to_str());
//        res.push_back(conf.posAttacker.to_str());
//        res.push_back(conf.posDefender.to_str());
//        for(auto j=0;j<z->get_allAbst_size();++j)
//        {
//            res.push_back(std::to_string(rl->sum_of_coll(j)));
//            res.push_back(rl->collusionMiniGrid_to_string(j));
//        }
//        for (auto &item : z->get_lPolEval())for(auto numL: item)res.push_back(std::to_string(numL));
//        string file_name = std::to_string(conf.eval_mode)+"__new.csv";
//        string path = conf.home+"/car_model/out/"+file_name;
//        toCSVTemp(path, res);
//        delete rl;
//        delete z;
//    }

//    //////// RTDP POLICY ////////
    /* If max speed is zero, the explict number of state is in the second place */
    vector<pair<int,int>> list_Q_data;
    list_Q_data.emplace_back(maxD,1);
    list_Q_data.emplace_back(0,tmp_pointer->getNumberOfState());

    //Policy *RTDP = new DeepRTDP("deepRTDP",maxD,rand(),pD2->get_id(), gloz_l.size(),conf.home,0,gameInfo_share);
    Policy *RTDP = new RtdpAlgo(maxD,g->getSizeIntGrid(),list_Q_data,pD2->get_id(),conf.home,gameInfo_share,5);
    //auto* ab = new abstractionDiv(g->getPointSzie(),Point(5),tmp_pointer);


    RTDP->add_tran(pGridPath);
    pA1->setPolicy(pGridPath);
    pD2->setPolicy(RTDP);
    auto *rtdp_ptr = dynamic_cast <RtdpAlgo*>(RTDP);
    rtdp_ptr->init_expder();
    FixAbstGame(conf,pGridPath,RTDP,listPointAttacker,listPointDefender,s->get_cur_state());
    return s;
}

void FixAbstGame(configGame &conf, Policy* policyA,Policy *policyD, std::vector<weightedPosition>& listPointAttacker,
                 std::vector<weightedPosition>& listPointDef, State *s)
{
    vector<pair<Point,Point>> vec(3);
    vec[0]={Point(2,2,1),Point(4,4,1)};
    vec[1]={Point(2,2,1),Point(2,2,1)};
    vec[2]={Point(1,1,1),Point(1,1,1)};
//    vec[0]={Point(4,4,1),Point(4,4,1)};
//    vec[1]={Point(4,4,1),Point(2,2,1)};
//    vec[2]={Point(1,1,1),Point(1,1,1)};



    auto sim = SimulationGame(conf, policyA,policyD,
            listPointAttacker, listPointDef,s,2);
    sim.main_loop();
    exit(0);
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
        int i = std::stoi(string(argv[1]));
        conf.abst = Point(i,i,1);
    }

}