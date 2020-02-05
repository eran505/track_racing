cmake_minimum_required(VERSION 3.15)
project(track_racing)

set(CMAKE_CXX_STANDARD 14)

#add_definitions(-D_GLIBCXX_USE_CXX14_ABI=0)

find_package(Torch REQUIRED)

include_directories(.)
include_directories(headers)
include_directories(headers/Policy)


add_executable(track_racing
        headers/Policy/Dog.hpp
        headers/Policy/Policy.hpp
        headers/Agent.hpp
        headers/Game.hpp
        headers/Grid.hpp
        headers/MdpPlaner.hpp
        headers/State.hpp
        headers/Transition.hpp
        headers/util_game.hpp
        Agent.cpp
        Game.cpp
        Grid.cpp
        main.cpp
        MdpPlaner.cpp
        State.cpp
        util_game.cpp headers/graph/graph_util.hpp headers/Policy/RTDP_util.hpp headers/Policy/RTDP_util.cpp headers/Policy/RtdpAlgo.hpp headers/Policy/RtdpAlgo.cpp headers/graph/graph_dummy.hpp headers/serach/Astar.hpp headers/serach/Astar.cpp headers/Policy/PathPolicy.hpp headers/util/csvfile.hpp headers/util/utilClass.hpp headers/learning/neuralNet.h headers/learning/FeatureGen.h headers/learning/DeepRTDP.h headers/learning/ReplayBuffer/ReplayBuffer.hpp)

target_link_libraries(track_racing "${TORCH_LIBRARIES}")

#-DCMAKE_PREFIX_PATH=/home/ise/programs/torch/libtorch