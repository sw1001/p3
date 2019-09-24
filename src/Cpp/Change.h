//
// Created by shaobo on 9/21/19.
//

#ifndef SUFFLINEAGE_CHANGE_H
#define SUFFLINEAGE_CHANGE_H

#include <map>
#include <string>
#include <vector>

using namespace std;

class Change {
public:
    Change();
    Change(const Change & orig);
    virtual ~Change();
    Change(vector< map<string, double> >, double, pair<string, double>, double);

    void setChangeOrder(vector< map<string, double> >, double, pair<string, double>, double);
    vector< pair<string, double> > getChangeOrder();

    void printChangeOrder(vector< pair<string, double> >);

private:
    vector< pair<string, double> > changeOrder;
};

#endif //SUFFLINEAGE_CHANGE_H
