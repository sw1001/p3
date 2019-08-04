/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Suff.cpp
 * Author: shaobo, chenyuan
 * 
 * Created on October 12, 2018, 5:43 AM
 * Updated on July 22, 2019
 */

#include <algorithm>
#include "Suff.h"
#include <time.h>
#include <valarray>
#include <random>
#include <iomanip>

using namespace std;

Suff::Suff() {
}

Suff::Suff(const Suff& orig) {
}

Suff::~Suff() {
}

vector < vector <Literal> > Suff::findMatch(vector < vector <Literal> > lambda) {
    //greedily find a match
    vector <string> names(0);
    vector < vector <Literal> > match(0);
    match.push_back(lambda[0]);
    for (int i = 0; i < lambda[0].size(); i++) {
        names.push_back(lambda[0][i].getName());
    }
    for (int i = 1; i < lambda.size(); i++) {
        bool independent = true;
        for (int j = 0; j < lambda[i].size(); j++) {
            if (find(names.begin(), names.end(), lambda[i][j].getName()) != names.end()) {
                independent = false;
                break;
            }
        }
        if (independent) {
            match.push_back(lambda[i]);
            for (int j = 0; j < lambda[i].size(); j++) {
                names.push_back(lambda[i][j].getName());
            }
        }
    }
    return match;
}
/*
vector< vector<Literal> > Suff::findSuff(vector< vector<Literal> > lambda, double epsilon) {
    sort(lambda.begin(), lambda.end(), [](const vector<Literal> lhs, const vector<Literal> rhs) {
        double p_lhs = 1.0;
        for (int i = 0; i < lhs.size(); i++) {
            p_lhs *= lhs[i].getProb();
        }

        double p_rhs = 1.0;
        for (int i = 0; i < rhs.size(); i++) {
            p_rhs *= rhs[i].getProb();
        }

        return p_lhs < p_rhs;
    });
    //Suff::printProv(lambda);
    //TODO: remove monomials whose probabilities equal to zero
    for (vector < vector<Literal> >::iterator it = lambda.begin(); it != lambda.end();) {
        vector <Literal> monomial = *it;
        bool isZero = false;
        for (vector <Literal>::iterator it2 = monomial.end(); it2 != monomial.begin(); it2--) {
            if (it2->getProb() == 0.0 || it2->getProb() == NULL) {
                isZero = true;
                break;
            }
        }
        if (isZero) {
            it = lambda.erase(it);
        } else {
            ++it;
        }
    }
    double pLambda = probMC(lambda);
    double t = pLambda - epsilon;

    int l = 0;
    int r = lambda.size()-1;
    while(l <= r){
        int m = floor((l+r)/2);
        vector< vector<Literal> > v_temp(0);
        vector< vector<Literal> > v_temp1(0);
        v_temp.assign(lambda.begin()+m,lambda.end());
        v_temp1.assign(lambda.begin()+m+1,lambda.end());
        //copy(lambda.begin()+m,lambda.end(),v_temp.begin());
        //copy(lambda.begin()+m+1,lambda.end(),v_temp1.begin());
        double p = probMC(v_temp);
        double p1 = probMC(v_temp1);
        if(p < t && p1 < t) {
            //cut less
            r = m;
        } else if(p > t && p1 > t) {
            //cut more
            l = m;
        } else if(p > t && p1 < t) {
            //right there
            return v_temp;
        } else {
            cout<<"Not found! "<<endl;
        }
    }

    return lambda;
}
*/

