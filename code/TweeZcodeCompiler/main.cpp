#include <iostream>
#include <bitset>
#include <TweeParser.h>

#include "FileReader.h"
#include "test/MainTest.h"

using namespace std;

int main(int argc, char *argv[]) {
    MainTest test;
    test.runAllTest();

    FileReader *fileReader = new FileReader();

    string filePath = fileReader->getFilePathFromArgs(argc,argv);

    string fileContent = fileReader->readFile(filePath);
    cout << fileContent;

    Twee::TweeParser parser;

    parser.parse();


    //TODO: next step call Lexer with FileContent
    return 0;
}