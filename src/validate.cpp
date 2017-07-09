#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

int validateRecord (vector <map<string,string>> &action) {
	vector <map<string,string>> temp;
	for (auto it = action.begin(); it != action.end(); it++ ) {
		map<string,string> tempm(it->begin(),it->end());
		if (tempm["tag"] != "" && tempm["port"] != "" && tempm["output"] != "") {
			if (tempm["rotation"] != "yes") {
				tempm["rotation"] = "no";
			}
			if (tempm["instant"] != "yes") {
				tempm["instant"] = "no";
			}
			temp.push_back(tempm);
		} else {
			for(auto it1=it->begin();it1!=it->end();++it1){
				cout << "Wrong Configuration - " << it1->first  << " - " << it1->second << endl;
			}
		}
	}
	action.swap(temp);
	if (action.empty()) {
		return 1;
	}
	return 0;
}