vector< vector<Literal> > Suff::findSuff(vector< vector<Literal> > lambda, double epsilon) {
    sort(lambda.begin(), lambda.end(), [](const vector<Literal> lhs, const vector<Literal> rhs) {
        double p_lhs = 1.0;
        for (int i = 0; i < lhs.size(); i++) {
            p_lhs *= lhs[i].getProb();
        }

        double p_rhs = 1.0;
        for (int i = 0; i < rhs.size(); i++) {
            p_rhs *= rhs[i].getProb();
        }

        return p_lhs < p_rhs;
    });
	
	clock_t tmc = clock();
    double pLambda = probMC(lambda);
    double t = pLambda - epsilon;
    tmc = clock()-tmc;
    cout<<"t="<<t<<endl;
    cout<<"suffcount=0"<<"  "<<"MC running time: "<<((float)tmc)/CLOCKS_PER_SEC<<" seconds"<<endl<<endl;
    
  
    int l = 0;
    int r = lambda.size()-1;
    int suffcount = 1;
    while(l <= r){
        int m = floor((l+r)/2);
        vector< vector<Literal> > v_temp(0);
        vector< vector<Literal> > v_temp1(0);
        v_temp.assign(lambda.begin()+m,lambda.end());
        v_temp1.assign(lambda.begin()+m+1,lambda.end());
        //copy(lambda.begin()+m,lambda.end(),v_temp.begin());
        //copy(lambda.begin()+m+1,lambda.end(),v_temp1.begin());
        clock_t tmc = clock();
        double p = probMC(v_temp);
        double p1 = probMC(v_temp1);
        tmc = clock()-tmc;
        cout<<"p="<<p<<" "<<"p1="<<p1<<endl;
        cout<<"suffcount="<<suffcount<<"  "<<"MC running time: "<<((float)tmc)/CLOCKS_PER_SEC<<" seconds"<<endl<<endl;
    	suffcount ++;
        if(p < t && p1 < t) {
            //cut less
            r = m;
        } else if(p > t && p1 > t) {
            //cut more
            l = m;
        } else if(p > t && p1 < t) {
            //right there
            return v_temp;
        } else {
        	//not gonna happen
            cout<<"Not found! "<<endl;
        }
    }

}




