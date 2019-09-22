//
// Created by shaobo on 9/17/19.
//

#include "Load.h"


Load::Load() {
}

Load::Load(const Load& orig) {
}

Load::~Load() {
}

Load::Load(string sd, string sp) {
    Load::setProv(sp);
    Load::setProbs(sd);
}

void Load::setProbs(string sd) {
    // a map to store literal's name and probability
    map<string, double> p;

    //read trust data from files
    ifstream fin(sd);
    string line;
    int i = 0;
    while (getline(fin, line))
    {
        istringstream sin(line);
        vector<string> fields;
        string field;
        while (getline(sin, field, ','))
        {
            fields.push_back(field);
        }
        if (fields[0] == "node1")
        {
            continue;
        }
        i++;
        string name;
        name.append("trust").append(fields[0]).append("-").append(fields[1]);
        double prob = stod(fields[2]);
        p[name] = prob;
        //cout << name << " " << p[name] << endl;
    }
    cout << "total number of element: " << i << endl;

    p["ra"] = 1.0;
    p["rb"] = 1.0;
    p["r0"] = 1.0;
    p["r1"] = 1.0;
    p["r2"] = 1.0;
    p["r3"] = 1.0;
    p["r4"] = 1.0;
    p["r5"] = 1.0;
    p["r6"] = 1.0;
    this->prbs = p;
}

map<string, double> Load::getProbs() {
    return this->prbs;
}

void Load::setProv(string sp) {
    //read prov from files
    ifstream pfin(sp);
    stringstream buffer;
    buffer << pfin.rdbuf();
    string prov = buffer.str();
    prov.erase(0,prov.find_first_not_of(" \t\r\n"));
    prov.erase(prov.find_last_not_of(" \t\r\n") + 1);
    this->prv = prov;
}

string Load::getProv() {
    return this->prv;
}

