///
/// Created by Yifen on 2/16/2019.
///

#include "meta.h"
#include "config.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <algorithm>

/**
 * @brief Will take in the data from the config class and read the data from the meta file. It will then use the error checking to keep running or not.
 * @param x , class object
 * @pre Will take class object so the class can access config data.
 * @post Will set give lines to the other functions to save data.
 */
void meta::readMeta(config& x, simulation& sim) {

    string start ("StartProgramMeta-DataCode:");
    string end ("EndProgramMeta-DataCode.");

    ifstream metaFile;
    string line;
    string temp;
    string descriptor;

    string file = x.getMetaPath();

    metaFile.open(file); /// opens the file that was given in the config file

    ////Checks if Meta-Data File is Empty
    if(metaFile.peek() == std::ifstream::traits_type::eof()) { ///checks to see if the file is empty
        meta_data.emplace_back("Meta-Data File is Empty"); /// if empty stores the error
        cycles.push_back(0); /// gives dummy varaible to the cycles
        indexError = ++index; /// gives the index of where the error is
        return;
    }

    //// Start Program Meta-Data Code:
    getline(metaFile, line); /// checks the start by getting that line
    line = junk(line);
    if(line != start){ /// checks if the start line matches the correct format and spelling
        meta_data.emplace_back("Typo in \"Start Program Meta-Data Code:\" Line"); ///If not it store the error message
        cycles.push_back(0); ///gives dummy variable
        indexError = ++index; /// records the index for printing
        return;
    }


    getline(metaFile, line); /// gets the first line of the meta data to pass to the function
    line = junk(line);
    size_t pos = line.find_first_of(';') + 1; /// finds the first semi colon which represents the end of a meta data
    temp = line.substr (0, pos); /// then a temp variable stores the data for the meta data

    if(!checkStart(temp, sim)) /// calls a function to see if the meta data is correct
        return; /// if not it stops the program

    line.erase(0,pos); /// if it is correct it erases the data from the whole line
    pos = line.find_first_of(';') + 1; /// finds the next whole meta data by finding the semi colon
    temp = line.substr (0, pos); /// then it stores that data in a temp string
    while(saveData(temp, x, sim)) { ///recursive call until save data returns false, I pass the temp variable and the class

        if(line.empty()){ /// check it the line is empty
            meta_data.pop_back(); /// my program stored the last meta data of each line twice so i deleted one
            sim.popVector(); /// my program stored the last meta data of each line twice so i deleted one
            cycles.pop_back();
            index--;
            getline(metaFile, line); ///gets a new line from the file
            line = junk(line);
            if(line.empty() || line == end){ /// if it got an empty line, it didn't find finish data

                meta_data.emplace_back("Could Not Find Finish"); /// stores the error message
                cycles.push_back(0);
                indexError = ++index;
                return;
            }
        }
        else{ ///if not an empty line it will keep getting data from the string

            if(line.at(0) == 'S') {
                if (ifFinish(line)) {
                    meta_data.emplace_back("Error in S descriptor"); /// stores the error message
                    cycles.push_back(0);
                    indexError = ++index;
                    break;
                }else{
                    break;
                }
            }


            pos = line.find_first_of(';') + 1; ///will get another string from the original string
            if(pos > line.size()){
                meta_data.emplace_back("Syntax Error"); /// stores the error message
                cycles.push_back(0);
                indexError = ++index;
                return;
            }

            temp = line.substr(0, pos); /// will store it in another variable
            line.erase(0, pos); /// will get rid of that data
            //cout << line << endl;
        }

    }

    /*
     * /// checks for the end of program
     * /// checks for correct spelling and format
    getline(metaFile, line);
    line.erase(remove(line.begin(),line.end(),' '),line.end());

    if(line != end){
        meta_data.emplace_back("Error in \"End Program Meta-Data Code.\" Line");
        cycles.push_back(0);
        indexError = ++index;
        return;
    }
    */
    metaFile.close(); /// closes the file
}
/**
 * @brief It gets passed the start meta data string to check it all the spelling and formatting is correct.
 * @param str
 * @return bool, if the string is valid or not
 */
