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
#include "Change.h"
#include "Para.h"

using namespace std;

#define ProvPath "/home/sleepytodeath/p3/data/prov/long.txt"
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
    cout<<"DNF number of monomials: "<<dnf.getLambda().size()<<endl;
    //dnf.ShowStructure();

    cout << "------------------------------" << endl;

    double errRate = 0.01; // approximation error rate
    Suff suff(dnf.getLambda(), errRate);
    // print results
    //cout << "The original DNF is: " << endl;
    //suff.printDNF(suff.getOrigDNF());
    cout << "Original Probability = " << suff.getOrigProb() << endl;
    cout << endl;
    cout << "The sufficient DNF is: " << endl;
    suff.printDNF(suff.getSuffDNF());
    cout << "SuffProv Probability = " << suff.getSuffProb() << endl;

    cout << "------------------------------" << endl;

    Influ influ(suff.getOrigDNF(), suff.getOrigProb());
    cout << "Influence: " << endl;
    influ.printInflu(influ.getInfluence(0));
    cout << endl;
    /*
    cout << "------------------------------" << endl;

    Change change(influ.getDNF(), influ.getDNFProb(), influ.getInfluence(1).front(), influ.getDNFProb()-0.1);
    cout << "Change order: " << endl;
    change.printChangeOrder(change.getChangeOrder());
    */
    cout << "------------------------------" << endl;
    clock_t t_para = clock();
    para.p_setInfluence(suff.getOrigDNF());
    t_para = clock() - t_para;
    cout << "Parallel Influence running time: " << ((float) t_para)/CLOCKS_PER_SEC << " seconds" << endl;
    

    return 0;
}

