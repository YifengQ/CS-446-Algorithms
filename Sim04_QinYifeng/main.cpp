#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "config.h"
#include "meta.h"
#include "simulation.h"
#include "PCB.h"
using namespace std;

int main(int argc, char *argv[]) {

    string conf;

    ///checks if there is a command line argument
    if (argc > 1) {
        string arg1(argv[1]); ///gets the first command line argument and turns it into a string
        conf = arg1;
    }else{
        cout << "Could not find command line argument or more than one" << endl;
        return 0;
    }

    config config1;

    ///checks if configuration file has error
    bool valid = config1.setData(conf);

    ///decides where to print the data

    if(!valid) { /// prints only if there is an error

        if (config1.getlogto() == 1) { /// only to file
            config1.log_config_file();
            cout << "Logged to File Only" << endl;
        } else if (config1.getlogto() == 3) { /// only to monitor
            cout << "monitor" << endl;
            config1.log_config_monitor();
            ofstream log; // open and clear data
            log.open(config1.getPath());
            log.clear();
            log.close();
        } else if (config1.getlogto() == 2) { /// both
            config1.log_config_file();
            config1.log_config_monitor();
        } else {
            ///If there is error before where to print was decided
            cout << "//////////////////////////////////////////////////////" << endl;
            cout << "Error Before Print Path Was Given. Default to Monitor" << endl;
            cout << "/////////////////////////////////////////////////////" << endl;
            config1.log_config_monitor();
        }
        return 0;
    }

    simulation sim1(config1);

    meta meta1;

    meta1.readMeta(config1, sim1);///reads the data and passes the class config to the meta class

    if (meta1.ifError()) { ///prints only if error
        if (config1.getlogto() == 1) { ///only to file
            meta1.printToFile(config1);
        } else if (config1.getlogto() == 3) { /// only to terminal
            meta1.printMeta();
        } else if (config1.getlogto() == 2) { /// both
            meta1.printMeta();
            meta1.printToFile(config1);
        }
        return 0;
    }

    PCB PCB1; ///creation of PCB object
    sim1.popFrontVector();

    sim1.sortSched(); ///sorts the processes based on algorithm
    sim1.run(PCB1); /// runs based on the sorted algorithm

    if (config1.getlogto() == 1) { ///only to file
        sim1.printFile(config1);
    } else if (config1.getlogto() == 3) { /// only to terminal
        sim1.printMonitor();
    } else if (config1.getlogto() == 2) { /// both
        sim1.printMonitor();
        sim1.printFile(config1);
    }

    return 0;
}