bool meta::checkStart(string str, simulation sim) {


    if(str.at(0) != 'S'){ //// checks if it starts with Character 'S'
        meta_data.emplace_back("Does not begin with Start"); /// if not, it is invalid and record error
        cycles.push_back(0);
        indexError = ++index; ///records index to print
        return false;
    }

    if(str.at(1) != '{'){ /// then it checks if '{' comes after
        meta_data.emplace_back("Error! Could not find \' { \' in Start");
        cycles.push_back(0);
        indexError = ++index;
        return false;
    }

    size_t pos = str.find('}') - 2; ///finds the position of the '}'
    if(pos + 2 > str.length()){ ///checks if there is a '}'
        meta_data.emplace_back("Error! Could not find \' } \' in Start");
        cycles.push_back(0);
        indexError = ++index;
        return false;
    }

    string descriptor = str.substr(2,pos); ///gets the descriptor between the '{' and '}'
    string c = str.substr(pos + 3); ///then gets the cycles after it
    c.pop_back(); ///gets rid of the ; at the end of cycles

    if(c.empty()) { ///checks if there is any cycles
        meta_data.emplace_back("Error! No Cycle");
        cycles.push_back(0);
        indexError = ++index;
        return false;
    }

    if(c.find_first_not_of( "0123456789" ) != std::string::npos){ ///checks if the cycle is a int value
        meta_data.emplace_back("Incorrect Cycles or Incorrect/Missing Colon For Start");
        cycles.push_back(0);
        indexError = ++index;
        return false;
    }
    return true;
}
/**
 * @brief Will take in the string and check if the data is correct. If correct it will save and calculate the data
 * @param str
 * @param x
 * @return bool, whether there is a error or not
 */
bool meta::saveData(string str , config& x, simulation& sim) {

    ///checks if the meta code is a valid one
    if(str.at(0) != 'S' && str.at(0) != 'A' && str.at(0) != 'P' && str.at(0) != 'I' && str.at(0) != 'O' && str.at(0) != 'M'){
        meta_data.emplace_back("Typo !!! Incorrect Meta Data Code or Missing Meta Data  ");
        cycles.push_back(0);
        indexError = ++index;
        return false;
    }

    ///if valid it checks if there is a '{' after
    if(str.at(1) != '{'){
        meta_data.emplace_back("Error! Could not find \' { \' ");
        cycles.push_back(0);
        indexError = ++index;
        return false;
    }


    size_t pos = str.find('}') - 2; /// finds the position of the '}'

    if(pos + 2 > str.length()){ /// checks that it found a '}'
        meta_data.emplace_back("Error! Could not find \' } \' ");
        cycles.push_back(0);
        indexError = ++index;
        return false;
    }

    string descriptor = str.substr(2,pos); ///gets the descriptor between '{' and '}'

    if(descriptor.empty()){ ///checks if descriptor is empty, if empty it errors out
        meta_data.emplace_back("Error! Descriptor is Empty");
        cycles.push_back(0);
        indexError = ++index;
        return false;
    }

    string c = str.substr(pos + 3); ///gets the cycles after '}' in string form
    c.pop_back(); /// removes the ';' from end of the string

    if(c.empty()) { ///checks if string is empty
        meta_data.emplace_back("Error! No Cycle");
        cycles.push_back(0);
        indexError = ++index;
        return false;
    }

    /// checks if the the string has only integers. Makes sure its not negative
    if(c.find_first_not_of( "0123456789" ) != std::string::npos){
        meta_data.emplace_back("Error! Invalid Cycles or Incorrect/Missing Colon");
        cycles.push_back(0);
        indexError = ++index;
        return false;
    }

    int cycle = atoi(c.c_str()); /// turns the string into a int

    str.pop_back(); /// gets rid of the ';' at end of string

    string code;
    code.push_back(str.at(0));


    if(str.at(0) == 'A'){  ///checks if the code is 'A'

        if(setApp(descriptor)){ /// if 'A' calls function to check formatting and spelling
            sim.setVector(code, descriptor, 0); /// adds process to simulation class vector
            return true;
        }
        else{ ///if not valid it errors out
            meta_data.emplace_back("Error! Invalid Descriptor in Meta Data Code A");
            cycles.push_back(0);
            indexError = ++index;
            return false;
        }

    } ///checks if the code is 'P'

    else if(str.at(0) == 'P') {

        /// if valid calls function to check validity, and calculates the cycles total
        int temp = setProcess(descriptor, cycle, x);
         if( temp > 0){ ///if error function returns -1 so if value is greater that means valid
             meta_data.push_back(str); /// if valid it adds the string to the vector
             cycles.push_back(temp); /// adds the total cycles to vector
             sim.setVector(code, descriptor, temp); /// adds process to simulation class vector
             index++; /// increments the index for printing
             return true;
         } else{
             meta_data.emplace_back("Error! Invalid Descriptor in Meta Data Code P"); /// else it errors out
             cycles.push_back(0);
             indexError = ++index;
             return false;
         }
    }///checks for 'I'
    else if(str.at(0) == 'I') {
    /// if valid calls function to check validity, and calculates the cycles total
        int temp = setInput(descriptor, cycle, x);
        if( temp > 0){///if error function returns -1 so if value is greater that means valid
            meta_data.push_back(str);/// if valid it adds the string to the vector
            cycles.push_back(temp);/// adds the total cycles to vector
            sim.setVector(code, descriptor, temp); /// adds process to simulation class vector
            index++;/// increments the index for printing
        } else{
            meta_data.emplace_back("Error! Invalid Descriptor in Meta Data Code I");/// else it errors out
            cycles.push_back(0);
            indexError = ++index;
            return false;
        }
    }///checks for 'O' , same as above
    else if(str.at(0) == 'O') {
    /// if valid calls function to check validity, and calculates the cycles total
        int temp = setOutput(descriptor, cycle, x);
        if( temp > 0){
            meta_data.push_back(str);
            cycles.push_back(temp);
            sim.setVector(code, descriptor, temp); /// adds process to simulation class vector
            index++;
        } else{
            meta_data.emplace_back("Error! Invalid Descriptor in Meta Data Code O");
            cycles.push_back(0);
            indexError = ++index;
            return false;
        }
    }///checks for 'N' , same as above
    else if(str.at(0) == 'M') {
    /// if valid calls function to check validity, and calculates the cycles total
        int temp = setMemory(descriptor, cycle, x);
        if( temp > 0){
            meta_data.push_back(str);
            cycles.push_back(temp);
            sim.setVector(code, descriptor, temp); /// adds process to simulation class vector
            index++;
        } else{
            meta_data.emplace_back("Error! Invalid Descriptor in Meta Data Code M");
            cycles.push_back(0);
            indexError = ++index;
            return false;
        }
    }///checks for 'S', this should contain the finish descriptor
    else if(str.at(0) == 'S') {
/// if valid calls function to check validity
        if(checkFinish(descriptor)){ /// if it contains false end the function
            return false;
        }
        else{
            meta_data.emplace_back("Error! Invalid Descriptor Finish"); /// if not valid , error out
            cycles.push_back(0);
            indexError = ++index;
            return false;
        }
    }
    return true;
}

