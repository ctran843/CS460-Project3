/*******************************************************************************
* File name:                                                                   *
* Project: CS 460 Project 2 Fall 2019                                          *
* Author(s):                                                                   *
* Date:                                                                        *
* Description: This file contains the header file for the Syntactical Analyzer.*
*******************************************************************************/

#ifndef SYN_H
#define SYN_H

#include <iostream>
#include <fstream>
#include "LexicalAnalyzer.h"
#include "CodeGenerator.h"

using namespace std;

class SyntacticalAnalyzer
{
    public:
	SyntacticalAnalyzer (char * filename);
	~SyntacticalAnalyzer ();
    private:
	int tabs;
    ofstream p2_file;
	LexicalAnalyzer * lex;
	CodeGen * cgen;
	token_type token;
	int program ();
	int more_defines ();
	int define ();
	int stmt_list ();
	int stmt ();
	int literal ();
	int quoted_lit ();
	int more_tokens ();
	int param_list ();
	int else_part ();
	int stmt_pair ();
	int stmt_pair_body ();
	int action ();
	int any_other_token ();
	bool isListop1;
	bool isListop2;
};

#endif
