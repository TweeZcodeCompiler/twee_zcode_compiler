#include <iostream>
#include <bitset>
#include "FileReader.h"
#include "Header.h"

using namespace std;

int main(int argc, char *argv[]) {

    Header *header = new Header();

    for (bitset<16> bs : header->getHeader()) {
        cout << bs << endl;
    }

    /*FileReader *fileReader = new FileReader();

    string filePath = fileReader->getFilePathFromArgs(argc,argv);

    string fileContent = fileReader->readFile(filePath);
    cout << fileContent;*/
    //TODO: next step call Lexer with FileContent
    return 0;
}