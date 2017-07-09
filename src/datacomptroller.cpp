//============================================================================
// Name        : datacomptroller.cpp
// Project     : Data Comptroller
// Author      : Vishwadeep Singh
// Version     :
// Copyright   : copywrite @Vishwadeep Singh
// Description : main argument processing
//============================================================================

#include "arguments.hpp"
#include "validate.hpp"
#include "logger.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <regex>

using namespace std;

int main(int argc, char **argv) {
	vector <map<string,string>> action;
	if (ParseArguments(argc, argv, action)) {
		return 1;
	}
	if (validateRecord(action)) {
		return 1;
	}
	vector <logger> alllooggers;
	for (auto it = action.begin(); it != action.end(); it++ ) {
		map<string,string> tempm(it->begin(),it->end());
		int port = atoi(tempm["port"].c_str());
		int rotation = 0;
		if (tempm["rotation"] == "yes") {
			rotation = 1;
		}
	    string::size_type lastPos = tempm["output"].find_first_not_of(" ", 0);
	    string::size_type pos     = tempm["output"].find_first_of(" ", lastPos);
		vector<int> outmode; // 0(file), 1(socket)
		vector<string> outnames; // hostnames and file names
		vector<int> outports;
	    while (string::npos != pos || string::npos != lastPos)
	    {
	    	string str = tempm["output"].substr(lastPos, pos - lastPos);
	    	regex e("^.*,.*$");
	    	if (regex_match(str,e)) {
	    		outmode.push_back(1);
	    		regex t("^(.*),(.*)$");
				smatch m;
				if (regex_search(str,m,t)) {
					outnames.push_back(m[1]);
					string sstr = m[2];
					int port1 = atoi(sstr.c_str());
					outports.push_back(port1);
				} else {
					outnames.push_back(str);
					outports.push_back(0);
				}
	    	} else {
	    		outmode.push_back(0);
	    		outnames.push_back(str);
	    		outports.push_back(0);
	    	}
	        lastPos = tempm["output"].find_first_not_of(" ", pos);
	        pos = tempm["output"].find_first_of(" ", lastPos);
	    }

		for(auto it1=it->begin();it1!=it->end();++it1){
			cout << "Working on Config file - " << it1->first  << " - " << it1->second << endl;
		}
		alllooggers.push_back(logger(tempm["tag"], port, rotation, tempm["filter"], tempm["nofilter"], outmode, outnames, outports));
	}

	return 0;
}
