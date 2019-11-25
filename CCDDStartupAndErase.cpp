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
       std::cout << "Default usage: ./CCDDStartupAndErase <config file>. ";
       std::cout << "No config file was specified. Using config/Config.ini\n";

       configFileName = "config/Config.ini";
    } else {
       configFileName = std::string(argv[1]);
    }

    
    std::cout << "This code will power on the leach and apply the clock and bias voltages.\n"
              << "Then it will perform an erase procedure.\n"
              << "The process starts in 10 seconds. Please ensure that the Leach is switched ON."
              << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(10));


	LeachController _ThisRunControllerInstance(configFileName);

	/*First, check if the settings file has changed in any way*/
	std::cout<<"Checking for new settings and loading them.\n";
	bool config, sequencer;
	int _CCDSettingsStatus = _ThisRunControllerInstance.LoadAndCheckForSettingsChange(config, sequencer);

	/*Startup the CCD*/
	std::cout<<"Starting up the controller.\n";
    _ThisRunControllerInstance.StartupController();

	/*Apply biases and clocks*/
	std::cout<<"Applying biases and clocks.\n";
	_ThisRunControllerInstance.ApplyAllCCDBasic();
	_ThisRunControllerInstance.ApplyAllBiasVoltages();
	_ThisRunControllerInstance.ApplyAllCCDClocks();
	/*Store the file hashes since new settings were applied*/
	_ThisRunControllerInstance.CopyOldAndStoreFileHashes();


	/*Erase procedure*/
	std::cout<<"Set IDLE clocks to ON and then start erase procedure in 5 seconds." << std::endl;
    _ThisRunControllerInstance.IdleClockToggle();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    _ThisRunControllerInstance.PerformEraseProcedure();


    std::cout<<"Leach system is now ready to take data.\n";


}

