//
// Created by Manuel Polzhofer on 06.05.15.
//

#include <iostream>
#include <fstream>

using namespace std;

#include "BinaryFileWriter.h"


void BinaryFileWriter::write(std::string fileName,std::vector<std::bitset<16>> *bitVector)
{

    std::ofstream outfile;
    outfile.open(fileName, ios::binary | ios::out);
    for(int i = 0;i<bitVector->size();i++)
    {
        string bitString = bitVector->at(i).to_string();
        outfile.write(bitString.c_str(), sizeof(bitString));
    }

    outfile.close();

}

