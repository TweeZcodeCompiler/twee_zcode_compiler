//
// Created by Manuel Polzhofer on 30.04.15.
//

#ifndef TWEEZCODECOMPILER_FILEREADER_H
#define TWEEZCODECOMPILER_FILEREADER_H

#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>


using namespace std;

class FileReader {

private:


public:
    string readFile(string filePath);
    string getFilePathFromArgs(int argc, char *argv[]);

};


#endif //TWEEZCODECOMPILER_FILEREADER_H
