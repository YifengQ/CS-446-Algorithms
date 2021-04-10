#include <iostream>
#include <fstream>
#include <string>
#include "config.h"
#include "meta.h"
using namespace std;

int main(int argc, char *argv[]) {

    string conf;

    ///checks if there is a command line argument
    if (argc > 1) {
        string arg1(argv[1]); //gets the first command line argument and turns it into a string
        conf = arg1;
    }else{
        cout << "Could not find command line argument or more than one" << endl;
        return 0;
    }

    config config1;

    ///checks if configuration file has error
    bool valid = config1.setData(conf);

    ///decides where to print the data

    if(config1.getlogto() == 1){ /// only to file
        config1.log_config_file();
        cout << "Logged to File Only" << endl;
    }
    else if(config1.getlogto() == 3){ /// only to monitor
        config1.log_config_monitor();

        ofstream log; // open and clear data
        log.open(config1.getPath());
        log.clear();
        log.close();
    }
    else if(config1.getlogto() == 2){ /// both
        config1.log_config_file();
        config1.log_config_monitor();
    }
    else if(!valid){
        //If there is error before where to print was decided
        cout << "//////////////////////////////////////////////////////" << endl;
        cout << "Error Before Print Path Was Given. Default to Monitor" << endl;
        cout << "/////////////////////////////////////////////////////" << endl;
        config1.log_config_monitor();

    }

    //cout <<boolalpha <<valid << endl;

    if(valid) { //makes sure there were no errors in config file
        meta meta1;

        meta1.readMeta(config1);//reads the data and passes the class config to the meta class

        if(config1.getlogto() == 1){ ///only to file
            meta1.printToFile(config1);
        }
        else if(config1.getlogto() == 3){ /// only to terminal
            meta1.printMeta();
        }
        else if(config1.getlogto() == 2) { /// both
            meta1.printMeta();
            meta1.printToFile(config1);
        }
    }

    return 0;
}
