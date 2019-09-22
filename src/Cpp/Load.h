//
// Created by shaobo on 9/17/19.
//

#ifndef SUFFLINEAGE_LOAD_H
#define SUFFLINEAGE_LOAD_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <string>
#include <map>

using namespace std;

class Load {
public:
    Load();
    Load(const Load& orig);
    ~Load();

    Load(string, string);
    void setProv(string);
    string getProv();

    void setProbs(string);
    map<string, double> getProbs();

private:
    map<string, double> prbs;
    string prv;
};

#endif //SUFFLINEAGE_LOAD_H
