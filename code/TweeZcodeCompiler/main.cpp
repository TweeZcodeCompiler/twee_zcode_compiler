#include <iostream>
#include <bitset>
#include "FileReader.h"
#include "Header.h"

using namespace std;

int main(int argc, char *argv[]) {
    FileReader *fileReader = new FileReader();

    string filePath = fileReader->getFilePathFromArgs(argc,argv);

    string fileContent = fileReader->readFile(filePath);
    cout << fileContent;
    //TODO: next step call Lexer with FileContent
    return 0;
}

void printHeader() {
    Header *header = new Header();
    header -> setFileLength(800, 99);

    vector<bitset<16>> *bits = header->getHeader();

    for (size_t i = 0; i < bits->size(); i++) {
        cout << bits->at(i) << endl;
    }
}