/*
vector< vector<Literal> > Suff::findSuff(vector< vector<Literal> > lambda, double epsilon) {
    vector< vector<Literal> > match = Suff::findMatch(lambda);
    //cout<<"print match"<<endl;
    //Suff::print(match);
    double pLambda = Suff::probMC(lambda);
    //cout<<"pLambda = "<<pLambda<<endl;
    double pMatch = Suff::probMatch(match);
    //cout<<"pMatch = "<<pMatch<<endl;
    int l = match.size();
    //cout<<"print match size"<<endl;
    //cout<<l<<endl;
    if(pLambda - pMatch <= epsilon) {
        //Sort(match);
        vector< vector< Literal > > m(0);        
        double order[l];
        double order_copy[l];
        int order_index[l];
        for (int i = 0; i < l; i++) {
            order[i] = Suff::probMono(match[i]);
            order_copy[i] = Suff::probMono(match[i]);
            order_index[i] = i;
        }
        //sort(order_index.begin(), order_index.end(), [&](int x, int y) { \
            return order[x] < order[y]; });
        
        sort(order, order + l, greater<double>());
        for (int i = 0; i < l ; i++) {
            order_index[i] = find(order_copy, order_copy + l, order[i]) - order_copy;
        }
                
        for (int i = 0; i < l ; i++) {
            m.push_back(match[order_index[i]]);
            //cout<<"m = ";
            //Suff::print(m);
            if(pLambda - Suff::probMatch(m) <= epsilon){
               break; 
            }            
        }
        //cout<<"m = "<<endl;
        //Suff::print(m);
        return m;
    } else {
        //find a small cover
        vector<Literal> cover(0);
        for (int i = 0; i < l ; i++) {
            for (int j = 0; j < match[i].size(); j++) {
                cover.push_back(match[i][j]);
            }
        }
        int c = cover.size();
        //cout<<"print cover size"<<endl;
        //cout<<c<<endl;
        //create a set of buckets
        vector<  vector< vector<Literal> > > buckets(c);
        //each mono in lambda
        for (int i = 0; i < lambda.size(); i++) {
            //each literal in mono
            vector<Literal> candidates(0);
            for (int j = 0; j < lambda[i].size(); j++) {
                //find potential covers
                if(Suff::isIn(lambda[i][j], cover)) {
                    candidates.push_back(lambda[i][j]);
                }
            }
            //arbitrarily pick a candidate literal
            Literal candidate = candidates[rand() % candidates.size()];
            //cout<<"candidate: "<<candidate.getName()<<endl;
            //remove candidate from lambda[i] and send it to corresponding bucket
            for (int j = 0; j < lambda[i].size(); j++) {
                if(lambda[i][j].getName() == candidate.getName()){
                    lambda[i].erase(lambda[i].begin() + j);
                    break;
                    //j--;
                }
            }
            for (int j = 0; j < c; j++) {
                //cout<<cover[j].getName()<<" "<<candidate.getName()<<endl;
                if(cover[j].getName() == candidate.getName() && !lambda[i].empty()){
                    //cout<<"print candidate's name: "<<endl;
                    //cout<<j<<" "<<candidate.getName()<<endl;
                    //Suff::print(lambda[i]);
                    //cout<<"+++++++++++++"<<"j="<<j<<endl;
                    buckets[j].push_back(lambda[i]);
                }
            }
        }        
        
        //iterate through each bucket
        vector< vector<Literal> >  lambda_s(0);
        for (int i = 0; i < buckets.size(); i++) {
            if (!buckets[i].empty()) {
                vector< vector<Literal> > v = Suff::findSuff(buckets[i], epsilon/c/cover[i].getProb());
                //add cover[i] back to v and attach to suff lineage lambda
                //cout<<"bucket cover"<<i<<"="<<cover[i].getName()<<endl;
                for (int j = 0; j < v.size(); j++) {
                    if (cover[i].getName() < v[j][0].getName()) {
                        v[j].insert(v[j].begin(), cover[i]);
                    } else if (cover[i].getName() > v[j][v[j].size()-1].getName()) {
                        v[j].push_back(cover[i]);
                    } else {
                        for (int k = 0; k < v[j].size()-1; k++) {
                            if (cover[i].getName() > v[j][k].getName() && cover[i].getName() < v[j][k+1].getName()) {
                                v[j].insert(v[j].begin()+k+1, cover[i]);
                            }
                        }
                    }
                    
                    lambda_s.push_back(v[j]);
                }
            }
        }
        return lambda_s;
    }
    
}
*/
//MC simulation
double Suff::probMC(vector< vector<Literal> > v) {
    int rounds = 10000;
    int sum = 0;
    //cout<<"print v"<<endl;
    //Suff::print(v);
    
    map<string, double> probability;
    map<string, int> assignment;    
    
    // Extract Variables from Lambda
    int vs = v.size();
    for (int i = 0; i < vs; i++) {
        vector<Literal> monomial = v[i];
        for (int j = 0; j < monomial.size(); j++) {
            if (assignment.find(monomial[j].getName()) == assignment.end()) {
                assignment[monomial[j].getName()] = 0;
                probability[monomial[j].getName()] = monomial[j].getProb();
            }
        }
    }
    //simulation
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    for (int i = 0; i < rounds; i++) {
        // assign random value to literals        
        for (map<string, int>::iterator it = assignment.begin(); it != assignment.end(); it++) {
            //double prand = (double) rand() / (RAND_MAX + 1.0);
            double prand = distribution(generator);
            if (probability[it->first] < prand) {
                it->second = 0;
            }
            else {
                it->second = 1;
            }
        }
        //compute
        int value = 0;
        //for each monomial
        for (vector< vector<Literal> >::iterator it = v.begin(); it != v.end(); it++) { 
            vector<Literal> monomial = *it;//for each literal
            int valueMono = 1;
            for (vector<Literal>::iterator it2 = monomial.begin(); it2 != monomial.end(); it2++) {
                if (assignment[it2->getName()] == 0) {
                    valueMono = 0;
                    break;
                }
            }
            if(valueMono == 1) {
                value = 1;
                break; 
            }                        
        }        
        //update sum
        sum += value;
    }
    //cout<<"print probability"<<endl;
    //cout<<sum/rounds<<endl;
    return 1.0 * sum / rounds;
}

