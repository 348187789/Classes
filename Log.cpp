#include "Log.h"

#include <string>
#include <fstream>
#include <cassert>

using namespace std;

fstream& getStream()
{
	static fstream f("Log.txt");
	assert(f.is_open());
	return f;
}

void LOG(const char* str)
{
	assert(str);
	getStream()<<str<<endl;
}

void LOG(const string& str)
{
	getStream()<<str<<endl;
}