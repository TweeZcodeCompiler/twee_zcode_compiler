//
// Created by Manuel Polzhofer on 06.05.15.
//

#include <iostream>
#include <fstream>


#include "BinaryFileWriter.h"

using std::ofstream;
using std::string;
using std::ios;


void BinaryFileWriter::write(std::string fileName,std::vector<std::bitset<8>> bits)
{
    ofstream file;
    file.open (fileName, ios::out | ios::binary);

    for(size_t i=0; i<bits.size(); i++) {
        unsigned long l = bits.at(i).to_ulong();
        unsigned char c = static_cast<unsigned char>( l );
        file << c;
    }

    file.close();
}