//MC2 simulation: for two lambda's
double Suff::probMC2(vector< vector<Literal> > v1, vector< vector<Literal> > v0) {
    int rounds = 10000;
    int sum = 0;
    map<string, double> probability;
    map<string, int> assignment;
    //cout<<"print v1"<<endl;
    //Suff::print(v1);
    int v1s = v1.size();
    //int v0s = v0.size();
    // Extract Variables from Lambda   
    for (int i = 0; i < v1s; i++) {
        vector<Literal> monomial = v1[i];
        for (int j = 0; j < monomial.size(); j++) {
            if (assignment.find(monomial[j].getName()) == assignment.end()) {
                assignment[monomial[j].getName()] = 0;
                probability[monomial[j].getName()] = monomial[j].getProb();
            }
        }
    }    
    //simulation
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    for (int i = 0; i < rounds; i++) {        
        // assign random value to literals        
        for (map<string, int>::iterator it = assignment.begin(); it != assignment.end(); it++) {
            //double prand = (double) rand() / (RAND_MAX + 1.0);
            double prand = distribution(generator);
            if (probability[it->first] < prand) {
                it->second = 0;
            }
            else {
                it->second = 1;
            }
        }
        //compute
        int value1 = 0;
        //for each monomial
        for (vector< vector<Literal> >::iterator it = v1.begin(); it != v1.end(); it++) { 
            vector<Literal> monomial = *it;//for each literal
            int valueMono = 1;
            for (vector<Literal>::iterator it2 = monomial.begin(); it2 != monomial.end(); it2++) {
                if (assignment[it2->getName()] == 0) {
                    valueMono = 0;
                    break;
                }
            }            
            if(valueMono == 1) {
                value1 = 1;
                break; 
            }            
        }
        
        int value0 = 0;
        //for each monomial
        for (vector< vector<Literal> >::iterator it = v0.begin(); it != v0.end(); it++) { 
            vector<Literal> monomial = *it;//for each literal
            int valueMono = 1;
            for (vector<Literal>::iterator it2 = monomial.begin(); it2 != monomial.end(); it2++) {
                if (assignment[it2->getName()] == 0) {
                    valueMono = 0;
                    break;
                }
            }            
            if(valueMono == 1) {
                value0 = 1;
                break; 
            }            
        }
        //update sum
        sum += value1-value0;
        
    }
    //cout<<"print probability"<<endl;
    //cout<<sum/rounds<<endl;
    return 1.0 * sum / rounds;
}

double Suff::probMatch(vector< vector<Literal> > v) {
    double p = 1.0;
    for (int i = 0; i < v.size(); i++) {
        p *= (1.0 - Suff::probMono(v[i]));
    }
    
    return 1.0 - p;
}

double Suff::probMono(vector<Literal> v) {
    double p_m = 1.0;
    for (int i = 0; i < v.size(); i++) {
        p_m *= v[i].getProb();
    }
    
    return p_m;
}

bool Suff::isIn(Literal x, vector<Literal> cover) {
    for(int i = 0; i < cover.size(); i++) {
        if(x.getName() == cover[i].getName()) {            
            return true;
        }
    }
    return false;
}


void Suff::printProv(vector< vector<Literal> > v) {    
    int n = v.size();
    vector <double> p(0);
    for(int i = 0; i < n; i++) {
        int m = v[i].size();
        double product = 1.0;
        for(int j = 0; j < m; j++){
            cout<<v[i][j].getName()<<", ";
            product *= v[i][j].getProb();
        }
        p.push_back(product);
        cout<<endl;
    }
    for(int i = 0; i<p.size(); i++) {
        cout<<"vector["<<i<<"] prob: "<<p[i]<<endl;
    }
}

