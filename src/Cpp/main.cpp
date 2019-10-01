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

#define ProvPath "/home/chenyuan/new/p3/data/prov/prov_sample_500.txt"
#define DataPath "/home/chenyuan/new/p3/data/trust/sample_500_shaobo.csv"

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
    // cout << "The original DNF is: " << endl;
    // suff.printDNF(suff.getOrigDNF());
    cout << "Original DNF size: " << suff.getOrigDNF().size() << endl;
    cout << "Original Probability = " << suff.getOrigProb() << endl;
    cout << endl;
    // cout << "The sufficient DNF is: " << endl;
    // suff.printDNF(suff.getSuffDNF());
    cout << "Sufficient DNF size: " << suff.getSuffDNF().size() << endl;
    cout << "SuffProv Probability = " << suff.getSuffProb() << endl;

    cout << "------------------------------" << endl;

    Influ influ(suff.getOrigDNF(), suff.getOrigProb());
    cout << "Original lambda influence: " << endl;
    influ.printInflu(influ.getInfluence(0));
    //cout << "Distinct literals in original lambda: " << influ.getInfluence(0).size() << endl;
    cout << endl;

    Influ influSuff(suff.getSuffDNF(), suff.getSuffProb());
    cout << "Sufficient lineage influence: " << endl;
    influSuff.printInflu(influSuff.getInfluence(0));
    cout << "Total number of literals in sufficient lineage: " << influSuff.getInfluence(0).size() << endl;
    cout << endl;

    cout << "------------------------------" << endl;

    Change change(influ.getDNF(), influ.getDNFProb(), influ.getInfluence(1).front().first, influ.getDNFProb()+0.1);
    cout << "Changing order: " << endl;
    change.printChangeOrder(change.getChangeOrder());

    cout << "------------------------------" << endl;

    clock_t t_para = clock();
    para.p_setInfluence(suff.getOrigDNF());
    t_para = clock() - t_para;
    cout << "Parallel Influence running time: " << ((float) t_para)/CLOCKS_PER_SEC << " seconds" << endl;
    

    return 0;
}

