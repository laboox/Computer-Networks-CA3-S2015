#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
#include <exception>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <map>
#include <sstream>
#include <time.h> 
#include <bitset>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <dirent.h>
#include <set>

#define sh(x) cerr<<#x<<" = "<<x<<endl;

#define MSG_LEN 48
#define STDIN 0

using namespace std;

typedef pair<int, int> pii;

class Exeption {
private:
	string error;
public:
	Exeption(string err): error(err) {};
	string get_error() {return error;}
};

extern int unique_addr;
