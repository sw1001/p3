//
// Created by shaobo on 9/17/19.
//

#include "Influ.h"
#include "Para.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <set>
#include "cl.hpp"
#include "util.hpp"
#include "err_code.h"
#include "device_picker.hpp"

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
/*
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
*/

void Influ::setInfluence(vector< map<string, double> > lambda) {
	//prepare for the buffers here
	map <string, double> para_influence;
	vector <int> h_lambdas(0);
	vector <double> h_lambdap(0);
	vector <int> h_dim2_size(0);		
	vector < map<string, double > > h_lambda;
	h_lambda = lambda;
	int count = 10000;
	int index1 = 0;
	int size = 0;
	int dim1_size = h_lambda.size();
    map <string, int> str2index;// set an index for each string
	for(vector< map<string, double> >::iterator mono = h_lambda.begin(); mono != h_lambda.end(); mono++) {
		h_dim2_size.push_back((*mono).size());
		size += (*mono).size();
		for(map<string, double>::iterator lit = (*mono).begin(); lit != (*mono).end(); lit++) {
		    if(str2index.find(lit->first) == str2index.end()) {
		        str2index[lit->first] = index1;
		        index1 ++;
		    }
		    h_lambdas.push_back(str2index[lit->first]);
		    h_lambdap.push_back(lit->second);
		}
	}
	cout<< "index1= "<<index1<<" "<<"size= "<<size<<" "<<"dim1_size= "<<dim1_size<<endl;
    vector <int> h_resultOnce(count);//int literals = index1;
    cl::Buffer d_lambdas, d_lambdap, d_dim2_size, d_resultOnce;
    d_lambdas = cl::Buffer(context, h_lambdas.begin(), h_lambdas.end(), true);
    d_lambdap = cl::Buffer(context, h_lambdap.begin(), h_lambdap.end(), true);
	d_dim2_size = cl::Buffer(context, h_dim2_size.begin(), h_dim2_size.end(), true);
	d_resultOnce = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * count);
	cl::make_kernel<int, int, int, int, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer> setInfluence(programInflu, "setInfluence");    
	//set the parallel influence results
	for(map<string, int>::const_iterator it = str2index.begin(); it != str2index.end(); ++it){
	    vector <double> h_parameters(0);	
	    std::default_random_engine generator;
		std::uniform_real_distribution<double> distribution(0.0,1.0);
		for(int i = 0; i < count; i++){
		    for(int j = 0; j < index1; j++){
		        double prand = distribution(generator);
		        h_parameters.push_back(prand);
		    }
		}				

		cl::Buffer d_parameters;
		d_parameters = cl::Buffer(context, h_parameters.begin(), h_parameters.end(), true);

		cl::NDRange global(count);
		setInfluence(cl::EnqueueArgs(queue, global), it->second, index1, size, dim1_size, d_lambdas, d_lambdap, d_dim2_size, d_parameters, d_resultOnce);
		queue.finish();		
		cl::copy(queue, d_resultOnce, h_resultOnce.begin(), h_resultOnce.end());		
		para_influence[it->first] = std::count(h_resultOnce.begin(), h_resultOnce.end(), 1)*1.0/count;

		//cout<< it->first << "  paraInfl=" << para_influence[it->first] <<endl;			
	}
    para_influence.erase("ra");
    para_influence.erase("rb");
    para_influence.erase("r0");
    para_influence.erase("r1");
    para_influence.erase("r2");
    para_influence.erase("r3");
    para_influence.erase("r4");
    para_influence.erase("r5");
    para_influence.erase("r6");
	
	this->influence = Influ::sortMap(para_influence);
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
    int rounds = 10000;
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







