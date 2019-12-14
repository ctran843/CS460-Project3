/*******************************************************************************
* File name: Project2.cpp                                                      *
* Project: CS 460 Project 2 Fall 2019                                          *
* Author(s): Andrew Langwell, Chris Tran, Lizzy Axworthy, Christopher DeGuzman, Original File Provided by Dr. Watts                                                         *
* Date: Fall 2019                                                              *
* Description: This file contains the driver program for Project 2 and Project3.            *
*******************************************************************************/

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "SetLimits.h"
#include "SyntacticalAnalyzer.h"

int main (int argc, char * argv[])
{
	if (argc < 2)
	{
		printf ("format: %s <filename>\n", argv[0]);
		exit (1);
	}
	SetLimits ();
	SyntacticalAnalyzer parse (argv[1]);

	return 0;
}
