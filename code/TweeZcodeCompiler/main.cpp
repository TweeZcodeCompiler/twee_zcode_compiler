#include <iostream>
#include "FileReader.h"
#include "test/MainTest.h"
#include "SimpleCompilerPipeline.h"


using namespace std;




int main(int argc, char *argv[]) {
    MainTest test;
    // test.runAllTest();

    FileReader *fileReader = new FileReader();

    string filePath = fileReader->getFilePathFromArgs(argc, argv);

    string fileContent = fileReader->readFile(filePath);


    //SIMPLE COMPILER PIPELINE
    SimpleCompilerPipeline compiler;
    compiler.compile(fileContent,"hello_world.z8");


    //TODO: next step call Lexer with FileContent
    return 0;
}

