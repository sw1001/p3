#ifndef PARA_H
#define PARA_H

#include <string>
#include <vector>
#include <map>
//include all related files for opencl
#include "cl.hpp"


using namespace std;

class Para {
public:
    Para();
    Para(const Para& orig);
    virtual ~Para();
    Para(int, char**);

    static double p_monteCarloSim(vector< map<string, double> >);
    void p_setInfluence(vector< map<string, double> >);
    void p_setInfluence_lit(vector< map<string, double> >);
    vector< pair<string, double> > sortMap(map<string, double>);

private:
    static cl::Context context;
    static cl::CommandQueue queue;
    static cl::Program programInflu;
    static cl::Program programInflu_lit;
    static cl::Program programMC;
    vector< pair<string, double> > influence;
};

#endif //SUFFICIENTLINEAGE_PARA_H
