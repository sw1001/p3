/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Literal.h
 * Author: shaobo
 *
 * Created on October 18, 2018, 1:29 AM
 */

#ifndef LITERAL_H
#define LITERAL_H

#include <string>
#include <iostream>

using namespace std;

class Literal {
public:
    Literal();
    //Literal(const Literal& orig);
    virtual ~Literal();
    Literal(string, double);
    void setProb(double);
    double getProb() const;
    void setName(string);
    string getName() const;
private:
    double prob;
    string name;

};

#endif /* LITERAL_H */

