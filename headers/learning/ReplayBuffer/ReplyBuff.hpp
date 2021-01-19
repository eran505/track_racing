//
// Created by eranhe on 1/11/21.
//

#ifndef TRACK_RACING_REPLYBUFF_HPP
#define TRACK_RACING_REPLYBUFF_HPP
#include <torch/torch.h>
#include <memory>
#include <vector>
#include <iostream>

#include "Featuerzer.hpp"
#include <algorithm>
#include <iterator>
#include <random>

class ExperienceReplay{

private:
    int64_t capacity;
    int64_t indexer=0;
    std::random_device rd;
public:
    std::deque<elementItem> buffer;
    std::unordered_map<double,int32_t> index_maping;
public:
    explicit ExperienceReplay (int64_t size);
    void push(elementItem && item);
    [[nodiscard]] int64_t size_buffer() const;
    auto sample_queue(int64_t batch_size);
    bool inset_to_dict(double key);

};

ExperienceReplay::ExperienceReplay(int64_t size) {

    capacity = size;
}

void ExperienceReplay::push(elementItem && item){
    auto ky = item.hash;
    if (!inset_to_dict(ky)) return;

    //cout<<"[buffer size]"<<buffer.size()<<endl;

    if (buffer.size() < capacity){
        buffer.push_back(std::move(item));
    }
    else {
        while (buffer.size() >= capacity) {
            buffer.pop_front();
        }
        buffer.push_back(std::move(item));
    }
}

auto ExperienceReplay::sample_queue(int64_t batch_size){
    if(batch_size>buffer.size())
        batch_size=buffer.size();
    std::vector<experienceTensor> b;
    b.reserve(batch_size);

    std::mt19937 g(rd());
    std::shuffle(buffer.begin(),buffer.end(),g);

    for(int i=0;i<batch_size;++i)
        b.push_back(buffer[i].exp);

    return b;
}

int64_t ExperienceReplay::size_buffer() const{

    return buffer.size();
}

bool ExperienceReplay::inset_to_dict(double key) {

    if (auto pos = index_maping.find(key);pos==index_maping.end())
    {
        index_maping.try_emplace(key,indexer++);
        return true;
    }
    return false;
}


#endif //TRACK_RACING_REPLYBUFF_HPP
