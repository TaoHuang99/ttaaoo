#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include "tinyxml2.h"
#include <cstdio>
// #include "tinyxml2.cpp"

using namespace std;
using namespace tinyxml2;


int createXML(const char * xmlPath,int status_num);

int addXML(const char *xmlPath);

int modifyXML(const char *xmlPath);