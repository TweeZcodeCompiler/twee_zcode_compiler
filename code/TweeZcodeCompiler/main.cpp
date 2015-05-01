#include <iostream>
#include "FileReader.h"

using namespace std;

int main(int argc, char *argv[]) {

    FileReader *fileReader = new FileReader();

    string filePath = fileReader->getFilePath(argc,argv);

    string fileContent = fileReader->readFile(filePath);
    cout << fileContent;
    //TODO: next step call Lexer with FileContent
    return 0;
}