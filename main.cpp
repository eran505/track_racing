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
Grid * init_grid(Point &pSize);
MdpPlaner* init_mdp(Grid *g);
void toCsv(string pathFile, vector<int*>* infoArr);
void initGame(int sizeInt);

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

int main() {
    // seeding the program
    int seed = int( time(nullptr));
    cout<<"seed:\t"<<seed<<endl;
    srand(seed);
    Point pSize(7,7,7);

    ////
//    Point p_sizer = pSize;
//    vector<Point*> endState;
//    endState.push_back(new Point(5,5,0));
//    vector<Point*> startState;
//    startState.push_back(new Point(0,0,0));
//    Policy *pGridPath =new  PathPolicy("SP",1, endState, startState, p_sizer);
//    delete(pGridPath);
//    exit(0);
    ////
    for (int p = 5; p < 6; ++p)
    {
        initGame(p);

    }
    return 0;

}

void initGame(int sizeGrid){
    Point ppGridSize(sizeGrid,sizeGrid,sizeGrid);
    auto g= init_grid(ppGridSize);
    //g->print_vaule();

    auto pPlaner = init_mdp(g);

    pPlaner->set_grid(g);
    pPlaner->get_player("B4") == nullptr ? cout << "ya\n" : cout << "no\n";


    Game* my_game = new Game(pPlaner);
    cout<<"------LOOP GAME!!------"<<endl;

    std::chrono::steady_clock::time_point begin2 = std::chrono::steady_clock::now();
    auto info = my_game->startGame(10);
    std::chrono::steady_clock::time_point end2 = std::chrono::steady_clock::now();
    std::cout << " " << std::chrono::duration_cast<std::chrono::minutes> (end2 - begin2).count() << "[m]:" ;
    std::cout << " " << std::chrono::duration_cast<std::chrono::milliseconds> (end2 - begin2).count() << "[ml]:" ;
    std::cout << " " << std::chrono::duration_cast<std::chrono::microseconds>(end2 - begin2).count() << "[µs]:" ;
    std::cout << " " << std::chrono::duration_cast<std::chrono::nanoseconds> (end2 - begin2).count() << "[ns] "<< std::endl ;
    string timeMin =  std::to_string(int(std::chrono::duration_cast<std::chrono::minutes> (end2 - begin2).count()));
    string timeMl =  std::to_string(int(std::chrono::duration_cast<std::chrono::milliseconds> (end2 - begin2).count()));
    auto fileName="raceExp_s_"+std::to_string(sizeGrid)+"_m_"+timeMin+"_ml_"+timeMl+".csv";
    //toCsv("/home/ise/car_model/exp/"+fileName,info);
    delete(my_game);
    delete (info);
    cout<<"------END MAIN!!------"<<endl;
}

Grid * init_grid(Point &pSize){
    int maxSzieGrid = pSize.array[0];
    game_params m{};
    auto m_ofList = new list<Point *>;
    //m_ofList->push_front(new Point(1,6,3));
    m_ofList->push_front(new Point(maxSzieGrid-1,maxSzieGrid-1,maxSzieGrid-1));
    m_ofList->push_front(new Point(maxSzieGrid-1,maxSzieGrid-3,maxSzieGrid-2));
    //m_ofList->push_front(new Point(3,4,0));
    m.size=pSize;
    m.list_goals=m_ofList;
    Grid *g = new Grid(m);
    return g;
}
MdpPlaner* init_mdp(Grid *g){
    int maxSizeGrid = g->getPointSzie().array[0];
    int maxA=2+maxSizeGrid/10;
    int maxB=1+maxSizeGrid/10;
    auto* a1 = new Agent(new Point(0,0,0)
            ,new Point(0,0,0)
            ,adversary,10);

//    auto* a2 = new Agent(new Point(3,4),
//            new Point(0,0)
//            ,adversary,10);

    auto* b2 = new Agent(new Point(maxSizeGrid-2,maxSizeGrid-2,maxSizeGrid-2),
            new Point(0,0,0)
            ,gurd,10);

//    Policy *p_polcy_a1 = new Dog("Dog",maxA);
//    Dog *d = dynamic_cast <Dog*>(p_polcy_a1);
//    for (Point  *item : gloz_l)
//        d->set_goal(*item);

    ///////////////////
    auto gloz_l = g->get_goals();
    Point p_sizer = g->getPointSzie();
    listPointWeighted endState;
    for (Point *item:gloz_l) {
        endState.push_back({new Point(*item),1.0/double(gloz_l.size())});
    }
    listPointWeighted startState;
    startState.push_back({new Point(0,0,0),1});
    Policy *pGridPath =new  PathPolicy("SP",maxA, endState, startState, p_sizer);
    auto *tmp_pointer = dynamic_cast <PathPolicy*>(pGridPath);
    printf("number of state:\t %d",tmp_pointer->getNumberOfState());

    ////////////////////

//    Policy *p_polcy_a2 = new Dog("Dog",1);
//    Dog *d2 = dynamic_cast <Dog*>(p_polcy_a2);
//    d2->set_goal(0,0);
//    d2->set_goal(1,0);

    //// init the RTDP algo
    list<pair<int,int>> l;
    l.emplace_back(maxB,1);
    l.emplace_back(0,tmp_pointer->getNumberOfState());

    Policy *RTDP = new RtdpAlgo("RTDP",maxB,g->getSizeIntGrid(),l);
    RTDP->add_tran(pGridPath);
//    RTDP->add_tran(p_polcy_a2);


    //p_polcy_a1->add_tran(p_polcy_a1);
    //p_polcy_a2->add_tran(p_polcy_a2);
    a1->setPolicy(pGridPath);
    b2->setPolicy(RTDP);
//    a2->setPolicy(p_polcy_a2);

    auto* s = new MdpPlaner();
    s->add_player(a1);
//  s->add_player(a2);
    s->add_player(b2);
    s->set_grid(g);
    s->set_state();


    return s;
}


void toCsv(string pathFile, vector<int*>* infoArr){
    try
    {
        csvfile csv(std::move(pathFile),","); // throws exceptions!
        // Hearer
        csv << "Iter" << "Wall"<<"Coll"<<"AtGoal" << endrow;
        // Data
        for (auto row:*infoArr)
        {
            csv << row[0] << row[1]<<row[2]<<row[3]<< endrow;
        }
    }
    catch (const std::exception &ex)
    {
        std::cout << "Exception was thrown: " << ex.what() << std::endl;
    }
}