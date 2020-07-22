//
// Created by ERANHER on 22.7.2020.
//

#ifndef TRACK_RACING_SAVER_HPP
#define TRACK_RACING_SAVER_HPP
#include <utility>

#include "csvfile.hpp"

template<typename H>
class Saver{
    string split=";";
    u_int32_t counter=0;
    string file_path;
    u_int32_t MAX_buffer=-1;
    vector<H> header;
    csvfile csver;
    bool is_capacity=false;

public:
    explicit Saver(string&& file_name_path,std::vector<H>&& header_name ,u_int write_max_buffer=-1):
    file_path(file_name_path)
    ,MAX_buffer(write_max_buffer)
    ,header(header_name)
    ,csver(file_path,split)
    {
        if(MAX_buffer>0)
            is_capacity=true;
        insert_header();
    }
    explicit Saver(const string &file_name_path,u_int32_t max_buff)
    :file_path(file_name_path),MAX_buffer(max_buff),
    csver(file_name_path,split)
    {}

    void set_header(const vector<H> &vec)
    {
        header=vec;
        insert_header();
    }

    template<typename T>
    void inset_data(const T& vec_data)
    {
        insert_data(vec_data);
    }
    void inset_endLine(){
        csver<<endrow;
        counter++;
        is_flush();
    }
private:
    void insert_header()
    {
        for(auto &item:header)
            csver<<item;
        csver<<endrow;
    }
    template<typename T>
    void insert_data(const T &d)
    {
        for(auto &item:d) csver<<item;

    }
    void is_flush()
    {
        if(counter>MAX_buffer)
        {
            counter=0;
            csver.flush();
            //insert_header();
        }
    }



};

#endif //TRACK_RACING_SAVER_HPP
