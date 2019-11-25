#include <iostream>
#include <iomanip>
#include <string>
#include <unistd.h>

#include "LeachController.hpp"
#include <chrono>
#include <thread>


// ------------------------------------------------------
//  Main program
// ------------------------------------------------------
int main( int argc, char **argv )
{


    std::string configFileName;

    /*Now get the args*/
    if (argc<2) {
       std::cout << "Default usage: ./CCDDPerformEraseProcedure <config file>. ";
       std::cout << "No config file was specified. Using config/Config.ini\n";

       configFileName = "config/Config.ini";
    } else {
       configFileName = std::string(argv[1]);
    }

    
    std::cout << "This code will perform an erase procedure.\n"
              << "The process starts in 10 seconds." <<std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(10));


	LeachController _ThisRunControllerInstance(configFileName);

	/*First, check if the settings file has changed in any way*/
	bool config,sequencer;
	int _CCDSettingsStatus = _ThisRunControllerInstance.LoadAndCheckForSettingsChange(config, sequencer);

    if (_CCDSettingsStatus == 0){
        std::this_thread::sleep_for(std::chrono::seconds(5));
        _ThisRunControllerInstance.PerformEraseProcedure();
        _ThisRunControllerInstance.IdleClockToggle();
        std::cout<<"Leach system is now ready to take data.\n";
    } else {
        if (config) std::cout<<"Error: The config file has changed but the new settings were not uploaded.\n";
        if (sequencer) std::cout<<"Error: The sequencer has changed but it was not uploaded.\n";
        std::cout<<"Erase procedure was not performed. Please resolve the conflicts in the config section first.\n";
    }





}

