/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Suff.cpp
 * Author: shaobo
 * 
 * Created on October 12, 2018, 5:43 AM
 */

#include <algorithm>
#include <random>
#include <iostream>
#include <set>
#include "Suff.h"
#include "Para.h"

using namespace std;

Suff::Suff() {
}

Suff::Suff(const Suff& orig) {
}

Suff::~Suff() {
}

Suff::Suff(vector< map<string, double> > lambda, double errRate) {
    Suff::setOrigDNF(lambda);
    Suff::setOrigProb(lambda);

    clock_t t = clock();
    Suff::setSuffDNF(errRate);
    t = clock() - t;
    cout << "Compute Suff lineage time: "<<((float) t)/CLOCKS_PER_SEC << " seconds" << endl;

    Suff::setSuffProb(Suff::getSuffDNF());
}

void Suff::setOrigDNF(vector<map<string, double> > lambda) {
    this->origDNF = lambda;
}

vector<map<string, double> > Suff::getOrigDNF() {
    return this->origDNF;
}

void Suff::setOrigProb(vector< map<string, double> > lambda) {
    clock_t t = clock();
    this->origProb = monteCarloSim(lambda);
    //this->origProb = shannonExpan(lambda);
    t = clock() - t;
    cout << "MC time: "<<((float) t)/CLOCKS_PER_SEC << " seconds" << endl;
}

double Suff::getOrigProb() {
    return this->origProb;
}

void Suff::setSuffDNF(double err) {
    // sort monomials in lambda in descending order
    sort(this->origDNF.begin(), this->origDNF.end(), [] (const map<string, double> & lhs, const map<string, double> & rhs) {
        double p_lhs = 1.0;
        for(map<string, double>::const_iterator it=lhs.begin(); it!=lhs.end(); it++) {
            p_lhs *= it->second;
        }

        double p_rhs = 1.0;
        for(map<string, double>::const_iterator it=rhs.begin(); it!=rhs.end(); it++) {
            p_rhs *= it->second;
        }
        return p_lhs > p_rhs;
    });
    // target probability of suffprov
    double t = this->origProb * (1 - err);
    // double t = this->origProb - error;

    /*
    // pseudo code for binary search
    function binary_search(A, n, T):
    L := 0
    R := n − 1
    while L <= R:
        m := floor((L + R) / 2)
        if A[m] < T:
            L := m + 1
        else if A[m] > T:
            R := m - 1
        else:
            return m
    return unsuccessful
     */

    // log time (binary search) trimming DNF
    int l = 1;
    int r = this->origDNF.size();
    // store visited point indexq
    set<int> memory;
    while(l <= r) {
        int m = (l+r) / 2; //floor
        if(memory.find(m) != memory.end()) {
            vector< map<string, double> > lambda_temp;
            lambda_temp.assign(this->origDNF.begin(), this->origDNF.begin()+m);
            this->suffDNF = lambda_temp;
            break;
        } else {
            vector< map<string, double> > lambda_temp;
            lambda_temp.assign(this->origDNF.begin(), this->origDNF.begin()+m);
            // copy(lambda.begin()+m,lambda.end(), v_temp.begin());
            double p = monteCarloSim(lambda_temp);
            double delta = 0.0001;
            if(p + delta < t) {
                //cut less
                l = m;
            } else if(t + delta < p) {
                //cut more
                r = m;
            } else if(p <= t && t <= p + delta) {
                //right there
                this->suffDNF = lambda_temp;
            } else {
                cout << "Not found! " << endl;
                this->suffDNF = this->origDNF;
            }
        }
        memory.insert(m);
    }
}

vector < map<string, double> > Suff::getSuffDNF() {
    return this->suffDNF;
}

void Suff::setSuffProb(vector<map<string, double> > lambda) {
    this->suffProb = Suff::monteCarloSim(lambda);
}

double Suff::getSuffProb() {
    return this->suffProb;
}

//MC simulation
/*
double Suff::monteCarloSim(vector< map<string, double> > lambda) {
    int rounds = 10000;
    int sum = 0;
    //simulation
    //default_random_engine generator;
    random_device rd;
    mt19937 generator(rd());
    uniform_real_distribution<double> distribution(0.0,1.0);
    for(int i = 0; i < rounds; i++) {
        map<string, int> assignment;
        /*
        assignment["ra"] = 1;
        assignment["rb"] = 1;
        assignment["r0"] = 1;
        assignment["r1"] = 1;
        assignment["r2"] = 1;
        assignment["r3"] = 1;
        assignment["r4"] = 1;
        assignment["r5"] = 1;
        assignment["r6"] = 1;
        *//*
        int lambdaValue = 0;
        // for each mono in lambda
        // from the first literal, if it is not in assignment, roll dice and record its assignment
        // if the assigned value == 0, then the mono == 0
        // if assigned value is one, then continue on next literal
        for(vector< map<string, double> >::iterator mono = lambda.begin(); mono != lambda.end(); mono++) {
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
            // if any mono == 1, then lambda == 1, record the value and continue on next round
            if(monoValue == 1) {
                lambdaValue = 1;
                break;
            }
        }
        sum += lambdaValue;
    }
    return 1.0 * sum / rounds;
}
*/

// Para monteCarloSimulation
double Suff::monteCarloSim(vector< map<string, double> > lambda) {
	return Para::p_monteCarloSim(lambda);
}

void Suff::printDNF(vector< map<string, double> > lambda) {
    for(vector< map<string, double> >::iterator mono = lambda.begin(); mono != lambda.end(); mono++) {
        double p_m = 1.0;
        cout << "[" << mono - lambda.begin() << "] ";
        for(map<string, double>::iterator literal = (*mono).begin(); literal != (*mono).end(); literal++) {
            cout << literal->first << " ";
            p_m *= literal->second;
        }
        cout << "MonoProb = " << p_m << endl;
    }
}

double Suff::shannonExpan(vector<map<string, double> > lambda) {
    // lambda = x * ( lambda(x=1) ) + (1-x) * ( lambda(x=0) )
    if(lambda.size() == 1) {
        double p_m = 1.0;
        for(vector< map<string, double> >::iterator mono = lambda.begin(); mono != lambda.end(); mono++) {
            for(map<string, double>::iterator lit = (*mono).begin(); lit != (*mono).end(); lit++) {
                p_m *= lit->second;
            }
        }
        return p_m;
    } else {
        string name = lambda.front().begin()->first;
        double p = lambda.front().begin()->second;
        vector< map<string, double> > lambda1(lambda);
        double l1 = 0.0;
        // cout << "Literal: " << literal.first << " " << literal.second << endl;
        for (vector<map<string, double> >::iterator mono = lambda1.begin(); mono != lambda1.end(); mono++) {
            if (mono->find(name) != mono->end()) {
                mono->erase(name);
            }
            if(mono->size() == 0) {
                l1 = 1.0;
            }
        }
        vector< map<string, double> > lambda0;
        double l2 = 1.0;
        for (vector<map<string, double> >::iterator mono = lambda.begin(); mono != lambda.end(); mono++) {
            if (mono->find(name) == mono->end()) {
                lambda0.push_back((*mono));
            }
        }
        if(lambda0.size() == 0) {
            l2 = 0.0;
        }
        if(l1 == 1.0 && l2 == 0.0) {
            return p;
        } else if(l1 == 1.0 && l2 != 0.0) {
            return p + (1-p) * shannonExpan(lambda0);
        } else if(l1 != 1.0 && l2 == 0.0) {
            return p * shannonExpan(lambda1);
        } else {
            return p * shannonExpan(lambda1) + (1-p) * shannonExpan(lambda0);
        }
    }
}