vector < vector<Literal> > Suff::getSuffProv() {
    return this->suffProv;
}

void Suff::setSuffProv(vector< vector<Literal> >  lambda, double epsilon) {
    this->suffProv = Suff::findSuff(lambda, epsilon);
}


map<string, double> Suff::getInfluence() {
    return this->influence;
}

void Suff::setInfluence(vector < vector<Literal> >  sp) {
    clock_t t = clock();
    map <string, double> infl_x;
    //double p = Suff::probMC(sp);
    //cout<<"p="<<p<<endl;
    for(int i = 0; i<sp.size(); i++) {
        for(int j = 0; j<sp[i].size(); j++) {
            if(infl_x.find(sp[i][j].getName()) == infl_x.end()) {
                clock_t t1 = clock();

                vector < vector<Literal> > sp_x_t;
                sp_x_t = Suff::newLambda(sp, sp[i][j], true);
                //double p1 = Suff::probMC(sp_x_t);
                vector < vector<Literal> > sp_x_f;
                sp_x_f = Suff::newLambda(sp, sp[i][j], false);
                //double p2 = Suff::probMC(sp_x_f);
                //double infl = ((p2/sp[i][j].getProb())-p)/(1-sp[i][j].getProb());
                //double infl = p1 - p2;
                double inflPrime = Suff::probMC2(sp_x_t, sp_x_f);
                //cout<<"Literal influence running time: "<<((float)(clock() - t1))/CLOCKS_PER_SEC<<" seconds"<<"		";
                //cout<<sp[i][j].getName()<</*" infl= "<<infl<<*/" seqInfl= "<<inflPrime<<endl;
                infl_x[sp[i][j].getName()] = inflPrime;               

            }
        }
    }
    t = clock()- t;
    //cout<<"Total influence running time: "<<((float)t)/CLOCKS_PER_SEC<<" seconds"<<endl;
    /*
    for(map<string, double>::const_iterator it = infl_x.begin(); it != infl_x.end(); ++it) {
        std::cout << it->first << " " << it->second << "\n";
    }
    */
    cout<<"Total number of literals: "<<infl_x.size()<<endl;
    this->influence = infl_x;
}

vector < vector<Literal> > 
Suff::newLambda(vector<vector<Literal> > sp, Literal l, bool f) {
    vector<vector<Literal> > spCopy = sp;
    //cout<<"literal"<<l.getName()<<" "<<l.getProb()<<endl;
    if(f) {
        for(int i = 0; i<sp.size(); i++) {
            for(int j = 0; j<sp[i].size(); j++) {
                if(l.getName() == sp[i][j].getName()){                    
                    spCopy[i].erase(spCopy[i].begin()+j);
                    break;
                }
            }
        }
        return spCopy;
    } else {
        for(int i = 0; i<spCopy.size(); i++) {
            //bool has = false;
            for(int j = 0; j<spCopy[i].size(); j++) {
                if(l.getName() == spCopy[i][j].getName()){
                    spCopy.erase(spCopy.begin()+i);
                    i--;
                    break;
                }
            }
        }
        return spCopy;
    }
}

Literal Suff::maxInfluence() {
    map <string, double> infl_x = this->influence;
    double max = 0.0;
    string name = "";
    for(map<string, double>::const_iterator it = infl_x.begin(); it != infl_x.end(); ++it) {
        if(it->second > max && it->first.compare("r1") != 0 && it->first.compare("r2") != 0 && it->first.compare("r3") != 0 && it->first.compare("ra") != 0 && it->first.compare("wordchurch") != 0 && it->first.compare("wordbarn") != 0 && it->first.compare("wordhotel") != 0 && it->first.compare("wordhouse") != 0) {
            max = it->second;
            name = it->first;
        }
    }
    return Literal(name, max);
}

