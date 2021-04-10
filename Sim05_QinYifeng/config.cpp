//
// Created by yifengq on 2/11/19.
//

#include "config.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
/**
 * @brief Default Constructor, initialize variables to zero
 */
config::config() {

     monitor = 0;
     processor = 0;
     scanner = 0;
     hard_drive = 0;
     keyboard = 0;
     memory = 0;
     projector = 0;
     logto = 0;
     systemMemory = 0;
     memoryBlock = 0;
     projQuantity = 0;
     hdQuantity = 0;
     QuantumNum = 0;
}

/**
 *@brief gets monitor variable
 *@return monitor
 */
int config::getMonitor( ) {
    return monitor;
}

/**
 *@brief gets processor variable
 *@return processor
 */
int config::getProcessor( ) {
    return processor;
}

/**
 *@brief gets monitor variable
 *@return scanner
 */
int config::getScanner( ) {
    return scanner;
}

/**
 * @brief gets hard_drive variable
 *@return hard_drive
 */
int config::getHD( ) {
    return hard_drive;
}

/**
 *@brief gets keyboard variable
 *@return keyboard
 */
int config::getKeyboard( ) {
    return keyboard;
}

/**
 *@brief gets memory variable
 *@return memory
 */
int config::getMemory( ) {
    return memory;
}

/**
 *@brief gets projector variable
 *@return projector
 */
int config::getProjector() {
    return projector;
}

/**
 *@brief gets file_path variable
 *@return file_path
 */
string config::getPath( ) {
    return file_path;
}

/**
 *@brief Going to open up the config file and start reading in the values
 * It tests the values read in to test if they are valid. To test if they are
 * valid, I declare the correct string without spaces so I don't have to worry
 * about extra white space. Then I use getline() to get the whole line as a string
 * and remove the white space from it. That way I don't have to worry about extra
 * white space. If it is false I set a string with the error and flag it as an error.
 * @return bool, if there is an error. It returns false if there is an error
 * so the rest of the program doesn't run.
 */
