//
// Created by eranhe on 02/08/2020.
//

#ifndef TRACK_RACING_NORMALIZER_H
#define TRACK_RACING_NORMALIZER_H

#include "util_game.hpp"


class Normalizer{


public:

    void getDict(unordered_map<u_int64_t,unordered_map<uint,double>> &map,unordered_map<u_int64_t, vector<double>*>* mapStateAction,const double weight) {

        for(const auto &item: map)
        {
//            if(item.first==2359856)
//                cout<<"";
            int sumAll = accumulate( item.second.begin(), item.second.end(), 0,
                                     []( int acc, std::pair<int, int> p ) { return ( acc + p.second ); } );

            auto pos_tmp = mapStateAction->find(item.first);
            if (pos_tmp==mapStateAction->end())
            {
                auto *vec = new vector<double>();
                for (auto mapItem: item.second) {
                    int tmp = mapItem.first;
                    int tmp2 = mapItem.second;
                    vec->push_back(tmp);
                    vec->push_back(double(tmp2)/double(sumAll)*weight);
                }
                mapStateAction->insert({item.first,vec});
            }
            else
            {
                for (auto mapItem: item.second) {
                    //cout<<"in"<<endl;
                    int tmp = mapItem.first;
                    int tmp2 = mapItem.second;
                    inset_to_vector_double(pos_tmp->second,tmp,double(tmp2)/double(sumAll)*weight);
                    //pos_tmp->second->push_back(tmp);
                    //pos_tmp->second->push_back(double(tmp2)/double(sumAll)*weight);
                }
            }
        }
    }

    static void inset_to_vector_double(vector<double>* l,double id_action,double p)
    {
        for(size_t j=0;j<l->size();j+=2)
        {
            if(l->operator[](j)==id_action)
            {
                l->operator[](j+1)+=p;
                return;
            }

        }
        l->push_back(id_action);
        l->push_back(p);
    }


    template< typename  T>
    void normalizeDict(T map){
        for(auto &item : map)
        {
            const size_t vecSize = item.second->size();
            double sumProbability=0;
            int indx=0;
            for (auto sec_Item : *item.second) {
                if (indx%2==1)
                    sumProbability+=sec_Item;
                indx++;
            }

            if (sumProbability>1.0) {
                cout<<"";
            }


            if (sumProbability==1.0) {
                continue;
            }

            if (sumProbability==0) {
                continue;
            }

            for(indx=1; indx<vecSize;indx=indx+2)
            {
                item.second->operator[](indx)=item.second->operator[](indx)/sumProbability;
            }

        }
    }
    static vector<double> minizTrans(const vector<double> *x) {
        vector<double> newVector;
        unordered_map<double,double> d;
        for (int i = 0; i <x->size() ; ++i) {
            auto keyH = x->operator[](i);
            auto prob = x->operator[](++i);
            auto pos = d.find(keyH);
            if (pos==d.end())
                d.insert({keyH,prob});
            else
                d[keyH]+=prob;
        }
        for (auto item : d) {
            newVector.push_back(item.first);
            newVector.push_back(item.second);
        }
        return newVector;
    }

};
#endif //TRACK_RACING_NORMALIZER_H
