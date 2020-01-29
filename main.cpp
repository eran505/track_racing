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
Grid * init_grid(Point &pSize);
MdpPlaner* init_mdp(Grid *g, ulong numPaths,float p);
void toCsv(string pathFile, vector<vector<int>>* infoArr,vector<string> &labels);
vector<vector<int>>* initGame(int sizeInt, ulong intSizePath, float p);
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
#include <random>
#include <headers/util/csvfile.hpp>
typedef vector<tuple<Point*,double>> listPointWeighted;
typedef unsigned long ulong;
int main() {

//    auto nn = new neuralNet();
//    nn->start();
//
//    exit(0);
    // seeding the program
    int seed = int( time(nullptr));
    cout<<"seed:\t"<<seed<<endl;
    srand(seed);
    int MaxInt = INT_MAX;


    vector<vector<int>> all_info;
    for (int sizeG = 7; sizeG < 8; ++sizeG)
    {
        for (ulong i =4; i <= 20; i+=50) {

            for (float prob =0.1 ; prob <=1.0 ; prob+=2) {

                std::chrono::steady_clock::time_point begin2 = std::chrono::steady_clock::now();
                auto info = initGame(sizeG,i,prob);
                std::chrono::steady_clock::time_point end2 = std::chrono::steady_clock::now();
                std::cout << " " << std::chrono::duration_cast<std::chrono::minutes> (end2 - begin2).count() << "[m]:" ;
                std::cout << " " << std::chrono::duration_cast<std::chrono::milliseconds> (end2 - begin2).count() << "[ml]:" ;
                std::cout << " " << std::chrono::duration_cast<std::chrono::microseconds>(end2 - begin2).count() << "[µs]:" ;
                std::cout << " " << std::chrono::duration_cast<std::chrono::nanoseconds> (end2 - begin2).count() << "[ns] "<< std::endl ;
                string timeMin =  std::to_string(int(std::chrono::duration_cast<std::chrono::minutes> (end2 - begin2).count()));
                string timeMl =  std::to_string(int(std::chrono::duration_cast<std::chrono::milliseconds> (end2 - begin2).count()));
                for (auto &item: *info)
                {
                    item.push_back(prob*10);
                    item.push_back(i);
                    item.push_back(sizeG);
                    item.push_back(int(std::chrono::duration_cast<std::chrono::milliseconds> (end2 - begin2).count()));
                    all_info.push_back(item);
                }

            }

        }
    }

    string fileName="all.csv";
    //csv
    std::vector<string> v(8);
    v = { "Index","Wall","Coll","At_Gaol","p","MaxPath","sizeGrid","time (ml)"};
    toCsv("/home/ERANHER/car_model/exp/"+fileName,&all_info,v);
    return 0;
}

vector<vector<int>>* initGame(int sizeGrid, ulong numPaths,float p ){
    Point ppGridSize(sizeGrid,sizeGrid,sizeGrid);
    auto g= init_grid(ppGridSize);
    //g->print_vaule();

    auto pPlaner = init_mdp(g,numPaths,p);

    pPlaner->set_grid(g);

    Game* my_game = new Game(pPlaner);
    cout<<"------LOOP GAME!!------"<<endl;

    auto info = my_game->startGame(400000);

    toCsvString("/home/ise/car_model/exp/buffer/buffer.csv", my_game->buffer);


    delete(my_game);
    //delete (info);
    cout<<"------END MAIN!!------"<<endl;
    return info;
}

Grid * init_grid(Point &pSize){
    int maxSzieGrid = pSize.array[0];
    game_params m{};
    auto m_ofList = new list<Point *>;
    //m_ofList->push_front(new Point(1,6,3));
    m_ofList->push_front(new Point(maxSzieGrid-1,maxSzieGrid-1,maxSzieGrid/2));
    m_ofList->push_front(new Point(maxSzieGrid-1,maxSzieGrid-1,2));
    //m_ofList->push_front(new Point(3,4,0));
    m.size=pSize;
    m.list_goals=m_ofList;
    Grid *g = new Grid(m);
    return g;
}
MdpPlaner* init_mdp(Grid *g, ulong numPaths,float p){
    int maxSizeGrid = g->getPointSzie().array[0];
    int maxA=2+maxSizeGrid/10;
    int maxB=1+maxSizeGrid/10;
    auto startAdversary = new Point(0,0,0);
    auto* a1 = new Agent(startAdversary
            ,new Point(0,0,maxA)
            ,adversary,10);

    auto* b2 = new Agent(new Point(maxSizeGrid-2,maxSizeGrid-2,0),
            new Point(0,0,maxB)
            ,gurd,10);


    ////////PATH POLICY///////////
    auto gloz_l = g->get_goals();
    Point p_sizer = g->getPointSzie();
    listPointWeighted endState;
    gloz_l.size() == 2 ? p=p: p=1.0;
    auto probabllitiesW = vector<float>();
    probabllitiesW.push_back(p);
    probabllitiesW.push_back(1.0-p);
    int ctr=0;
    for (Point *item:gloz_l) {
        endState.push_back({new Point(*item),probabllitiesW[ctr]});
        ctr++;
    }
    listPointWeighted startState;
    startState.push_back({startAdversary,1});
    Policy *pGridPath =new  PathPolicy("SP",maxA, endState, startState, p_sizer,a1->get_id(),numPaths);
    auto *tmp_pointer = dynamic_cast <PathPolicy*>(pGridPath);
    printf("number of state:\t %d",tmp_pointer->getNumberOfState());
    ////////PATH POLICY////////////


    //// init the RTDP algo
    /* If max speed is zero, the explict number of state is in the second place */
    list<pair<int,int>> l;
    l.emplace_back(maxB,1);
    l.emplace_back(0,tmp_pointer->getNumberOfState());


    //Policy *RTDP = new DeepRTDP("deepRTDP",maxB,rand(),b2->get_id());
    Policy *RTDP = new RtdpAlgo("RTDP",maxB,g->getSizeIntGrid(),l,b2->get_id());
    RTDP->add_tran(pGridPath);
    a1->setPolicy(pGridPath);
    b2->setPolicy(RTDP);

    auto* s = new MdpPlaner();
    s->add_player(a1);
    s->add_player(b2);
    s->set_grid(g);
    s->set_state();


    return s;
}


void toCsv(string pathFile, vector<vector<int>>* infoArr,vector<string> &labels){
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

        // Data
        for (const auto& row:*infoArr)
        {
            csv << row;
            csv<< endrow;
        }
    }
    catch (const std::exception &ex)
    {
        std::cout << "Exception was thrown: " << ex.what() << std::endl;
    }
}