void Suff::sortInfluence(string head){
	vector<Literal> infl_tmp;
	for(map<string, double>::const_iterator it = influence.begin(); it != influence.end(); ++it){
		string name = it->first;
		if(name.find(head) == 0 || head == ""){
			Literal l(name, it->second);
			infl_tmp.push_back(l);
		}

	}
	
	sort(infl_tmp.begin(), infl_tmp.end(), [](const Literal x, const Literal y){
		return x.getProb() > y.getProb();
	});
	for(int i = 0; i < infl_tmp.size(); ++i){
		cout<<setw(35)<<left<<infl_tmp[i].getName();
		cout<<"seqInfl="<<infl_tmp[i].getProb()<<endl;
	}
	
}



Literal Suff::findMostInfl(vector<vector<Literal> > sp) {
    map <string, double> infl_x;
    for(int i = 0; i<sp.size(); i++) {
        for(int j = 0; j<sp[i].size(); j++) {
            if(infl_x.find(sp[i][j].getName()) == infl_x.end()) {
                vector < vector<Literal> > sp_x_t;
                sp_x_t = Suff::newLambda(sp, sp[i][j], true);
                //double p1 = Suff::probMC(sp_x_t);
                vector < vector<Literal> > sp_x_f;
                sp_x_f = Suff::newLambda(sp, sp[i][j], false);
                //double p2 = Suff::probMC(sp_x_f);
                //double infl = ((p2/sp[i][j].getProb())-p)/(1-sp[i][j].getProb());
                //double infl = p1 - p2;
                double inflPrime = Suff::probMC2(sp_x_t, sp_x_f);
                //cout<<sp[i][j].getName()<<" inflPrime= "<<inflPrime<<endl;
                infl_x[sp[i][j].getName()] = inflPrime;                
            }
        }
    }
    double max = 0.0;
    string name = "";
    for(map<string, double>::const_iterator it = infl_x.begin(); it != infl_x.end(); ++it) {
        if(it->second > max && it->first.compare("r1") != 0 && it->first.compare("r2") != 0 && it->first.compare("r3") != 0 && it->first.compare("ra") != 0) {
            max = it->second;
            name = it->first;
        }
    }
    
    return Literal(name, max);
}



Literal Suff::p_findMostInfl(vector<vector<Literal> >sp, cl::Context context, cl::CommandQueue queue, cl::Program program){
	//prepare for the buffers here
	vector <int> h_lambdas(0);
	vector <float> h_lambdap(0);
	vector <int> h_dim2_size(0);			
	vector < vector<Literal> > h_lambda;
	h_lambda = sp;
	int index1 = 0;
	int size = 0;
	int dim1_size = h_lambda.size();
	//cout<<"size of suff.influence="<<suff.getInfluence().size()<<endl;
    map <string, int> str2index;// set an index for each string
	for(int i = 0; i < h_lambda.size(); i++){
		h_dim2_size.push_back(h_lambda[i].size());
		size += h_lambda[i].size();
		for(int j = 0; j < h_lambda[i].size(); j++){
			//cout<<i<<","<<j<<endl;
			if(str2index.find(h_lambda[i][j].getName()) == str2index.end()){
				str2index[h_lambda[i][j].getName()] = index1;
				index1 ++;               
			}
			h_lambdas.push_back(str2index[h_lambda[i][j].getName()]);
			h_lambdap.push_back(h_lambda[i][j].getProb());
			
		}
	}			
	vector <float> h_influence(index1);//int literals = index1;
	cout<< "index1= "<<index1<<" "<<"size= "<<size<<" "<<"dim1_size= "<<dim1_size<<endl;	

	cl::Buffer d_lambdas, d_lambdap, d_dim2_size, d_influence;
	d_lambdas = cl::Buffer(context, h_lambdas.begin(), h_lambdas.end(), true);
	d_lambdap = cl::Buffer(context, h_lambdap.begin(), h_lambdap.end(), true);
	d_dim2_size = cl::Buffer(context, h_dim2_size.begin(), h_dim2_size.end(), true);
	d_influence = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * index1);

	cl::make_kernel<int, int, int, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer> setInfluence(program, "setInfluence");
	cl::NDRange global(index1);
    setInfluence(cl::EnqueueArgs(queue, global), index1, size, dim1_size, d_lambdas, d_lambdap, d_dim2_size, d_influence);
    queue.finish();		
	cl::copy(queue, d_influence, h_influence.begin(), h_influence.end());
	
	//output the parallel results
	map <string, double> para_influence;
	map <string, double> seq_influence = getInfluence();
	for(map<string, int>::const_iterator it = str2index.begin(); it != str2index.end(); ++it){
		para_influence[it->first] = h_influence[str2index[it->first]];
		//cout<< it->first << "  deltaInfl=" << abs(para_influence[it->first] - seq_influence[it->first]) <<"  ";
		//cout<< it->first << "  paraInfl=" << para_influence[it->first] <<endl;
			
	}
	
	double max = 0.0;
	string max_name = "";
	for(map<string, double>::const_iterator it = para_influence.begin(); it != para_influence.end(); ++it) {
		if(it->second > max && it->first.compare("r1") != 0 && it->first.compare("r2") != 0 && it->first.compare("r3") != 0 && it->first.compare("ra") != 0) {
		    max = it->second;
		    max_name = it->first;
		}
	}

	return Literal(max_name, max);
	
}






