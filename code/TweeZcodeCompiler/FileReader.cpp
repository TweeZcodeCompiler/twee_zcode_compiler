//
// Created by Manuel Polzhofer on 30.04.15.
//

#include "FileReader.h"


string FileReader::getFilePathFromArgs(int argc, char *argv[]) {

    if ( argc != 2 )
        cout <<"Enter <filename> as parameter for call\n";
    else {

        ifstream the_file ( argv[1] );
        if ( !the_file.is_open() )
            cout<<"Could not open file\n";
        else
            return string(argv[1]);

    }
    return NULL;

}

string FileReader::readFile(string filePath) {

    std::ifstream t(filePath);
    std::string fileContent((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());

    return fileContent;

}