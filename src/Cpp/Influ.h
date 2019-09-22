//
// Created by shaobo on 9/17/19.
//

#ifndef INFLU_H
#define INFLU_H

#include <map>
#include <string>
#include <vector>

using namespace std;

class Influ {
public:
    Influ();
    Influ(const Influ& orig);
    virtual ~Influ();
    Influ(vector< map<string, double> >, double);

    void setDNFProb(double);
    double getDNFProb();

    void setDNF(vector< map<string, double> >);
    vector< map<string, double> > getDNF();

    void setInfluence(vector< map<string, double> >);
    vector< pair<string, double> > getInfluence(int);

    void setChangeOrder(vector< pair<string, double> >, double);
    vector< pair<string, double> > getChangeOrder();

    double computeInflu(vector< map<string, double> >, pair<string, double>);
    double monteCarloSim(vector< map<string, double> >);
    vector< pair<string, double> > sortMap(map<string, double>);
    void printInflu(vector< pair<string, double> >);

private:
    vector< pair<string, double> > influence;
    vector< pair<string, double> > changeOrder;
    vector< map<string, double> > DNF;
    double DNFProb;
};

#endif //SUFFICIENTLINEAGE_INFLU_H
