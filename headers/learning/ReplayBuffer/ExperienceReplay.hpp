//
// Created by ERANHER on 11.3.2020.
//

#ifndef TRACK_RACING_EXPERIENCEREPLAY_HPP
#define TRACK_RACING_EXPERIENCEREPLAY_HPP

#include "SumTree.hpp"

class ExperienceReplay{

    vector<experienceTuple*> memo;
    bool allowDuplicatesInBatchSampling;
    ExperienceReplay(unsigned int size, bool allow_duplicates_in_batch_sampling):allowDuplicatesInBatchSampling(allow_duplicates_in_batch_sampling){
        this->memo.reserve(size);

    }
};

#endif //TRACK_RACING_EXPERIENCEREPLAY_HPP
