Yifeng Qin
CS446
Project 5
5/8/19

File Names:
main.cpp config.h config.cpp meta.h meta.cpp PCB.h PCB.cpp simulation.h simulation.cpp makefile
Also include the files I tested with, configuation.conf and Test.mdf
There is Cmake file, but ignore them. They are just for me.

Compile:
There is a makefile, the executable should be ./sim5 ____________ with the configuration file name.

Design:
This is designed from the last project. I just added the RR and STR fucntions to the simulation.cpp file. First the data from the meta data is stored into a 2d vector. So each row is a process and each column in the row is a task. If i need to perform RR i subtract the quantum time from the total time. Then if that is completed I pop that task from the vector. For STR I find the lowest time to run and pass that index to the 2d vector. Again then i subtract the quantum time form it and if is completed i pop it from the vector. Also I added an interupt function for the non I/O tasks.The project reloads the data every 100ms.


Notes:
For print, it doesn't print the last project out unless there is an error. So it doesn't print the config and metadata. Printing the simulation is seperate from the simulation itslef, so if everything is correct, it waits until the simulation is done to print. So you have to wait until the simulation finishes and it will print all at once.

Vineeth told me that there is no scanner in the config file. So if there is a scanner then there will be a problem. I included the config file that i tested it with. 
