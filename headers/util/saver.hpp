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
    u_int ctr_indx = 0;
    u_int32_t MAX_buffer=-1;
    vector<H> header;
    vector<H> body;
    csvfile csver;
    bool is_capacity=false;
    std::function<void (H &item)> writeData=[&](H &item){csver<<item;};

public:
    void set_write_lamda(std::function<void (H &item)> &write_data){this->writeData=write_data;}
    explicit Saver(string&& file_name_path,std::vector<H>&& header_name ,u_int write_max_buffer=-1):
    file_path(file_name_path)
    ,MAX_buffer(write_max_buffer)
    ,header(header_name)
    ,csver(file_path,split)
    {

        if(MAX_buffer>0)
        {
            is_capacity=true;
            body= std::vector<H>(MAX_buffer);
        }
        insert_header();
    }
    explicit Saver(const string &file_name_path,u_int32_t max_buff)
    :file_path(file_name_path),MAX_buffer(max_buff),
    csver(file_name_path,split)
    {
        if(MAX_buffer>0)
        {
            body= std::vector<H>(MAX_buffer);
        }
    }
    ~Saver()
    {
        if(body.empty()) return;
        int i=0;
        if(ctr_indx<this->MAX_buffer/3){
            i=MAX_buffer/3;
            ctr_indx=MAX_buffer;
        }
        for(auto &item:body)
        {
            if(i>=ctr_indx)
                break;
            writeData(item);
            csver<<endrow;
            i++;
        }
        csver.flush();
    }
    void set_header_vec(const vector<H> &vec)
    {
        insert_header(vec);
    }

    template<typename V>
    void inset_one_item(const V &item)
    {
        csver<<item;
    }
    template<typename V>
    void inset_one_item_endLine(const V &item)
    {
        csver<<item;
        inset_endLine();
    }
    void save_string_body(H &&val)
    {
        body[ctr_indx]=val;
        ctr_indx++;

        if(ctr_indx>=MAX_buffer){
            ctr_indx=0;
        }
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
    void insert_header(const vector<H> &vec)
    {
        for(auto &item:vec)
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
