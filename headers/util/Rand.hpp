//
// Created by ERANHER on 30.7.2020.
//

#ifndef TRACK_RACING_RAND_HPP
#define TRACK_RACING_RAND_HPP

#include <random>

struct Randomizer{
public:
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution;
    explicit Randomizer(int seed):
            generator(seed),
            distribution(0.0,1.0)
    {

    }
    double get_double(){return this->distribution(this->generator);}
};

#endif //TRACK_RACING_RAND_HPP
