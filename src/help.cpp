#include <config.hpp>
#include <iostream>
#include <cstdlib>

void help () {
	std::cout << __APP__ << std::endl;
	//const char* command = getenv("_");
	std::cout << "\t" << __APPNAME__ << " server -c <config.xml> (to invoke server)" << std::endl;
	std::cout << "\t" << __APPNAME__ << " update -c <config.xml> (to update server configuration on the fly)" << std::endl;
	std::cout << "\t" << __APPNAME__ << " sample (to dump sample server config file)" << std::endl;
	std::cout << "\t" << __APPNAME__ << " kill -c <config.xml> (to kill particular configuration with cleanup)" << std::endl;
	std::cout << "\t" << __APPNAME__ << " killall (to kill all with cleanup)" << std::endl;
	std::cout << "\t" << __APPNAME__ << " status (check the status of all logging with read/write and buffer)" << std::endl;
	std::cout << "\t" << __APPNAME__ << " help/h/-h (show help)" << std::endl << std::endl;
	return;
}
