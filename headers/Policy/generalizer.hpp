//
// Created by ERANHER on 4/22/20.
//

#ifndef TRACK_RACING_GENERALIZER_HPP
#define TRACK_RACING_GENERALIZER_HPP

#include "util_game.hpp"
#include "util/csvfile.hpp"


class Generalizer{

public:
    Generalizer()= default;
    static void uploadData(const string &pathTable, const string &pathDict);



};

void Generalizer::uploadData(const string &pathTable,const string &pathDict) {
    CSVReader reader(pathTable,';');
    vector<vector<string>> rowsCsvQ = reader.getDataCSV();
    size_t tableSize = rowsCsvQ.size()-1;
    for (auto &item : rowsCsvQ)
    {

    }
dert
}

#endif //TRACK_RACING_GENERALIZER_HPP