/**
 * @brief Takes the descriptor given and tests it to valid strings
 * @return bool, if descriptor is valid
 */
bool meta::setApp(string str) {

    string begin ("begin");
    string finish ("finish");
    return str == begin || str == finish;

}
/**
 * @brief Takes the descriptor given and tests it to valid strings. If it is valid it will calculate the total cycles by accessing the cycles it needs from the other class
 * @return bool, If invalid descriptor it will return -1 which flags it as invalid for the other function to handle
 */
int meta::setProcess(string str, int cyc, config& x) {

    string run ("run");

    if(str == run){
        return x.getProcessor() * cyc;
    }
    else{
        return -1;
    }
}

/**
* @brief Takes the descriptor given and tests it to valid strings. If it is valid it will calculate the total cycles by accessing the cycles it needs from the other class
* @return bool, If invalid descriptor it will return -1 which flags it as invalid for the other function to handle
*/
int meta::setInput(string str, int cyc, config& x) {

    string HD ("harddrive");
    string keyboard ("keyboard");
    string scanner ("scanner");

    if(str == HD){
        return x.getHD() * cyc;
    }
    else if(str == keyboard){
        return x.getKeyboard() * cyc;
    }
    else if(str == scanner){
        return x.getScanner() * cyc;
    }
    else{
        return -1;
    }
}
/**
 * @brief Takes the descriptor given and tests it to valid strings. If it is valid it will calculate the total cycles by accessing the cycles it needs from the other class
 * @return bool, If invalid descriptor it will return -1 which flags it as invalid for the other function to handle
 */
