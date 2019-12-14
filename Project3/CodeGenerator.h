// Project: CS 460 Fall 2019 Project 3
// File: CodeGenerator.h
// Authors: Modified by Andrew Langwell, Chris Tran, Lizzy Axworthy, Christopher DeGuzman, Origial File Provided By Dr. Watts
// Desciption: Code generator header file for Project 3.

#ifndef CG_H
#define CG_H

#include <iostream>
#include <fstream>

using namespace std;

class CodeGen 
{
    public:
	CodeGen (string filename);
	~CodeGen ();
	void WriteCode (int tabs, string code);
    private:
	ofstream cpp;
};
	
#endif
