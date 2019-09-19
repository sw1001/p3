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
vector<cl::CommandQueue> queues;
cl::Program program;


int main(int argc, char** argv) {
//-------------------------------------------------------------------------------
// Create a context and a queue(or queues)
//-------------------------------------------------------------------------------

    // Get list of devices
    vector<cl::Device> devices;
    unsigned numDevices = getDeviceList(devices);
    context = cl::Context(devices);//a context with all devices
    for (int i = 0; i < numDevices; i++) {
        string name;
        getDeviceName(devices[i], name);
        cout << "\nUsing OpenCL device: " << name << "\n";
        queues.push_back(cl::CommandQueue(context, devices[i]));//each device has a queue
    } 
    program = cl::Program(context, util::loadProgram("../C_setInfluence_wcz.cl"), true);
    
//-------------------------------------------------------------------------------
// Read in the test data
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
    string provfilename = "/home/chenyuan/p3/data/prov/prov_sample_500.txt";
    //string head = "sim" + provfilename.substr(provfilename.find_last_of("_") + 1, provfilename.find(".txt") - provfilename.find_last_of("_") - 1);
    ifstream pfin(provfilename);
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
    
    ifstream fin("/home/chenyuan/p3/data/trust/sample_500_shaobo.csv");
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
        //cout<<name<<" "<<p[name]<<endl;
    }
    cout<<"total number of element: "<<i<<endl;
    
    
    
    //code to read vqa data from files
    //need to change ...maxInfluence, p_findMostInfl(Suff.cpp)
    /*
    vector<string> filenames;
    filenames.push_back("/home/sleepytodeath/rapidnet_v1.0/data/vqa/eval/word_obs.txt");
    filenames.push_back("/home/sleepytodeath/rapidnet_v1.0/data/vqa/eval/sim_obs.txt");
    filenames.push_back("/home/sleepytodeath/rapidnet_v1.0/data/vqa/eval/hasq_obs.txt");
    filenames.push_back("/home/sleepytodeath/rapidnet_v1.0/data/vqa/eval/hasimg_obs.txt");
    for(int i = 0; i < filenames.size(); i++){
    	vector<string> re;
		string s;
		ifstream f(filenames[i], ios_base::in);
		while(!f.eof()){
			getline(f, s);
			re.push_back(s);
		}
		f.close();
		
		for(int j = 0; j < re.size(); j++){
			string line = re[j];
			if(line.size()==0) continue;
			vector<string> tokens;
			stringstream ss(line);      
			string intermediate;
			while(getline(ss, intermediate, '\t')) 
			{ 
		    	tokens.push_back(intermediate); 
			}
			double pp = atof(tokens.back().c_str());
			
			string pname("");
			switch(i){
				case 0 : 
					pname += "word";
					break;
				case 1 :
					pname += "sim";
					break;
				case 2 :
					pname += "hasQ";
					break;
				case 3 :
					pname += "hasImg";
					break;			
			}
			for(int k = 0; k < tokens.size() -1; k++){
				pname += tokens[k];
			}
			p[pname] = pp;		
			//cout<<"pname="<<pname<<" pp="<<p[pname]<<endl;
		}		
    }
    */    
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
   					
//--------------------------------------------------------------------------------
// Sequential queries
//--------------------------------------------------------------------------------
	clock_t t1 = clock();
	DNF dnf (prov, p);
	t1 = clock()-t1;
	cout<<"DNF running time: "<<((float)t1)/CLOCKS_PER_SEC<<" seconds"<<endl;
	cout<<"DNF number of monomials: "<<dnf.getLambda().size()<<endl;
	//dnf.ShowStructure();
	//cout<<endl<<dnf.ToString()<<endl<<endl;
	
	
	Suff suff;    
	//cout<<"Original formula: "<<endl;
	//suff.printProv(dnf.getLambda());
	double pLambda = suff.probMC(dnf.getLambda());
	cout<<"pLambda = "<<pLambda<<endl<<endl;

	/*
	clock_t tsuff = clock();
	double epsilon = 1*0.1*pLambda; // approximation error
	suff.setSuffProv(dnf.getLambda(), epsilon);
	tsuff = clock() - tsuff;
	cout<<"Suff lineage time: "<<((float) tsuff)/CLOCKS_PER_SEC<<" seconds"<<endl;
	cout<<"Sufficient lineage: "<<endl;
	//suff.printProv(suff.getSuffProv());
	cout<<endl;
	*/
	
	clock_t t2 = clock();
	suff.setInfluence(dnf.getLambda());
	//suff.setInfluence(suff.getSuffProv());
	t2 = clock() - t2;
	cout<<"Sequential influence running time: "<<((float) t2)/CLOCKS_PER_SEC<<" seconds"<<endl;
	//suff.sortInfluence("hasImg");
	//suff.sortInfluence("hasQ");
	//suff.sortInfluence("sim");
	//suff.sortInfluence("r");
	//cout<<endl<<"Unique tuples:"<<endl;
	//suff.sortInfluence(head);
	Literal x = suff.maxInfluence();
	cout<<"Sequential maxInfluence Literal: "<<x.getName()<<" "<<x.getProb()<<endl<<endl;
	
											
	/*
	clock_t t3 = clock();
	vector<Literal> vcl = suff.changedLiterals(dnf.getLambda(), 0.9);
	//vector<Literal> vcl = suff.changedLiterals(suff.getSuffProv(), 0.9);
	t3 = clock() - t3;
	cout<<"Sequential changed literals running time: "<<((float)t3)/CLOCKS_PER_SEC<<" seconds"<<endl<<endl;
	//cout<<"Changed Literals: "<<endl;
	//suff.print(vcl);
	*/
		
//----------------------------------------------------------------------------------
// Parallel queries
//----------------------------------------------------------------------------------
	
	clock_t tpara = clock();
	Literal maxInfl= suff.p_findMostInfl_wcz(dnf.getLambda());
	//Literal maxInfl= suff.p_findMostInfl(suff.getSuffProv());
	tpara = clock() - tpara;
	cout<<endl<<"Parallel influence running time: "<<((float) tpara)/CLOCKS_PER_SEC<<" seconds"<<endl;
	cout<<"Parallel maxInfluence Literal: "<< maxInfl.getName()<<" "<<"Infl="<< maxInfl.getProb() <<endl<<endl;
	
	
	/*
	clock_t t3 = clock();
	vector<Literal> vcl = suff.changedLiterals(dnf.getLambda(), 0.0234, head);
	//vector<Literal> vcl = suff.changedLiterals(suff.getSuffProv(), 0.9, head);
	t3 = clock() - t3;
	cout<<"Parallel changed literals running time: "<<((float)t3)/CLOCKS_PER_SEC<<" seconds"<<endl<<endl;
    */
	
	
	return EXIT_SUCCESS;
}

