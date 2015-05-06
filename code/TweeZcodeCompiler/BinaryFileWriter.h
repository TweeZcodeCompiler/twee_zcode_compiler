//
// Created by Manuel Polzhofer on 06.05.15.
//

#ifndef TWEEZCODECOMPILER_BINARYFILEWRITER_H
#define TWEEZCODECOMPILER_BINARYFILEWRITER_H

#include <bitset>
#include <vector>

class BinaryFileWriter {

public:

    void write(std::string fileName,std::vector<std::bitset<16>> *bitVector);

private:

};


#endif //TWEEZCODECOMPILER_BINARYFILEWRITER_H
