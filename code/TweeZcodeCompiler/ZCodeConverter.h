//
// Created by philip on 01.05.15.
//

#ifndef TWEEZCODECOMPILER_ZCODECONVERTER_H
#define TWEEZCODECOMPILER_ZCODECONVERTER_H


#include <bitset>
#include <vector>

class ZCodeConverter {
private:
    std::bitset<5> convertIntToBitset(int i);
    std::vector<std::bitset<8>> convert5BitTo8Bit(std::vector<std::bitset<5>> source);
    int calculateSpecialCharacters(int ascii);
    int getAlphabet(int asciiValue);

public:
    std::vector<std::bitset<8>> convertStringToZSCII(std::string source);

};


#endif //TWEEZCODECOMPILER_ZCODECONVERTER_H
