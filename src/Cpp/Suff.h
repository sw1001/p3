/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Suff.h
 * Author: shaobo
 *
 * Created on October 12, 2018, 5:43 AM
 */

#ifndef SUFF_H
#define SUFF_H

#include <vector>
#include <string>
#include <random>
#include <map>

using namespace std;

class Suff {
public:
    Suff();
    Suff(const Suff& orig);
    virtual ~Suff();
    Suff(vector< map<string, double> >, double);

    void setSuffDNF(double);
    vector< map<string, double> > getSuffDNF();

    void setSuffProb(vector< map<string, double> >);
    double getSuffProb();

    void setOrigDNF(vector< map<string, double> >);
    vector < map<string, double> > getOrigDNF();

    void setOrigProb(vector< map<string, double> >);
    double getOrigProb();

    static double shannonExpan(vector< map<string, double> >);
    static double monteCarloSim(vector< map<string, double> >);
    static void printDNF(vector< map<string, double> >);

private:
    vector< map<string, double> > origDNF;
    double origProb;
    vector< map<string, double> > suffDNF;
    double suffProb;
};

#endif /* SUFF_H */

