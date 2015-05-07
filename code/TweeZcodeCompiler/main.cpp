#include <iostream>
#include "FileReader.h"
#include "test/MainTest.h"


using namespace std;



int main(int argc, char *argv[]) {

    // RUN TESTS
    MainTest mainTest;
    mainTest.runAllTest();

    FileReader *fileReader = new FileReader();

    string filePath = fileReader->getFilePathFromArgs(argc,argv);

    string fileContent = fileReader->readFile(filePath);
    cout << fileContent;
    //TODO: next step call Lexer with FileContent
    return 0;
}
