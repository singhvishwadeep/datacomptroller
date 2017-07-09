#include <iostream>
#include <string>
#include <stdio.h>
#include <regex>
#include <sys/stat.h>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
using namespace std;
namespace po = boost::program_options;
using boost::property_tree::ptree;
ptree pt;


int CheckFileExists (const string &name) {
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0);
}

int ParseArguments(int argc, char** argv, vector <map<string,string>> &action) {
	po::options_description desc("Allowed options");
	desc.add_options()
		("server", "invoking the server")
		("update", "updating the server")
		("file", po::value<string>(), "configuration file")
		("kill", "kill the server with particular configuration")
		("killall", "killall connections from the server")
		("status", "status of the server")
		("help", "help");
	po::variables_map vm;

    try {

    	po::store(po::parse_command_line(argc, argv, desc), vm);
    	po::notify(vm);

        if (vm.count("file") && !CheckFileExists(vm["file"].as<string>())) {
			cout << "File " << vm["file"].as<string>() << " not found." << endl;
			cout << desc << "\n";
			return 1;
        }

        if (vm.count("server") && vm.count("file")) {
			cout << "going to invoke serve along with file " << vm["file"].as<string>() << endl;
//			return 0;
		}
        if (vm.count("update") && vm.count("file")) {
			cout << "going to update serve along with file " << vm["file"].as<string>() << endl;
//			return 0;
		}
        if (vm.count("kill") && vm.count("file")) {
			cout << "going to kill server configuration using file " << vm["file"].as<string>() << endl;
//			return 0;
		}
        if (vm.count("killall")) {
			cout << "going to kill all server configurations " << endl;
//			return 0;
		}
        if (vm.count("status")) {
			cout << "going to display status of server " << endl;
//			return 0;
		}
        if (vm.count("sample")) {
			cout << "going to dump sample configuration file " << endl;
//			return 0;
		}
//        cout << desc << "\n";
//        return 1;
    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        cout << desc << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
        cout << desc << "\n";
        return 1;
    }

//    read_xml(vm["file"].as<string>(), pt);
//    cout << pt.get<std::string>("datacomptroller.log.tag") << endl;
//
//    try {
//    	cout << pt.get<std::string>("datacomptroller.log.tagnotfound") << endl;
//    } catch (...) {
//    	cout << "not found" << endl;
//    }


    ifstream r;
    r.open(vm["file"].as<string>());
    if (!r) {
        cerr << "Unable to open file " << vm["file"].as<string>();
        exit(1);
    }
    std::string line;
    regex e("^#.*$");
    map <string,string> temp;
    action.clear();
    while (getline(r, line))
    {
    	if (line == "" || regex_match(line,e)) {
    		continue;
    	}
    	if (line == "log") {
    		if (!temp.empty()) {
    			action.push_back(temp);
    		}
    		temp.clear();
    		temp["output"] = "";
    		temp["tag"] = "";
    		temp["port"] = "";
    		temp["filter"] = "";
    		temp["nofilter"] = "";
    		temp["rotation"] = "";
    		temp["instant"] = "";
    	}
    	if (regex_match(line,regex("^output:.*$"))) {
    		regex t("^output:(.*)$");
    		smatch m;
    		if (regex_search(line,m,t)) {
    			temp["output"] = m[1];
    		}
    	}
    	if (regex_match(line,regex("^tag:.*"))) {
    		regex t("^tag:(.*)$");
    		smatch m;
    		if (regex_search(line,m,t)) {
    			temp["tag"] = m[1];
    		}
    	}
    	if (regex_match(line,regex("^port:.*"))) {
    		regex t("^port:(.*)$");
    		smatch m;
    		if (regex_search(line,m,t)) {
    			temp["port"] = m[1];
    		}
    	}
    	if (regex_match(line,regex("^filter:.*"))) {
    		regex t("^filter:(.*)$");
    		smatch m;
    		if (regex_search(line,m,t)) {
    			temp["filter"] = m[1];
    		}
    	}
    	if (regex_match(line,regex("^nofilter:.*"))) {
    		regex t("^nofilter:(.*)$");
    		smatch m;
    		if (regex_search(line,m,t)) {
    			temp["nofilter"] = m[1];
    		}
    	}
    	if (regex_match(line,regex("^rotation:.*"))) {
    		regex t("^rotation:(.*)$");
    		smatch m;
    		if (regex_search(line,m,t)) {
    			temp["rotation"] = m[1];
    		}
    	}
    	if (regex_match(line,regex("^instant:.*"))) {
    		regex t("^instant:(.*)$");
    		smatch m;
    		if (regex_search(line,m,t)) {
    			temp["instant"] = m[1];
    		}
    	}
    }
    if (!temp.empty()) {
		action.push_back(temp);
	}
    r.close();
//    for (auto it = action.begin(); it != action.end(); it++ ) {
//    	for(auto it1=it->begin();it1!=it->end();++it1){
//    		cout << it1->first  << " - " << it1->second << endl;
//    	}
//    }
 // nice way to add xml parsing
    //http://www.boost.org/doc/libs/1_42_0/libs/property_tree/examples/debug_settings.cpp
//	if (argv[1] == NULL) {
//		help();
//	} else if (string(argv[1]) == "server" && string(argv[2]) == "-c") {
//		// start server
//	} else if (string(argv[1]) == "update" && string(argv[2]) == "-c") {
//		// update server
//	} else if (string(argv[1]) == "sample") {
//		// dump sample config file
//	} else if (string(argv[1]) == "kill" && string(argv[2]) == "-c") {
//		// kill config file
//	} else if (string(argv[1]) == "killall") {
//
//	} else if (string(argv[1]) == "status") {
//
//	} else if (string(argv[1]) == "help" || string(argv[1]) == "h" || string(argv[1]) == "-h") {
//
//		return 0;
//	} else {
//		help();
//	}
	return 0;
}
