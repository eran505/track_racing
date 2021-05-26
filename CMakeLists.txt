cmake_minimum_required(VERSION 3.15)
project(track_racing)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_VERBOSE_MAKEFILE ON)
#add_definitions(-D_GLIBCXX_USE_CXX14_ABI=0)
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-3")
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -gdwarf-3")

#set(CMAKE_CXX_FLAGS_DEFAULT "-O3 -g")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")
#set(CMAKE_CXX_FLAGS_DEBUG  "-O3 -g")


#debug mode
#set(CMAKE_CXX_FLAGS -DDEBUG)
set(CMAKE_CXX_FLAGS "-Wall " )

list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/programs/torch/libtorch")

#find_package(Torch REQUIRED)

include_directories(.)
include_directories(headers)
include_directories(headers/Policy)

#set(CMAKE_VERBOSE_MAKEFILE ON)
add_executable(track_racing
        headers/Policy/Dog.hpp
        headers/Policy/Policy.hpp
        headers/Agent.hpp
        headers/Grid.hpp
        headers/State.hpp
        headers/Transition.hpp
        headers/util_game.hpp
        Agent.cpp
        Grid.cpp
        main.cpp
        State.cpp
        util_game.cpp headers/graph/graph_util.hpp headers/Policy/RTDP_util.hpp headers/Policy/RTDP_util.cpp headers/Policy/RtdpAlgo.hpp headers/Policy/RtdpAlgo.cpp headers/graph/graph_dummy.hpp headers/serach/Astar.hpp headers/serach/Astar.cpp headers/Policy/PathPolicy.hpp headers/util/csvfile.hpp headers/util/utilClass.hpp headers/learning/neuralNet.h headers/learning/FeatureGen.h headers/learning/DeepRTDP.h headers/Policy/valueIter.h headers/learning/Learner.h headers/learning/ReplayBuffer/prioritizedExperienceReplay.hpp headers/util/Lookhead.h headers/learning/MCTS/UCT.hpp headers/Abstract/abstractionDiv.h headers/Abstract/AlgoRtdp.h headers/Trees/Tree.hpp "headers/Abstract/Simulation.hpp" headers/Abstract/FactoryAgent.hpp headers/Abstract/RealTimeSimulation.hpp headers/util/Configer.h headers/Abstract/containerAbstract.h headers/Fix/fixManager.hpp headers/Fix/fixSimulation.hpp headers/Fix/AbsAgnet.hpp headers/Fix/containerFix.hpp headers/Fix/fixAbstractLevel.hpp headers/Fix/Grid_Abs.h headers/Fix/Grid_transition.h headers/util/saver.hpp headers/Policy/Update_RTDP/Vanilla.hpp headers/Policy/Update_RTDP/Evaluator.hpp headers/Policy/Update_RTDP/Reward.hpp headers/Policy/Update_RTDP/RollBackUpdate.hpp headers/MultiAction/Scheduler.hpp headers/Policy/Update_RTDP/ActionExpnder.hpp headers/MultiAction/MultiActionAgent.hpp headers/MultiAction/Simulator.hpp headers/MultiAction/EvaluatorActionizer.hpp headers/MultiAction/StackAction.hpp headers/Policy/Attacker/PathFinder.hpp headers/Policy/Attacker/ABpathFinder.hpp headers/util/Rand.hpp headers/Policy/Attacker/normalizer.h headers/MultiAction/SinglePath.hpp headers/Policy/Attacker/PathGenrator.hpp headers/Policy/Update_RTDP/PathMapper.hpp headers/Policy/Attacker/StaticPolicy.hpp headers/learning/ReplayBuffer/Featuerzer.hpp headers/learning/Networks/Noisy.hpp headers/learning/Networks/DQN.hpp headers/learning/Trainer.hpp headers/learning/Networks/FF_net.hpp headers/learning/ReplayBuffer/ReplyBuff.hpp headers/learning/ReplayBuffer/TreeSum.hpp headers/GR/GoalRec.hpp headers/GR/PRecAgent.hpp)

#target_link_libraries(track_racing "${TORCH_LIBRARIES}")

#set(SOURCE_FILES main.cpp)

# cmake -DCMAKE_PREFIX_PATH=/home/ise/programs/torch/libtorch
# cmake --build . --config Release