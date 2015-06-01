#include <iostream>

#include "FileReader.h"
#include "SimpleCompilerPipeline.h"


int main(int argc, char *argv[]) {

    FileReader *fileReader = new FileReader();

    string filePath = fileReader->getFilePathFromArgs(argc, argv);

    string fileContent = fileReader->readFile(filePath);

    cout << fileContent << endl;

    //SIMPLE COMPILER PIPELINE
    SimpleCompilerPipeline compiler;
    compiler.compile(filePath,"hello_world.z8");

    return 0;
}

