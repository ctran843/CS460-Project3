// Autogenerated Scheme to C++ Code
// File: P3-49.cpp
#include <iostream>
#include "Object.h"
using namespace std;


Object function1 (Object V, Object L)
{
	Object __RetVal;
;
	__RetVal = listop ("cons", V, L);
	return __RetVal;
}

int main ()
{
	Object __RetVal;
;
	cout << function1(Object(3)  , Object("(1 3 )") );
	cout << endl;
	return 0;
}
