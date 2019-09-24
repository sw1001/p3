//
// Created by shaobo on 9/21/19.
//

#include "Change.h"
#include "Influ.h"
#include "Suff.h"
#include "Para.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <set>

using namespace std;

Change::Change() {

}

Change::Change(const Change & orig) {

}

Change::~Change() {

}

Change::Change(vector< map<string, double> > lambda, double lambdaProb, pair<string, double> topInflu, double target) {
    clock_t t = clock();
    Change::setChangeOrder(lambda, lambdaProb, topInflu, target);
    t = clock() - t;
    cout << "Changing order running time: " << ((float) t)/CLOCKS_PER_SEC << " seconds" << endl;
}

void Change::setChangeOrder(vector< map<string, double> > lambda, double lambdaProb, pair<string, double> topInflu, double target) {
    cout << "Target prob = " << target << endl;
    cout << "Current prob = "<< lambdaProb << endl;
    double memory = lambdaProb;
    if(target == lambdaProb) {
        cout << "No change needed!" << endl;
    } else if(lambdaProb < target) {
        // increase the prob of the top influ literal to 1, then do MC
        vector< map<string, double> > DNFTemp(lambda);
        for(vector< map<string, double> >::iterator mono = DNFTemp.begin(); mono != DNFTemp.end(); mono++) {
            if(mono->find(topInflu.first) != mono->end() ) {
                mono->erase(topInflu.first);
            }
        }
        double prob = Suff::monteCarloSim(DNFTemp);
        //double prob = Suff::shannonExpan(DNFTemp);
        cout<<"Changed literal: "<<topInflu.first<<" New prob = "<<prob<<endl;
        if(prob < target) {
            this->changeOrder.emplace_back(topInflu.first + " current prob =", prob);
            Influ influ(DNFTemp, prob);
            cout<<"Top influ literal: ";
            influ.printInflu(influ.getInfluence(1));
            cout<<endl;
            Change::setChangeOrder(DNFTemp, prob, influ.getInfluence(1).front(), target);
        } else {
            double delta_p = (target - memory) / topInflu.second;
            this->changeOrder.emplace_back(topInflu.first + " delta_p =", delta_p);
        }

    } else {
        // decrease the prob of the top influ literal to 0, then do MC
        vector< map<string, double> > DNFTemp;
        for(vector< map<string, double> >::iterator mono = lambda.begin(); mono != lambda.end(); mono++) {
            if(mono->find(topInflu.first) == mono->end() ) {
                DNFTemp.push_back((*mono));
            }
        }
        double prob = Suff::monteCarloSim(DNFTemp);
        //double prob = Suff::shannonExpan(DNFTemp);
        cout<<"Changed literal: "<<topInflu.first<<" New prob = "<<prob<<endl;
        if(prob > target) {
            this->changeOrder.emplace_back(topInflu.first + " current prob =", prob);
            Influ influ(DNFTemp, prob);
            cout<<"Top influ literal: ";
            influ.printInflu(influ.getInfluence(1));
            cout<<endl;
            Change::setChangeOrder(DNFTemp, prob, influ.getInfluence(1).front(), target);
        } else {
            double delta_p = (target - memory) / topInflu.second;
            this->changeOrder.emplace_back(topInflu.first + " delta_p =", delta_p);
        }
    }
}

vector< pair<string, double> > Change::getChangeOrder() {
    return this->changeOrder;
}

void Change::printChangeOrder(vector< pair<string, double> > order) {
    for(vector< pair<string, double> >::iterator it = order.begin(); it != order.end(); it++) {
        cout << it->first << " " << it->second << endl;
    }
}
