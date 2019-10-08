//
// Created by shaobo on 9/17/19.
//

#include "Influ.h"
#include "Para.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <set>

using namespace std;

Influ::Influ() {

}

Influ::Influ(const Influ &orig) {

}

Influ::~Influ() {

}

Influ::Influ(vector< map<string, double> > lambda, double prob) {
    Influ::setDNF(lambda);
    Influ::setDNFProb(prob);
    clock_t t = clock();
    Influ::setInfluence(lambda);
    t = clock() - t;
    cout << "Influence running time: " << ((float) t)/CLOCKS_PER_SEC << " seconds" << endl;
}

void Influ::setDNFProb(double prob) {
    this->DNFProb = prob;
}

double Influ::getDNFProb() {
    return this->DNFProb;
}

void Influ::setDNF(vector<map<string, double> > lambda) {
    this->DNF = lambda;
}

vector< map<string, double> > Influ::getDNF() {
    return this->DNF;
}

// influence is defined as: For a literal x in lambda, Infl(x) =  P(lambda | x=1) - P(lambda | x=0)

void Influ::setInfluence(vector< map<string, double> > lambda) {
    map<string, double> influTemp;
    set<string> memory;
    for(vector< map<string, double> >::iterator mono = lambda.begin(); mono != lambda.end(); mono++) {
        for(map<string, double>::iterator literal = (*mono).begin(); literal != (*mono).end(); literal++) {
            if(memory.find(literal->first) == memory.end()) {
                //compute its influence
                influTemp[literal->first] = Influ::computeInflu(lambda, literal->first);
                //update memory
                memory.insert(literal->first);
            }
        }
    }
    // remove rule literal if needed
    influTemp.erase("ra");
    influTemp.erase("rb");
    influTemp.erase("r0");
    influTemp.erase("r1");
    influTemp.erase("r2");
    influTemp.erase("r3");
    influTemp.erase("r4");
    influTemp.erase("r5");
    influTemp.erase("r6");

    this->influence = Influ::sortInflu(influTemp);
}
/*
void Influ::setInfluence(vector< map<string, double> > lambda) {
	this->influence = Para::p_getInfluence(lambda, 1);
}
*/
// get the top-k most influential literals
vector< pair<string, double> > Influ::getInfluence(int k) {
    if(k <= 0 || k > this->influence.size()) {
        return this->influence;
    } else {
        vector< pair<string, double> > k_influ;
        k_influ.assign(this->influence.begin(), this->influence.begin()+k);
        return k_influ;
    }
}

double Influ::computeInflu(vector< map<string, double> > lambda, string literal) {
    vector< map<string, double> > lambda1(lambda);
    // cout << "Literal: " << literal.first << " " << literal.second << endl;
    for (vector<map<string, double> >::iterator mono = lambda1.begin(); mono != lambda1.end(); mono++) {
        if (mono->find(literal) != mono->end()) {
            mono->erase(literal);
        }
    }
    vector< map<string, double> > lambda0;
    for (vector<map<string, double> >::iterator mono = lambda.begin(); mono != lambda.end(); mono++) {
        if (mono->find(literal) == mono->end()) {
            lambda0.push_back((*mono));
        }
    }

    return Influ::monteCarloSim2(lambda1, lambda0);
}
double Influ::monteCarloSim2(vector< map<string, double> > lambda1, vector< map<string, double> > lambda0) {
    int rounds = 100000;
    int sum1 = 0;
    int sum0 = 0;
    //simulation
    random_device rd;
    mt19937 generator(rd());
    uniform_real_distribution<double> distribution(0.0,1.0);
    for(int i = 0; i < rounds; i++) {
        map<string, int> assignment;
        // assignment[literal.first] = 1;
        int lambdaValue1 = 0;
        for(vector< map<string, double> >::iterator mono = lambda1.begin(); mono != lambda1.end(); mono++) {
            int monoValue = 1;
            for(map<string, double>::iterator lit = (*mono).begin(); lit != (*mono).end(); lit++) {
                if(assignment.find(lit->first) == assignment.end()) {
                    double prand = distribution(generator);
                    if (lit->second < prand) {
                        assignment[lit->first] = 0;
                        monoValue = 0;
                        break;
                    } else {
                        assignment[lit->first] = 1;
                        continue;
                    }
                } else {
                    if (assignment[lit->first] == 0) {
                        monoValue = 0;
                        break;
                    }
                }
            }
            if(monoValue == 1) {
                lambdaValue1 = 1;
                break;
            }
        }
        sum1 += lambdaValue1;

        // assignment[literal.first] = 0;
        int lambdaValue0 = 0;
        for(vector< map<string, double> >::iterator mono = lambda0.begin(); mono != lambda0.end(); mono++) {
            int monoValue = 1;
            for(map<string, double>::iterator lit = (*mono).begin(); lit != (*mono).end(); lit++) {
                if(assignment.find(lit->first) == assignment.end()) {
                    double prand = distribution(generator);
                    if (lit->second < prand) {
                        assignment[lit->first] = 0;
                        monoValue = 0;
                        break;
                    } else {
                        assignment[lit->first] = 1;
                        continue;
                    }
                } else {
                    if (assignment[lit->first] == 0) {
                        monoValue = 0;
                        break;
                    }
                }
            }
            if(monoValue == 1) {
                lambdaValue0 = 1;
                break;
            }
        }
        sum0 += lambdaValue0;
    }
    return 1.0 * (sum1 - sum0) / rounds;
}

//sort influTemp by value
vector< pair<string, double> > Influ::sortInflu(map<string, double> influTemp) {
    vector< pair<string, double> > sortedInflu;
    for(map<string, double>::iterator it = influTemp.begin(); it != influTemp.end(); it++) {
        sortedInflu.push_back(make_pair(it->first, it->second));
    }
    sort(sortedInflu.begin(), sortedInflu.end(), [] (const pair<string, double> & lhs, const pair<string, double> & rhs) {
        return lhs.second > rhs.second;
    });
    return sortedInflu;
}

void Influ::printInflu(vector< pair<string, double> > influ) {
    for(vector< pair<string, double> >::iterator it = influ.begin(); it != influ.end(); it++) {
        cout << it->first << " " << it->second << endl;
    }
}







