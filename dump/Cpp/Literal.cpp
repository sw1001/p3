/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Literal.cpp
 * Author: shaobo
 * 
 * Created on October 18, 2018, 1:29 AM
 */

#include "Literal.h"

Literal::Literal() {
}

//Literal::Literal(const Literal& orig) {
//}

Literal::~Literal() {
}

Literal::Literal(string s, double d){
    prob = d;
    name = s;
}

string Literal::getName() const {
    return name;
}

double Literal::getProb() const {
    return prob;
}

void Literal::setName(string s) {
    name = s;
}

void Literal::setProb(double p) {
    prob = p;
}