vector<Literal> 
Suff::changedLiterals(vector< vector<Literal> > lambda, double t) {
    vector<Literal> v(0);
    double p = Suff::probMC(lambda);
    cout<<"plambda="<<p<<endl;
    if(p == t) {
        return v;
    }
    //find most influential literal(or randomly)
    Literal xm = Suff::p_findMostInfl(lambda);

    if(p < t) {
        // increase most influential literal
        for(int i = 0; i<lambda.size(); i++) {
            for(int j = 0; j<lambda[i].size(); j++) {
                if(lambda[i][j].getName() == xm.getName()) {
                    //lambda[i][j].setProb(1.0);
                    lambda[i].erase(lambda[i].begin()+j);
                    break;
                }
            }
        }
        double pp = Suff::probMC(lambda);
        cout<<"increased pp="<<pp<<endl;
        v.push_back(xm);
        if(pp-t >= 0.0) {
            //find delta prob for the most influential literal
            cout<<xm.getName()<<" cost="<<(t-p)/xm.getProb()<<endl;
            return v;
        } else {
            cout<<xm.getName()<<" cost="<<(pp-p)/xm.getProb()<<endl;
            vector<Literal> v2 = Suff::changedLiterals(lambda, t);
            v.insert(v.end(), v2.begin(), v2.end());
            return v;
        }
    } else {
        // decrease most influential literal
        for(int i = 0; i<lambda.size(); i++) {
            for(int j = 0; j<lambda[i].size(); j++) {
                if(lambda[i][j].getName() == xm.getName()) {
                    lambda.erase(lambda.begin()+i);
                    i--;
                    break;
                }
            }
        }
        double pp = Suff::probMC(lambda);
        cout<<"decreased pp="<<pp<<endl;
        v.push_back(xm);
        if(t-pp >= 0.0) {
            //find delta prob for the most influential literal
            cout<<xm.getName()<<" cost="<<(t-p)/xm.getProb()<<endl;
            return v;
        } else {
        	cout<<xm.getName()<<" cost="<<(pp-p)/xm.getProb()<<endl;
            vector<Literal> v2 = Suff::changedLiterals(lambda, t);
            v.insert(v.end(), v2.begin(), v2.end());
            return v;
        }
    }
}

void Suff::print(vector<Literal> v) {
    cout<<"------------------"<<endl;
    for(int i = 0; i<v.size(); i++) {
        cout<<"name="<<v[i].getName()<<" prob="<<v[i].getProb()<<endl;
    }
}
