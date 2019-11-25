#include <iostream>
#include <iomanip>
#include <string>
#include <unistd.h>

#include "LeachController.hpp"
#include <chrono>
#include <thread>


// ------------------------------------------------------
//  Main program
//  Toggle the substrate bias using the battery box relay
// ------------------------------------------------------
int main( int argc, char **argv )
{


    std::string configFileName = "";

    std::string direction = argc > 1 ? argv[1] : "";
    /*Now get the args*/
    if (direction != "on" && direction != "1" 
        && direction != "off" && direction != "0") {
       std::cerr << "Usage: ./CCDDToggleBias <on|off> [<volts=4.88>]";
       return 1;
    } 
    
    bool on = (direction == "on" || direction == "1");
    double volts = argc > 2 ? atof(argv[2]) : 4.88;
    
    std::cout << "Switching relay bias "<< (on ? "on" : "off") <<"...";


    LeachController _ThisRunControllerInstance(configFileName);
    _ThisRunControllerInstance.BiasParams.battrelay = volts;
    _ThisRunControllerInstance.CCDBiasToggle(on);
    
    std::cout<<" OK!"<<std::endl;
        
    return 0;

}

