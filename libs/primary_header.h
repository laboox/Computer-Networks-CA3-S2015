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
#include <cstdio>
#include <dirent.h>

#define sh(x) cerr<<#x<<" = "<<x<<endl;

#define STDIN 0

#define MSG_LEN 48
#define STDIN 0

#define SERVER_ADDR 1000

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
