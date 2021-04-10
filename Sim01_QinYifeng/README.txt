Yifeng Qin
CS 446
2/19/19

How to compile:
There is a make file. To compile just type "make" into the terminal in the right directory.
It should compile, and to run it type "./sim1" along with command line arugment.

Design:
There are two classes, one for the config file and one for the meta data. In main we start with the config file. It runs a function to read the data. In that function it will also check for errors within the config file. If there is an error the function will return false. And the program will end with printing the valid data up to the point along with the error. All the data in the config file is stored in either ints or strings that specify what they represent. Then the meta class represents the meta data. If there are no errors in the config file, it will also read the data from a file, which name was given in the config file. It will also read through the data and check if there are errors. If there are, it will stop the program and print out the valid data and error.If not it will continue to print where ever it was directed too. The data is stored in two vectors. One for the string that contains the base data and one that contains an ingetger with the calculated cycles. There is an index that keeps track of how many strings there are and a index for the error to flag such error. 

Notes:
I had problems with using getline or the file itself. I had random new lines '\n' and carriage returns '\r' within the meta and config files. So I decided to delete all such characters from the strings. I included the test files I ran the program with as well. There should be no problems, but is just a side note. 

configuration.conf and Test_1a.mdf were my test files

Included are also html and latex documentation created by doxygen. I have both the files for them included. I am not really familar with documentation so I have the annotated.html as my html documentation. I don't know if that's whats needed so I also included the whole file. I also included the latex file, which I don't know how to use.