bool config::setData(string conf) {

    ///All the strings I test the inputs too.
    string start ("StartSimulatorConfigurationFile");
    string filePath ("FilePath");
    string testMonitor("Monitordisplaytime{msec}");
    string testProcessor("Processorcycletime{msec}");
    string testScanner("Scannercycletime{msec}");
    string testHD("Harddrivecycletime{msec}");
    string testKeyboard("Keyboardcycletime{msec}");
    string testMemory("Memorycycletime{msec}");
    string testProjector("Projectorcycletime{msec}");
    string testMemoryKbytes ("Systemmemory{kbytes}");
    string testMemoryGbytes ("Systemmemory{Gbytes}");
    string testMemoryMbytes ("Systemmemory{Mbytes}");
    string testBlockKbytes("Memoryblocksize{kbytes}");
    string testBlockGbytes("Memoryblocksize{Gbytes}");
    string testBlockMbytes("Memoryblocksize{Mbytes}");
    string testLog("Log");
    string testPath("LogFilePath");
    string testProjQuant("Projectorquantity");
    string testHDQuant("Harddrivequantity");
    string testQuantNum("ProcessorQuantumNumber{msec}");
    string testSched("CPUSchedulingCode");
    //string testEnd("End Simulator Configuration File");
    string dummyString;
    string content;
    string tempString;
    int tempInt = 0;

    ///check mdf extension
    string conf_ext = conf;
    size_t position = conf_ext.find('.') + 1; /// finds the . which is the start of the extension

    if(position == 0){
        error = "Could not find extension\n"; ///if extension incorrect sets the string to error message
        conf_path = "error"; ///flags the variable as error
        return false;
    }
    conf_ext.erase(0,position);

    if(conf_ext != "conf"){ ///checks the extension with the correct value
        error = "Error in  configuration file extension\n"; ///if extension incorrect sets the string to error message
        conf_path = "error"; ///flags the variable as error
        return false;
    }

    ifstream configFile;
    configFile.open(conf);

    ///Checks if Configuration File is Empty
    if(configFile.peek() == std::ifstream::traits_type::eof()){
        error = "Configuration File is Empty\n";
        file = -1;
        exit(0);
    }

    /// Start Simulator Configuration File Line
    getline(configFile, content); ///gets the whole line
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    if(content != start){ ///checks if the read line matches the correct format and spelling
        error = "Typo in \"Start Simulator Configuration File\" Line\n"; ///when wrong it sets a strong to the error message
        file = -1;/// then it flags the variable that is incorrect
        return false;
    }

    /// Version/Phase: /// Ignore because irrelevant
    getline(configFile, dummyString);

    ///File Path:
    getline(configFile, content, ':'); ///gets the whole line until the colon
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    if(content != filePath){///checks if the read line matches the correct format and spelling
        error = "Typo in \"File Path\" Line\n";///when wrong it sets a strong to the error message
        file_path = "error";/// then it flags the variable that is incorrect
        return false;
    }

    configFile >> content; ///reads in the file name

    if(content.empty()){
        error = "Missing File Path";
        file_path = "error";
        return false;
    }



    ///check mdf extension
    string extension = content;
    size_t pos = extension.find('.') + 1; /// finds the . which is the start of the extension
    extension.erase(0,pos);

    if(extension == "mdf"){ ///checks the extension with the correct value
        meta_path = content; ///sets the meta data path if valid
    }
    else{
        error = "Error in file extension"; ///if extension incorrect sets the string to error message
        meta_path = "error"; ///flags the variable as error
        return false;
    }

    ///Processor Quantum Number:
    getline(configFile, content, ':');
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    if( testQuantNum != content){
        error = "Typo in \"Processor Quantum Number:\" Line. Check Spelling and Colon\n";
        QuantumNum = -1;
        return false;
    }

    configFile >> tempInt;
    if(tempInt == 0){
        error = "Missing Processor Quantum Number or a Number of 0\n";
        QuantumNum = -1;
        return false;
    }
    QuantumNum = tempInt;

    ///CPU Scheduling Code:
    getline(configFile, content, ':');
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    if( testSched != content){
        error = "Typo in \"CPU Scheduling Code:\" Line. Check Spelling and Colon\n";
        scheduling = "error";
        return false;
    }

    configFile >> content;
    if(content.empty()){
        error = "Missing CPU Scheduling Code\n";
        scheduling = "error";
        return false;
    }
    if(!(content == "FIFO" || content == "PS" ||content == "SJF" ||content == "RR"||content == "STR" )){
        error = "Incorrect CPU Scheduling Code\n";
        scheduling = "error";
        cout << "Incorrect CPU Scheduling Code\n" << endl;
        cout << content << endl;
        return false;
    }
    scheduling = content;

    ///Processor cycle time {msec}:
    getline(configFile, content, ':');
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    if( testProcessor != content){
        error = "Typo in \"Processor cycle time {msec}:\" Line. Check Spelling and Colon\n";
        processor = -1;
        return false;
    }

    configFile >> tempInt;
    if(tempInt == 0){
        error = "Missing Processor Cycle Time or a Time of 0\n";
        processor = -1;
        return false;
    }
    processor = tempInt;

    ///Monitor display time {msec} Line
    getline(configFile, content, ':'); ///gets the new line
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    /// tests if the spelling and format of the line is incorrect, if incorrect it will error out
    if( testMonitor != content){
        error = "Typo in \"Monitor display time {msec}:\" Line. Check Spelling and Colon\n"; ///sets the error
        monitor = -1; ///flags where it errors
        return false;
    }

    configFile >> tempInt; ///if correct it will read in the cycles

    if(tempInt == 0){ /// checks if it is a 0 or missing a cycle time. Does not check if negative.
        error = "Missing Processor Cycle Time or a Time of 0\n";
        monitor = -1;
        return false;
    }

    monitor = tempInt; /// if all is correct, it will store the save the cycle time

    ///Hard drive cycle time {msec}:
    getline(configFile, content, ':');
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    if(testHD != content){
        error = "Typo in \"Hard drive cycle time {msec}:\" Line. Check Spelling and Colon\n";
        hard_drive = -1;
        return false;
    }

    configFile >> tempInt;
    if(tempInt == 0){
        error = "Missing Processor Cycle Time or a Time of 0\n";
        hard_drive = -1;
        return false;
    }
    hard_drive = tempInt;

    ///Projector cycle time {msec}:
    getline(configFile, content, ':');
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    if(testProjector != content){
        error = "Typo in \"Projector cycle time {msec}:\" Line. Check Spelling and Colon\n";
        projector = -1;
        return false;
    }
    configFile >> tempInt;
    if(tempInt == 0){
        error = "Missing Processor Cycle Time or a Time of 0\n";
        projector = -1;
        return false;
    }
    projector = tempInt;

    ///Keyboard cycle time {msec}:
    getline(configFile, content, ':');
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    if(testKeyboard != content){
        error = "Typo in \"Keyboard cycle time {msec}:\" Line. Check Spelling and Colon\n";
        keyboard = -1;
        return false;
    }

    configFile >> tempInt;
    if(tempInt == 0){
        error = "Missing Processor Cycle Time or a Time of 0\n";
        keyboard = -1;
        return false;
    }
    keyboard = tempInt;

    ///Memory cycle time {msec}:
    getline(configFile, content, ':');
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    if( testMemory != content){
        error = "Typo in \"Memory cycle time {msec}:\" Line. Check Spelling and Colon\n";
        memory = -1;
        return false;
    }

    configFile >> tempInt;
    if(tempInt == 0){
        error = "Missing Processor Cycle Time or a Time of 0\n";
        memory = -1;
        return false;
    }
    memory = tempInt;
/*
    ///Scanner cycle time {msec}:
    getline(configFile, content, ':');
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    if(testScanner != content){
        error = "Typo in \"Scanner cycle time {msec}:\" Line. Check Spelling and Colon\n";
        scanner = -1;
        return false;
    }

    configFile >> tempInt;
    if(tempInt == 0){
        error = "Missing Processor Cycle Time or a Time of 0\n";
        scanner = -1;
        return false;
    }
    scanner = tempInt;
*/
    ///System memory {_bytes}:
    getline(configFile, content, ':');
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    configFile >> tempInt;
    //cout << content << endl;
    if(tempInt == 0){ ///checks if there is a integer value or 0 value
        error = "Missing System Memory or value of 0\n";
        systemMemory = -1;
        return false;
    }

    if(content == testMemoryKbytes){ /// checks if its kbytes
        systemMemory = tempInt;
    }
    else if(content == testMemoryMbytes){ /// checks if its Mbytes and converst to kbytes
        systemMemory = tempInt * 1000;
    }
    else if(content == testMemoryGbytes){ /// checks if its Gbytes and converts to kybytes
        systemMemory = tempInt * 1000000;
    }
    else{ ///if not there is an error in spelling or syntax
        error = "Error in system memory. Check Spelling and Syntax\n";
        systemMemory = -1;
        return false;
    }
    /////////////////////////////////////////////////////// end of same

    ///Memory block size {kbytes}:
    getline(configFile, content, ':');
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    configFile >> tempInt;
    //cout << content << endl;
    if(tempInt == 0){ ///checks if there is a integer value or 0 value
        error = "Missing Memory block size or value of 0\n";
        memoryBlock = -1;
        return false;
    }

    if(content == testBlockKbytes){ /// checks if its kbytes
        memoryBlock = tempInt;
    }
    else if(content == testBlockMbytes){ /// checks if its Mbytes and converst to kbytes
        memoryBlock = tempInt * 1000;
    }
    else if(content == testBlockGbytes){ /// checks if its Gbytes and converts to kybytes
        memoryBlock = tempInt * 1000000;
    }
    else{ ///if not there is an error in spelling or syntax
        error = "Error in memory block size . Check Spelling and Syntax\n";
        memoryBlock = -1;
        return false;
    }

    ///Projector quantity:
    getline(configFile, content, ':');
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    if(testProjQuant != content){
        error = "Typo in \"Projector quantity:\" Line. Check Spelling and Colon\n";
        projQuantity = -1;
        return false;
    }
    configFile >> tempInt;
    if(tempInt == 0){
        error = "Missing Projector quantity or a value of 0\n";
        projQuantity = -1;
        return false;
    }
    projQuantity = tempInt;

    ///Hard drive quantity:
    getline(configFile, content, ':');
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    if(testHDQuant != content){
        error = "Typo in \"Projector quantity:\" Line. Check Spelling and Colon\n";
        hdQuantity = -1;
        return false;
    }
    configFile >> tempInt;
    if(tempInt == 0){
        error = "Missing Projector quantity or a value of 0\n";
        hdQuantity = -1;
        return false;
    }
    hdQuantity = tempInt;

    ///Log:
    getline(configFile, content, ':'); ///gets the log path
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace

    if( testLog != content){
        error = "Typo in \"Log:\" Line. Check Spelling and Colon\n";
        logto = -1;
        return false;
    }

    configFile.ignore(256, ' '); /// ignores the words until the wanted string
    configFile.ignore(256, ' ');
    configFile.ignore(256, ' ');
    configFile >> content; ///reads in the wanted string

    ///checks if the string is valid, and if valid, sets a flag for the main to print.
    if(content == "File"){
        logto = 1;
    }
    else if(content == "Both"){
        logto = 2;
    }
    else if(content == "Monitor"){
        logto = 3;
    }
    else{
        error = "Typo in \"Log:\" Line. Check Spelling and Colon.\n";
        logto = -1;
    }

    ///Log File Path: /// same as others
    getline(configFile, content, ':');
    content = junk(content); /// removes excess characters I had problems with like \n, \r, and whitespace
    if( testPath != content){
        error = "Typo in \"Log File Path:\" Line. Check Spelling and Colon\n";
        file_path = "error";
        return false;
    }

    configFile >> tempString;
    if(tempString.empty()){
        error = "Missing Log File Path\n";
        file_path = "error";
        return false;
    }



    string ext = tempString;/// Here it takes the string read in, and sets it to a temp string
    size_t index = ext.find('.') + 1; ///It finds the the '.' that's the start of the extension
    ext.erase(0,index); /// it erases the string and leaves only the extension

    if(ext != "lgf"){ ///checks it its the wring extension and errors out
        error = "Error in file extension\n";
        file_path = "error";
        exit(0);
    }
    file_path = tempString; /// if not, saves the while file name plus extension

    getline(configFile, dummyString); /// gets the last line, but doesn't matter

    configFile.close();
    return true;
}

