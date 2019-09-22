#include "Para.h"
#include <iostream>
#include <random>
#include <algorithm>
//include all related files for opencl
#define __CL_ENABLE_EXCEPTIONS
#include "cl.hpp"
#include "util.hpp"
#include "err_code.h"
#include "device_picker.hpp"
#define SUCCESS  1
#define FAILURE  0

using namespace std;

cl::Context Para::context;
cl::CommandQueue Para::queue;
cl::Program Para::programInflu;
cl::Program Para::programInflu_lit;
cl::Program Para::programMC;

Para::Para() {

}

Para::Para(const Para &orig) {

}

Para::~Para() {

}

Para::Para(int argc, char** argv) {
    cl_uint deviceIndex = 0;
    parseArguments(argc, argv, &deviceIndex);

    // Get list of devices
    vector<cl::Device> devices;
    unsigned numDevices = getDeviceList(devices);

    cl::Device device = devices[deviceIndex];

    string name;
    getDeviceName(device, name);
    cout << "\nUsing OpenCL device: " << name << "\n";

    vector<cl::Device> chosen_device;
    chosen_device.push_back(device);
    context = cl::Context(chosen_device);
    queue = cl::CommandQueue(context, device);
    programInflu = cl::Program(context, util::loadProgram("../computeInflu.cl"), true);
    programInflu_lit = cl::Program(context, util::loadProgram("../computeInflu_lit.cl"), true);
    programMC = cl::Program(context, util::loadProgram("../computeMC.cl"), true);  
}

vector< pair<string, float> > Para::sortMap(map<string, float> influTemp) {
    vector< pair<string, float> > sortedInflu;
    for(map<string, float>::iterator it = influTemp.begin(); it != influTemp.end(); it++) {
        sortedInflu.push_back(make_pair(it->first, it->second));
    }
    sort(sortedInflu.begin(), sortedInflu.end(), [] (const pair<string, float> & lhs, const pair<string, float> & rhs) {
        return lhs.second > rhs.second;
    });
    return sortedInflu;
}

void Para::p_setInfluence_lit(vector< map<string, double> > lambda) {
	//prepare for the buffers here
	map <string, float> para_influence;
	vector <int> h_lambdas(0);
	vector <float> h_lambdap(0);
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
    vector <float> h_influence(index1);//int literals = index1;
	cout<< "index1= "<<index1<<" "<<"size= "<<size<<" "<<"dim1_size= "<<dim1_size<<endl;	

	cl::Buffer d_lambdas, d_lambdap, d_dim2_size, d_influence;
	d_lambdas = cl::Buffer(context, h_lambdas.begin(), h_lambdas.end(), true);
	d_lambdap = cl::Buffer(context, h_lambdap.begin(), h_lambdap.end(), true);
	d_dim2_size = cl::Buffer(context, h_dim2_size.begin(), h_dim2_size.end(), true);
	d_influence = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * index1);

	cl::make_kernel<int, int, int, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer> setInfluence_lit(programInflu_lit, "setInfluence_lit");
	cl::NDRange global(index1);
    setInfluence_lit(cl::EnqueueArgs(queue, global), index1, size, dim1_size, d_lambdas, d_lambdap, d_dim2_size, d_influence);
    queue.finish();		
	cl::copy(queue, d_influence, h_influence.begin(), h_influence.end());	
	
    for(map<string, int>::const_iterator it = str2index.begin(); it != str2index.end(); ++it){
		para_influence[it->first] = h_influence[str2index[it->first]];
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
	
	this->influence = Para::sortMap(para_influence);
	cout << "Parallel Most Influential Tuple: " << endl;
	cout << influence[0].first << " " << influence[0].second <<endl;
}

void Para::p_setInfluence(vector< map<string, double> > lambda) {
	//prepare for the buffers here
	map <string, float> para_influence;
	vector <int> h_lambdas(0);
	vector <float> h_lambdap(0);
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
	    vector <float> h_parameters(0);	
	    std::default_random_engine generator;
		std::uniform_real_distribution<float> distribution(0.0,1.0);
		for(int i = 0; i < count; i++){
		    for(int j = 0; j < index1; j++){
		        float prand = distribution(generator);
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
	
	this->influence = Para::sortMap(para_influence);
	cout << "Parallel Most Influential Tuple: " << endl;
	cout << influence[0].first << " " << influence[0].second <<endl;
}

double Para::p_monteCarloSim(vector< map<string, double> > lambda) {
	//prepare for the buffers here
	vector <int> h_lambdas(0);
	vector <float> h_lambdap(0);
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
	cl::make_kernel<int, int, int, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer> getMC(programMC, "getMC");    
	//output the parallel MC result	
    vector <float> h_parameters(0);	
    std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(0.0,1.0);
	for(int i = 0; i < count; i++){
	    for(int j = 0; j < index1; j++){
	        float prand = distribution(generator);
	        h_parameters.push_back(prand);
	    }
	}				

	cl::Buffer d_parameters;
	d_parameters = cl::Buffer(context, h_parameters.begin(), h_parameters.end(), true);

	cl::NDRange global(count);
	getMC(cl::EnqueueArgs(queue, global), index1, size, dim1_size, d_lambdas, d_lambdap, d_dim2_size, d_parameters, d_resultOnce);
	queue.finish();		
	cl::copy(queue, d_resultOnce, h_resultOnce.begin(), h_resultOnce.end());		
	return std::count(h_resultOnce.begin(), h_resultOnce.end(), 1)*1.0/count;
}





