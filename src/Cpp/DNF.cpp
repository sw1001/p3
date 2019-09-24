/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DNF.cpp
 * Author: shaobo, chia-hsuan 
 * 
 * Created on October 12, 2018, 5:14 AM
 */

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <iomanip>
#include "DNF.h"

DNF::DNF() {
}

DNF::DNF(const DNF& orig) {
}

DNF::~DNF() {
}

DNF::DNF(string s, map<string, double> p) {
    this->provStr = s;
    setProbs(p);
    root = new TreeNode(s);
    DNF::ConvertToDNF();
    DNF::setLambda(p);
}

vector< map<string, double > > DNF::getLambda() {
    return lambda;
}

void DNF::setLambda(map<string, double> p){
    //read in the string then split it into monomials.
    //loop through the string and convert it to a vector and store it in dnf
    int s = dnf_vector.size();
    for (int i = 0; i < s; i++) {
        map<string, double> mono;
        int si = dnf_vector[i].size();
        bool hasIDB = false;
        for (int j = 0; j < si; j++) {
            if (p.find(dnf_vector[i][j]) != p.end()) {
                mono[dnf_vector[i][j]] = p[dnf_vector[i][j]];
            } else {
                hasIDB = true;
                break;
            }
        }
        if(!hasIDB) {
            mono.erase("ra");
            mono.erase("rb");
            mono.erase("r0");
            mono.erase("r1");
            mono.erase("r2");
            mono.erase("r3");
            mono.erase("r4");
            mono.erase("r5");
            mono.erase("r6");
            lambda.push_back(mono);
        }
    }
}

map<string, double> DNF::getProbs() {
    return probs;
}

void DNF::setProbs(map<string, double> p) {
    this->probs = p;
}


void
DNF::ConvertToDNF() {
    BuildTree(root);
    ConvertToVector(root);
}

void
DNF::RemoveRedundant(TreeNode *node) {
    bool redundant = true;
    while (redundant) {
        if (node->str.length() > 0 && node->str[0] != '(') {
            return;
        }
        int parenthesis_count = 0;
        bool parenthesis_exist = false;
        for (size_t i=0; i<node->str.length(); i++) {
            if (node->str[i] == '(') {
                parenthesis_count++;
                parenthesis_exist = true;
            } else if (node->str[i] == ')') {
                parenthesis_count--;
                if (i != node->str.length() - 1 && parenthesis_count == 0) {
                    redundant = false;
                    break;
                } else if (i == node->str.length() - 1 && redundant) {
                    node->str = node->str.substr(1, (node->str).length()-2);
                }
            }
        }
        if (!parenthesis_exist) {
            redundant = false;
        }
    }
}

bool
DNF::IsVariable(string str) {
    for (size_t i=0; i<str.length(); i++) {
        if (str[i] == '(' || str[i] == ')' || str[i] == '+' || str[i] == '*') {
            return false;
        }
    }
    return true;
}

TreeNode*
DNF::BuildTree(TreeNode *node) {
    RemoveRedundant(node);
    if (IsVariable(node->str)) {
        set<string> set;
        set.insert(node->str);
        string key = ConvertSetToString(set);
        node->maps[key] = set;
        return node;
    }
    int parenthesis_count = 0;
    int start = 0;
    for (size_t i=0; i<=node->str.length(); i++) {
        if (node->str[i] == '(') {
            parenthesis_count++;
        } else if (node->str[i] == ')') {
            parenthesis_count--;
        }

        //cout << i << " " << parenthesis_count << endl;
        if ((i == node->str.length()) || (parenthesis_count == 0 && (node->str[i] == '+' || node->str[i] == '*'))) {
            //cout<<"start = "<<start<<endl;
            //cout<<"i = "<<i<<endl;
            //cout<<"substring = "<<(node->str).substr(start, i-start)<<endl;
            node->children.push_back(new TreeNode((node->str).substr(start, i-start)));                        
            start = i + 1;
            if (i < node->str.length()) {
                if (node->str[i] == '+') {
                    node->op = OR;
                } else {
                    node->op = AND;
                }
            }
        }
    }
    if (node->children.size() == 0) {
        return node;
    }
    for (size_t i=0; i < node->children.size(); i++) {
        //cout<<"i = "<<i<<endl;
        //cout<<node->str<<endl;
        node->children[i] = BuildTree(node->children[i]);
    }
    if (node->op == OR) {
        for (size_t i=0; i<node->children.size(); i++) {
            for (map< string, set<string> >::iterator it = node->children[i]->maps.begin(); it != node->children[i]->maps.end(); it++) {
                string key = ConvertSetToString((*it).second);
                node->maps[key] = (*it).second;
            }
        }
    } else {
        for (size_t i=0; i<node->children.size()-1; i++) {
            if (i == 0) { 
                for (map< string, set<string> >::iterator it = node->children[i]->maps.begin(); it != node->children[i]->maps.end(); it++) {
                    string key = ConvertSetToString((*it).second);
                    node->maps[key] = (*it).second;
                }
            }
            map< string, set<string> > new_map;
            for (map< string, set<string> >::iterator it1 = node->maps.begin(); it1 != node->maps.end(); it1++) {
                for (map< string, set<string> >::iterator it2 = node->children[i+1]->maps.begin(); it2 != node->children[i+1]->maps.end(); it2++) {
                    set<string> temp;
                    for (set<string>::iterator it3 = (*it1).second.begin(); it3 != (*it1).second.end(); it3++) {
                        temp.insert(*it3);
                    }
                    for (set<string>::iterator it3=(*it2).second.begin(); it3 != (*it2).second.end(); it3++) {
                        temp.insert(*it3);
                    }
                    string key = ConvertSetToString(temp);
                    new_map[key] = temp;
                }
            }
            node->maps = new_map;
        }
    }
    return node;
}

void
DNF::ConvertToVector(TreeNode* node) {
    for (map< string, set<string> >::iterator it1 = node->maps.begin(); it1 != node->maps.end(); it1++) {
        vector<string> vec;
        for (set<string>::iterator it2 = (*it1).second.begin(); it2 != (*it1).second.end(); it2++) {
            vec.push_back(*it2);
        }
        dnf_vector.push_back(vec);
    }
}

string
DNF::ConvertSetToString(set<string> string_set) {
    string result;
    for (set<string>::iterator it = string_set.begin(); it != string_set.end(); it++) {
        result = result + (*it) + "#";
    }
    return result;
}

bool
DNF::Evaluate() {
    return true;
}

void
DNF::ShowStructure() {
    for (size_t i=0; i<dnf_vector.size(); i++) {
        cout << setw(10) << "vector[" << i << "] ";
        for (size_t j=0; j<dnf_vector[i].size(); j++) {
            cout << setw(6) << dnf_vector[i][j];
            if (j != dnf_vector[i].size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
}

string
DNF::ToString(){
    string s = "";
    for (size_t i=0; i<dnf_vector.size(); i++) {
        s += "(";
        for (size_t j=0; j<dnf_vector[i].size(); j++) {
            s += dnf_vector[i][j];
            if (j != dnf_vector[i].size() - 1) {
                s += "*";
            }
        }
        s += ")";
        if (i != dnf_vector.size() - 1) {
            s += " + ";
        }
    }
    return s;
}