/**
 * @brief Logs to Data to the terminal
 * Checks it if was flagged as an error
 * Runs until the end or finds and error flag
 * If no errors, it prints the data saved
 * If error it prints out the error and ends the function
 *
 */
void config::log_config_monitor() {

    if(conf_path == "error"){
        cout << error << endl;
        return;
    }
    if (file != 1) {
        cout << "Configuration File Data" << endl;
    } else {
        cout << error << endl;
        return;
    }
    
    if (file_path == "error") {
        cout << error << endl;
    }
    
    if(QuantumNum == -1){
        cout << error << endl;
        return;
    }
    if(scheduling == "error"){
        cout << error << endl;
        return;
    }
    
    if (monitor != -1) {
        cout << "Monitor: " << monitor << " m/s cycle" << endl;
    }else{
        cout << error << endl;
        return;
    }
    if(processor != -1){
    cout << "Processor: " << processor << " m/s cycle" << endl;
    }else{
        cout << error << endl;
        return;
    }
    if(scanner != -1){
        cout << "Scanner: " << scanner << " m/s cycle" << endl;
    }else{
        cout << error << endl;
        return;
    }
    if(hard_drive != -1){
        cout << "Hard Drive: " << hard_drive << " m/s cycle" << endl;
    }else{
        cout << error  << endl;
        return;
    }
    if(keyboard != -1){
        cout << "Keyboard: " << keyboard << " m/s cycle" << endl;
    }else{
        cout << error  << endl;
        return;
    }
    if(memory != -1){
        cout << "Memory: " << memory << " m/s cycle" << endl;
    }else{
        cout << error  << endl;
        return;
    }
    if(projector != -1){
        cout << "Projector: " << projector << " m/s cycle" << endl;
    }else{
        cout << error  << endl;
        return;
    }

    if(systemMemory != -1){
        cout << "System Memory: " << systemMemory << endl;
    }else{
        cout << error << endl;
        return;
    }

    if(memoryBlock != -1){
        cout << "Memory block size: " << memoryBlock << endl;
    }else{
        cout << error << endl;
        return;
    }

    if(projQuantity != -1){
        cout << "Projector quantity: " << projQuantity << endl;
    }else{
        cout << error << endl;
        return;
    }

    if(hdQuantity != -1){
        cout << "Hard drive quantity: " << hdQuantity << endl;
    }else{
        cout << error  << endl;
        return;
    }

    if(logto == 1){
        cout << "Logged to: " << file_path << endl;
    }
    else if(logto == 2){
        cout << "Logged to monitor and " << file_path << endl;
    }
    else if (logto == 3){
        cout << "Logged to monitor";
    }
    else{
        cout << error << endl;
    }

}

