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

public:
    std::vector<bitset<8>> convertStringToZSCII(string source);

};


#endif //TWEEZCODECOMPILER_ZCODECONVERTER_H