int meta::setOutput(string str, int cyc, config& x) {

    string HD ("harddrive");
    string monitor ("monitor");
    string projector ("projector");

    if(str == HD){
        return x.getHD() * cyc;
    }
    else if(str == monitor){
        return x.getMonitor() * cyc;
    }
    else if(str == projector){
        return x.getProjector() * cyc;
    }
    else{
        return -1;
    }
}
/**
 * @brief Takes the descriptor given and tests it to valid strings. If it is valid it will calculate the total cycles by accessing the cycles it needs from the other class
 * @return bool, If invalid descriptor it will return -1 which flags it as invalid for the other function to handle
 */
int meta::setMemory(string str, int cyc, config& x) {

    string allocate ("allocate");
    string block  ("block");

    if(str == allocate || str == block){
        return x.getMemory() * cyc;
    }
    else{
        return -1;
    }
}

/**
 * @brief Prints the data up to and index point in terminal
 * The string and cycle are stored in vector so it prints it out in a for loop
 * If there is an error, there is an error index that the index compares itself to
 * If they match that will end the printing.
 * the variable index is the total amount of strings
 */
void meta::printMeta() {

    cout << endl << "Meta-Data Metrics" << endl;

    for (int i = 0; i < index ; ++i) {

        if(i == indexError - 1) {
            cout << meta_data[i] << endl;
            return;
        }

        cout << meta_data[i] << " - " << cycles[i] << " ms" << endl;
    }
}
/**
 *@brief default constructor, sets integers to set values
 */
meta::meta() {

    index = 0;
    indexError = -1;
}
/**
 * @brief Takes the descriptor given and tests it to valid strings
 * @return bool, if descriptor is valid
 */
bool meta::checkFinish(string str) {
    return str == "finish";
}

/**
 * @brief Takes in a string and gets rid of unwanted invisible characters or white space
 * @param content ,string)
 * @return string, without the characters
 */
string meta::junk(string content) {

    content.erase(remove(content.begin(),content.end(),'\r'),content.end()); /// had a problem with '/r' being in my lines so I decided to remove any
    content.erase(remove(content.begin(),content.end(),'\n'),content.end()); /// erases the beginning because in my config file, all lines had character in front
    content.erase(remove(content.begin(),content.end(),' '),content.end()); ///removes all the white space, so white space spacing doesn't matter
    return content;

}

/**
 * @brief Prints the data up to and index point in file
 * The string and cycle are stored in vector so it prints it out in a for loop
 * If there is an error, there is an error index that the index compares itself to
 * If they match that will end the printing.
 * the variable index is the total amount of strings
 */
void meta::printToFile(config& x) {

    ofstream log;
    log.open(x.getPath(), std::ios_base::app);

    log << endl << "Meta-Data Metrics" << endl;
    for (int i = 0; i < index ; ++i) {

        if(i == indexError - 1) {
            log << meta_data[i] << endl;
            return;
        }

        log << meta_data[i] << " - " << cycles[i] << " ms" << endl;
    }

    log.close();
}

bool meta::ifFinish(string str) {

    if(str.at(0) != 'S'){ //// checks if it starts with Character 'S'
        meta_data.emplace_back("Does not begin with Start"); /// if not, it is invalid and record error
        cycles.push_back(0);
        indexError = ++index; ///records index to print
        return false;
    }

    if(str.at(1) != '{'){ /// then it checks if '{' comes after
        meta_data.emplace_back("Error! Could not find \' { \' in Start");
        cycles.push_back(0);
        indexError = ++index;
        return false;
    }

    size_t pos = str.find('}') - 2; ///finds the position of the '}'
    if(pos + 2 > str.length()){ ///checks if there is a '}'
        meta_data.emplace_back("Error! Could not find \' } \' in Start");
        cycles.push_back(0);
        indexError = ++index;
        return false;
    }

    string descriptor = str.substr(2,pos); ///gets the descriptor between the '{' and '}'
    string c = str.substr(pos + 3); ///then gets the cycles after it
    c.pop_back(); ///gets rid of the ; at the end of cycles

    if(c.empty()) { ///checks if there is any cycles
        meta_data.emplace_back("Error! No Cycle");
        cycles.push_back(0);
        indexError = ++index;
        return false;
    }

    if(c.find_first_not_of( "0123456789" ) != std::string::npos){ ///checks if the cycle is a int value
        meta_data.emplace_back("Incorrect Cycles or Incorrect/Missing Colon For Start");
        cycles.push_back(0);
        indexError = ++index;
        return false;
    }

    if(descriptor == "finish")
        return false;

    return true;

}

bool meta::ifError() {

    if(indexError != -1) {
        return true;
    } else{
        return false;
    }
}