/**
 * @brief Logs to Data to the File
 * Same as the log to monitor only it cout to the file
 * Opens file
 * Checks it if was flagged as an error
 * Runs until the end or finds and error flag
 * If no errors, it prints the data saved
 * If error it prints out the error and ends the function and closes file
 *
 */
void config::log_config_file() {

    ofstream log;
    /// gets the file path that was stored earlier and clears file.
    log.open(file_path, std::ofstream::out | std::ofstream::trunc);

    if(conf_path == "error"){
        log << error << endl;
        return;
    }
    if (file != 1) {
        log << "Configuration File Data" << endl;
    } else {
        log << error << endl;
        return;
    }
    if (file_path == "error") {
        log << error << endl;
    }

    if(QuantumNum == -1){
        log << error << endl;
        return;
    }
    if(scheduling == "error"){
        log << error << endl;
        return;
    }
    
    if (monitor != -1) {
        log << "Monitor: " << monitor << " m/s cycle" << endl;
    }else{
        log << error << endl;
        return;
    }
    if(processor != -1){
        log << "Processor: " << processor << " m/s cycle" << endl;
    }else{
        log << error << endl;
        return;
    }
    if(scanner != -1){
        log << "Scanner: " << scanner << " m/s cycle" << endl;
    }else{
        log << error << endl;
        return;
    }
    if(hard_drive != -1){
        log << "Hard Drive: " << hard_drive << " m/s cycle" << endl;
    }else{
        log << error << endl;
        return;
    }
    if(keyboard != -1){
        log << "Keyboard: " << keyboard << " m/s cycle" << endl;
    }else{
        log << error << endl;
        return;
    }
    if(memory != -1){
        log << "Memory: " << memory << " m/s cycle" << endl;
    }else{
        log << error << endl;
        return;
    }
    if(projector != -1){
        log << "Projector: " << projector << " m/s cycle" << endl;
    }else{
        log << error << endl;
        return;
    }

    if(systemMemory == -1) {
        log << error << endl;
        return;
    }

    if(logto == 1){
        log << "Logged to: " << file_path << endl;
    }
    else if(logto == 2){
        log << "Logged to monitor and " << file_path << endl;
    }
    else if (logto == 3){
        log << "Logged to monitor" << endl;
    }
    else{
        log << error << endl;
    }

    log.close();
}

/**
 * returns logto
 */
int config::getlogto() {
    return logto;
}

/**
 * @brief returns met_path variable
 * @return meta_path
 */
string config::getMetaPath() {
    return meta_path;
}

/**
 * @brief Takes in a string and gets rid of unwanted invisible characters or white space
 * @param content
 * @return string, without the characters
 */
string config::junk(string content) {

    content.erase(remove(content.begin(),content.end(),'\r'),content.end()); /// had a problem with '/r' being in my lines so I decided to remove any
    content.erase(remove(content.begin(),content.end(),'\n'),content.end()); /// erases the beginning because in my config file, all lines had character in front
    content.erase(remove(content.begin(),content.end(),' '),content.end()); ///removes all the white space, so white space spacing doesn't matter
    return content;
}

int config::getBlock() {
    return memoryBlock;
}

int config::getNumeProj() {
    return projQuantity;
}

int config::getNumHD() {
    return hdQuantity;
}

int config::getSysMem() {
    return systemMemory;
}

int config::getQuantNum() {
    return QuantumNum;
}

string config::getScheduling() {
    return scheduling;
}
