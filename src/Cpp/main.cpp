/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: shaobo
 *
 * Created on October 12, 2018, 3:32 AM
 */

#include <cstdlib>
#include <iostream>
#include <cctype>
#include <ctime>
#include "Load.h"
#include "DNF.h"
#include "Suff.h"
#include "Influ.h"
#include "Para.h"
#include "cl.hpp"

using namespace std;

#define ProvPath "/home/sleepytodeath/p3/data/prov/prov_sample_500.txt"
#define DataPath "/home/sleepytodeath/p3/data/trust/sample_500_shaobo.csv"

/*
 * main method to compute the sufficient lineage given a k-mDNF formula and 
 * an approximation error epsilon greater than 0
 */
int main(int argc, char** argv) {

    Para para(argc, argv);
    Load load(DataPath, ProvPath);

    cout << "------------------------------" << endl;

    DNF dnf(load.getProv(), load.getProbs());
    //cout << dnf.ToString() << endl;

    cout << "------------------------------" << endl;

    double errRate = 0.01; // approximation error rate
    Suff suff(dnf.getLambda(), errRate);
    //cout << "The original DNF is: " << endl;
    //suff.printDNF(suff.getOrigDNF());
    cout << "Original Probability = " << suff.getOrigProb() << endl;
    cout << endl;
    cout << "The sufficient DNF is: " << endl;
    suff.printDNF(suff.getSuffDNF());
    cout << "SuffProv Probability = " << suff.getSuffProb() << endl;

    cout << "------------------------------" << endl;

    Influ influ(suff.getOrigDNF(), suff.getOrigProb());
    cout << "Sequential Influence: " << endl;
    influ.printInflu(influ.getInfluence(0));
    cout << "Sequential Most Influential Tuple: " << endl;
    influ.printInflu(influ.getInfluence(1));
    cout << endl;
    /*
    clock_t t = clock();
    influ.setChangeOrder(influ.getInfluence(0), influ.getDNFProb() - 0.1);
    t = clock() - t;
    cout << "Changing order running time: " << ((float) t)/CLOCKS_PER_SEC << " seconds" << endl;
    cout << "Changing order: " << endl;
    influ.printInflu(influ.getChangeOrder());
    */
    
    cout << "------------------------------" << endl;
    clock_t t_para = clock();
    para.p_setInfluence_lit(suff.getOrigDNF());
    t_para = clock() - t_para;
    cout << "Parallel Influence running time: " << ((float) t_para)/CLOCKS_PER_SEC << " seconds" << endl;
    
    return 0;
}

