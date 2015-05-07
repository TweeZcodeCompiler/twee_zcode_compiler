//
// Created by Manuel Polzhofer on 06.05.15.
//

#include <iostream>
#include <fstream>


#include "BinaryFileWriter.h"


void BinaryFileWriter::write(std::string fileName,std::vector<std::bitset<8>> bitVector)
{

    FILE* pFile;
    pFile = fopen(fileName.c_str(), "wb");
    for(int i = 0;i<bitVector.size();i++)
    {
        fwrite(&bitVector.at(i), 1, 1, pFile);

    }

    fclose(pFile);

}

