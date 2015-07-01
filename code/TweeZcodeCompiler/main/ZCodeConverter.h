//
// Created by philip on 01.05.15.
//

#ifndef TWEEZCODECOMPILER_ZCODECONVERTER_H
#define TWEEZCODECOMPILER_ZCODECONVERTER_H


#include <bitset>
#include <vector>
#include <map>

class ZCodeConverter {
private:
    std::bitset<5> convertIntToBitset(int i);

    std::vector<std::bitset<8>> convert5BitTo8Bit(std::vector<std::bitset<5>> source);
    static int unicode_table[] ;
    int calculateSpecialCharacters(int ascii);

    int calculateUnicode(int ascii);

    int getAlphabet(int asciiValue);

public:
    std::vector<std::bitset<8>> convertStringToZSCII(std::string source);


    void appendZSCII(std::vector<std::bitset<5>> &vector, int value);
};


#endif //TWEEZCODECOMPILER_ZCODECONVERTER_H
