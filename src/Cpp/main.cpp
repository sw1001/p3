/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: shaobo, chenyuan
 *
 * Created on October 12, 2018, 3:32 AM
 * Updated on July 22, 2019
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <cctype>
#include <cmath>
#include "DNF.h"
#include "Suff.h"
#include <time.h>

//include all related files for opencl
#define __CL_ENABLE_EXCEPTIONS
#include "cl.hpp"
#include "util.hpp"
#include "err_code.h"
#include "device_picker.hpp"
#define SUCCESS  1
#define FAILURE  0

using namespace std;
cl::Context context;
cl::CommandQueue queue;
cl::Program program;


int main(int argc, char** argv) {
//-------------------------------------------------------------------------------
// Create a context and a queue(or queues)
//-------------------------------------------------------------------------------

    cl_uint deviceIndex = 0;
    parseArguments(argc, argv, &deviceIndex);

    // Get list of devices
    vector<cl::Device> devices;
    unsigned numDevices = getDeviceList(devices);

    // Check device index in range
    if (deviceIndex >= numDevices)
    {
      cout << "Invalid device index (try '--list')\n";
      return EXIT_FAILURE;
    }

    cl::Device device = devices[deviceIndex];

    string name;
    getDeviceName(device, name);
    cout << "\nUsing OpenCL device: " << name << "\n";

    vector<cl::Device> chosen_device;
    chosen_device.push_back(device);
    context = cl::Context(chosen_device);
    queue = cl::CommandQueue(context, device);
    program = cl::Program(context, util::loadProgram("../C_setInfluence_v2.cl"), true);
    
//-------------------------------------------------------------------------------
// Prepare the data on the host
//-------------------------------------------------------------------------------    
        
    map<string, double> p;
    /*
    int sa = sizeof(arr_s35)/sizeof(arr_s35[0]);
    cout<<"number of literals: "<<sa<<endl;
    for (int i = 0; i < sa; i++) {
        string name = "x";
        stringstream ss;
        ss << (i + 1);
        name.append(ss.str());
        cout<<name<<" ";
        p[name] = arr_s35[i];
        cout<<p[name]<<endl;
    }
    */
    //code to read prov from files
    ifstream pfin("/home/sleepytodeath/rapidnet_v1.0/data/prov/mutualTrustPath1-2_20.txt");
    stringstream buffer;
    buffer << pfin.rdbuf();
    string prov = buffer.str();
    prov.erase(0,prov.find_first_not_of(" \t\r\n"));
    prov.erase(prov.find_last_not_of(" \t\r\n") + 1);
    while(prov.find("@n257") != string::npos){
    	prov.replace(prov.find("@n257"), 5, "*");
    }
    cout<<"prov: "<<prov<<endl;
    


    //code to read trust data from files
    ifstream fin("/home/sleepytodeath/rapidnet_v1.0/data/trust/sample_20.csv");
    string line;
    int i = 0;
    while (getline(fin, line))
    {
        istringstream sin(line);
        vector<string> fields;
        string field;
        while (getline(sin, field, ','))
        {
            fields.push_back(field);
        }
        if (fields[0] == "node1")
        {
            continue;
        }
        i++;
        string name;
        name.append("trust").append(fields[0]).append("-").append(fields[1]);
        double prob = stod(fields[2]);
        p[name] = prob;
        cout<<name<<" "<<p[name]<<endl;
    }
    cout<<"total number of element: "<<i<<endl;
    
    p["ra"] = 1.0;
    p["rb"] = 1.0;
    p["r0"] = 1.0;
    p["r1"] = 1.0;    
    p["r2"] = 1.0;
    p["r3"] = 1.0;
    p["r4"] = 1.0;
    p["r5"] = 1.0;
    p["r6"] = 1.0;
    cout<<endl;
   
    
	//try{ 							
	
//--------------------------------------------------------------------------------
// Sequential queries
//--------------------------------------------------------------------------------
			clock_t t1 = clock();
			DNF dnf (prov, p);
			t1 = clock()-t1;
			cout<<"DNF running time: "<<((float)t1)/CLOCKS_PER_SEC<<" seconds"<<endl;
			//dnf.ShowStructure();
			//cout<<dnf.ToString()<<endl;

			Suff suff;    
			//cout<<"Original formula: "<<endl;
			//suff.printProv(dnf.getLambda());
			double pLambda = suff.probMC(dnf.getLambda());
			cout<<"pLambda = "<<pLambda<<endl;


			clock_t tsuff = clock();
			double epsilon = 1*0.01*pLambda; // approximation error
			suff.setSuffProv(dnf.getLambda(), epsilon);
			tsuff = clock() - tsuff;
			cout<<"Suff lineage time: "<<((float) tsuff)/CLOCKS_PER_SEC<<" seconds"<<endl;
			cout<<"Sufficient lineage: "<<endl;
			suff.printProv(suff.getSuffProv());
			cout<<endl;


			cout<<"Influence:" <<endl;
			clock_t t2 = clock();
			//suff.setInfluence(dnf.getLambda());
			suff.setInfluence(suff.getSuffProv());
			t2 = clock() - t2;
			cout<<"Sequential influence running time: "<<((float) t2)/CLOCKS_PER_SEC<<" seconds"<<endl;
			Literal x = suff.maxInfluence();
			cout<<"Sequential maxInfluence Literal: "<<x.getName()<<" "<<x.getProb()<<endl;
			cout<<endl;
			
			
			/*
			//prepare for the buffers here
			vector <int> h_lambdas(0);
			vector <float> h_lambdap(0);
			vector <int> h_dim2_size(0);			
			vector < vector<Literal> > h_lambda;
			h_lambda = suff.getSuffProv();//dnf.getLambda();
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
			cout<< "index1= "<<index1<<" "<<"size= "<<size<<" "<<"dim1_size= "<<dim1_size<<endl<<endl;
			*/
										
			
			clock_t t3 = clock();
			//vector<Literal> vcl = suff.changedLiterals(dnf.getLambda(), 0.9);
			vector<Literal> vcl = suff.changedLiterals(suff.getSuffProv(), 0.9);
			t3 = clock() - t3;
			cout<<"Changed literals running time: "<<((float)t3)/CLOCKS_PER_SEC<<" seconds"<<endl<<endl;
			//cout<<"Changed Literals: "<<endl;
			//suff.print(vcl);
			
			suff.p_findMostInfl(suff.getSuffProv());
			
			
		
//----------------------------------------------------------------------------------
// Set up buffers, initialise them, and write them into global memory
//----------------------------------------------------------------------------------
			/*
			cl::Buffer d_lambdas, d_lambdap, d_dim2_size, d_influence;
			d_lambdas = cl::Buffer(context, h_lambdas.begin(), h_lambdas.end(), true);
			d_lambdap = cl::Buffer(context, h_lambdap.begin(), h_lambdap.end(), true);
			d_dim2_size = cl::Buffer(context, h_dim2_size.begin(), h_dim2_size.end(), true);
			d_influence = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * index1);
			*/
				
//----------------------------------------------------------------------------------
// Parallel queries
//----------------------------------------------------------------------------------
			/*
			//cl::Program program(context, util::loadProgram("../C_setInfluence_v2.cl"), true);
			//program.build(chosen_device);
			cl::make_kernel<int, int, int, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer> setInfluence(program, "setInfluence");
			clock_t tpara = clock();
			cl::NDRange global(index1);
		    setInfluence(cl::EnqueueArgs(queue, global), index1, size, dim1_size, d_lambdas, d_lambdap, d_dim2_size, d_influence);
		    queue.finish();		
			tpara = clock() - tpara;
			cout<<endl<<"Parallel influence running time: "<<((float) tpara)/CLOCKS_PER_SEC<<" seconds"<<endl;
			cl::copy(queue, d_influence, h_influence.begin(), h_influence.end());
			
			//output the parallel results
			int k = 0;
			map <string, double> para_influence;
			map <string, double> seq_influence = suff.getInfluence();
			for(map<string, int>::const_iterator it = str2index.begin(); it != str2index.end(); ++it){
				para_influence[it->first] = h_influence[str2index[it->first]];
				cout<< it->first << "  deltaInfl=" << abs(para_influence[it->first] - seq_influence[it->first]) <<"  ";
				cout<< it->first << "  paraInfl=" << para_influence[it->first] <<endl;
				k++;	
					
			}
			//cout<<"k="<<k<<endl;
			
			double max = 0.0;
			string max_name = "";
			for(map<string, double>::const_iterator it = para_influence.begin(); it != para_influence.end(); ++it) {
				if(it->second > max && it->first.compare("r1") != 0 && it->first.compare("r2") != 0 && it->first.compare("r3") != 0 && it->first.compare("ra") != 0) {
				    max = it->second;
				    max_name = it->first;
				}
			}
			cout<<"Parallel maxInfluence Literal: "<< max_name <<" "<< max <<endl;
			*/
			
		/*	
		}catch (cl::Error err)
		{
		    cout << "Exception\n";
		    cerr << "ERROR: "
	             << err.what()
	             << "("
	             << err_code(err.err())
	             << ")"
	             << endl;
	        if (err.err() == CL_BUILD_PROGRAM_FAILURE)
    			for (cl::Device dev : chosen_device)
				{
				  // Check the build status
				  cl_build_status status = program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(dev);
				  if (status != CL_BUILD_ERROR)
					continue;

				  // Get the build log
				  std::string device_name = dev.getInfo<CL_DEVICE_NAME>();
				  std::string buildlog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(dev);
				  std::cerr << "Build log for " << device_name << ":" << std::endl
						    << buildlog << std::endl;
			  	}
 			
		}*/

		return EXIT_SUCCESS;
}

