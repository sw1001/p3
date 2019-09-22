/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DNF.h
 * Author: shaobo, chia-hsuan
 *
 * Created on October 12, 2018, 5:14 AM
 */

#ifndef DNF_H
#define DNF_H

#include <vector>
#include <string>
#include <set>
#include <map>
#include "Literal.h"

using namespace std;

enum Operator {OR, AND};

class TreeNode {
public:
    TreeNode() {
            str = "";
    }

    TreeNode(string s) {
            str = s;
    }

    ~TreeNode() {}
                
private:
    string str;
    Operator op;
    vector<TreeNode*> children;
    map< string, set<string> > maps;
    friend class DNF;
};


class DNF {
public:
    DNF();
    DNF(const DNF& orig);
    virtual ~DNF();
    DNF(string, map<string, double>);
    vector< vector<Literal> > getLambda();
    void setLambda(map<string, double>);
    map<string, double> getProbs();
    void setProbs(map<string, double>);
    //converting
    
    void ConvertToDNF();
		
    TreeNode* BuildTree(TreeNode*);

    void RemoveRedundant(TreeNode*);

    bool IsVariable(string s);

    void ConvertToVector(TreeNode*);

    string ConvertSetToString(set<string> set);

    bool Evaluate();

    void ShowStructure();

    string ToString();
private:
    vector< vector<Literal> > lambda;
    map<string, double> probs;
    vector< vector<string> > dnf_vector;
    string str;
    TreeNode* root;
};



#endif /* DNF_H */

