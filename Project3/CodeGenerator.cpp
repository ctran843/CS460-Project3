#include <iostream>
#include <fstream>
#include "CodeGenerator.h"
#include "Object.h"

using namespace std;

/********************************************************************************/
/* This function will								*/
/********************************************************************************/
CodeGen::CodeGen (string filename)
{
	string cppname = filename.substr (0, filename.length()-3) + ".cpp"; 
	cpp.open (cppname);
	
	cpp << "// Autogenerated Scheme to C++ Code\n";
	cpp << "// File: " << cppname << "\n";
	cpp << "#include <iostream>\n";
	cpp << "#include \"Object.h\"\n";
	cpp << "using namespace std;\n\n";
}

/********************************************************************************/
/* This function will								*/
/********************************************************************************/
CodeGen::~CodeGen ()
{
	cpp.close();
}

/********************************************************************************/
/* This function will								*/
/********************************************************************************/
void CodeGen::WriteCode (int tabs, string code)
{
	for (int t = 0; t < tabs; t++)
		cpp << '\t';
	cpp << code;
}
