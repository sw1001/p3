//
// Created by shaobo on 9/17/19.
//

#include "Influ.h"
#include "Suff.h"
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
    cout << "Sequential Influence running time: " << ((float) t)/CLOCKS_PER_SEC << " seconds" << endl;
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
                //find its influence
                influTemp[literal->first] = Influ::computeInflu(lambda, (*literal));
                //update memory
                memory.insert(literal->first);
            }
        }
    }

    influTemp.erase("ra");
    influTemp.erase("rb");
    influTemp.erase("r0");
    influTemp.erase("r1");
    influTemp.erase("r2");
    influTemp.erase("r3");
    influTemp.erase("r4");
    influTemp.erase("r5");
    influTemp.erase("r6");

    this->influence = Influ::sortMap(influTemp);
}

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

double Influ::computeInflu(vector< map<string, double> > lambda, pair<string, double> literal) {
    int rounds = 10000;
    int sum0 = 0;
    int sum1 = 0;
    //simulation
    default_random_engine generator;
    uniform_real_distribution<double> distribution(0.0,1.0);
    for(int i = 0; i < rounds; i++) {
        map<string, int> assignment;
        assignment[literal.first] = 0;
        // sum0 += Suff::singleRound(assignment, lambda, generator, distribution);
        int lambdaValue0 = 0;
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

        assignment[literal.first] = 1;
        // sum1 += Suff::singleRound(assignment, lambda, generator, distribution);
        int lambdaValue1 = 0;
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
    }
    return 1.0 * (sum1 - sum0) / rounds;
}

//sort influTemp by value
vector< pair<string, double> > Influ::sortMap(map<string, double> influTemp) {
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

double Influ::monteCarloSim(vector< map<string, double> > lambda) {
    int rounds = 10000;
    int sum = 0;
    //simulation
    default_random_engine generator;
    uniform_real_distribution<double> distribution(0.0,1.0);
    for(int i = 0; i < rounds; i++) {
        map<string, int> assignment;
        // sum += Suff::singleRound(assignment, lambda, generator, distribution);
        int lambdaValue = 0;
        // for each mono in lambda
        // from the first literal, if it is not in assignment, roll dice and record its assignment
        // if the assigned value of this literal equals zero, then the mono == 0
        // if assigned value is one, then continue on next literal
        // if any mono == 1, then lambda == 1, record the value and continue on next round
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
                    }
                } else {
                    if (assignment[lit->first] == 0) {
                        monoValue = 0;
                        break;
                    }
                }
            }
            if(monoValue == 1) {
                lambdaValue = 1;
                break;
            }
        }
        sum += lambdaValue;
    }
    return 1.0 * sum / rounds;
}

void Influ::setChangeOrder(vector< pair<string, double> > influ, double target) {
    cout<<"Target prob = "<<target<<endl;
    cout<<"Current prob = "<<this->DNFProb<<endl;
    if(this->DNFProb < target) {
        // increase the prob of the top influ literal to 1, then do MC
        vector< map<string, double> > DNFTemp = this->DNF;
        double memory = this->DNFProb;
        for(vector< pair<string, double> >::iterator it = influ.begin(); it != influ.end(); it++) {
            for(vector< map<string, double> >::iterator mono = DNFTemp.begin(); mono != DNFTemp.end(); mono++) {
                (*mono)[it->first] = 1.0;
            }
            double prob = Suff::monteCarloSim(DNFTemp);
            cout<<"New prob after changing = "<<prob<<endl;
            if(prob < target) {
                this->changeOrder.emplace_back(it->first, prob);
                memory = prob;
                continue;
            } else {
                double delta_p = (target - memory) / it->second;
                this->changeOrder.emplace_back(it->first + "delta_p", delta_p);
                break;
            }
        }
    } else {
        // decrease the prob of the top influ literal to 0, then do MC
        vector< map<string, double> > DNFTemp = this->DNF;
        double memory = this->DNFProb;
        for(vector< pair<string, double> >::iterator it = influ.begin(); it != influ.end(); it++) {
            for(vector< map<string, double> >::iterator mono = DNFTemp.begin(); mono != DNFTemp.end(); mono++) {
                (*mono)[it->first] = 0.0;
            }
            double prob = Suff::monteCarloSim(DNFTemp);
            cout<<"New prob after changing:"<<prob<<endl;
            if(prob > target) {
                this->changeOrder.emplace_back(it->first, prob);
                memory = prob;
                continue;
            } else {
                double delta_p = (target - memory) / it->second;
                this->changeOrder.emplace_back(it->first + " delta_p", delta_p);
                break;
            }
        }
    }
}

vector< pair<string, double> > Influ::getChangeOrder() {
    return this->changeOrder;
}





