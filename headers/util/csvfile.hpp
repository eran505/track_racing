//
// Created by ise on 1.1.2020.
//

#ifndef TRACK_RACING_CSVFILE_HPP
#define TRACK_RACING_CSVFILE_HPP
#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "util_game.hpp"
#include <filesystem>
class csvfile;

inline static csvfile& endrow(csvfile& file);
inline static csvfile& flush(csvfile& file);

class csvfile
{
    std::ofstream fs_;
    bool is_first_;
    const std::string separator_;
    const std::string escape_seq_;
    const std::string special_chars_;
public:

    explicit csvfile(const std::string filename, const std::string separator = ";",bool isApp= false)
            : fs_()
            , is_first_(true)
            , separator_(separator)
            , escape_seq_("\"")
            , special_chars_("\"")
    {
        delete_existing_file(filename);
        fs_.exceptions(std::ios::failbit | std::ios::badbit);
        if (!isApp) fs_.open(filename,std::ios_base::app | ios_base::out);
        else fs_.open(filename,std::ios_base::app | ios_base::out);
    }

    ~csvfile()
    {
        flush();
        fs_.close();
    }

    void flush()
    {
        fs_.flush();
    }

    void endrow()
    {
        fs_ << std::endl;
        is_first_ = true;
    }

    csvfile& operator << ( csvfile& (* val)(csvfile&))
    {
        return val(*this);
    }

    csvfile& operator << (const char * val)
    {
        return write(escape(val));
    }

    csvfile& operator << (const std::string & val)
    {
        return write(escape(val));
    }

    template<typename T>
    csvfile& operator << (const T& val)
    {
        return write(val);
    }

private:
    static void delete_existing_file(const string &file_path)
    {
        cout<<"in"<<endl;
        try {
            if (std::filesystem::remove(file_path))
                std::cout << "file " << file_path << " deleted.\n";
            else
                std::cout << "file " << file_path << " not found.\n";
        }
        catch(const std::filesystem::filesystem_error& err) {
            std::cout << "filesystem error: " << err.what() << '\n';
        }
    }
    template<typename T>
    csvfile& write (const T& val)
    {
        if (!is_first_)
        {
            fs_ << separator_;
        }
        else
        {
            is_first_ = false;
        }
        fs_ << val;
        return *this;
    }

    std::string escape(const std::string & val)
    {
        std::ostringstream result;
        result << '"';
        std::string::size_type to, from = 0u, len = val.length();
        while (from < len &&
               std::string::npos != (to = val.find_first_of(special_chars_, from)))
        {
            result << val.substr(from, to - from) << escape_seq_ << val[to];
            from = to + 1;
        }
        result << val.substr(from) << '"';
        return result.str();
    }
};


inline static csvfile& endrow(csvfile& file)
{
    file.endrow();
    return file;
}

inline static csvfile& flush(csvfile& file)
{
    file.flush();
    return file;
}

/*
 * A class to read data from a csv file. ----- CSVReader ------
 */

class CSVReader
{
    std::string fileName;
    char delimeter;

public:
    CSVReader(std::string filename, char delm = ',') :
            fileName(filename), delimeter(delm)
    { }

    // Function to fetch data from a CSV File
    std::vector<std::vector<std::string>> getDataCSV()
    {
        std::ifstream file(fileName);

        std::vector<std::vector<std::string> > dataList;

        std::string line = "";
        // Iterate through each line and split the content using delimeter
        while (getline(file, line))
        {
            auto vec = split(line, delimeter);
            dataList.push_back(vec);
        }
        // Close the File
        file.close();

        return dataList;
    }
    std::vector<std::string> split(const std::string& s, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens;
    }
};

/*
* Parses through csv file line by line and returns the data
* in vector of vector of strings.
*/


#endif //TRACK_RACING_CSVFILE_HPP
