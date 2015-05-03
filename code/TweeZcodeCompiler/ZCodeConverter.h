//
// Created by philip on 01.05.15.
//

#ifndef TWEEZCODECOMPILER_ZCODECONVERTER_H
#define TWEEZCODECOMPILER_ZCODECONVERTER_H


#include <bitset>
#include <vector>

using namespace std;

class ZCodeConverter {
private:
    bitset<5> convertIntToBitset(int i);
    vector<bitset<8>> convert5BitTo8Bit(vector<bitset<5>> source);
    int calculateSpecialCharacters(int ascii);
    int getAlphabet(int asciiValue);

public:
    std::vector<bitset<8>> convertStringToZSCII(string source);

};


#endif //TWEEZCODECOMPILER_ZCODECONVERTER_H
