//
// Created by philip on 01.05.15.
//

#include "ZCodeConverter.h"
#include <iostream>

vector<bitset<8>> ZCodeConverter::convertStringToZSCII(string source) {
    vector<bitset<5>> zcode = vector<bitset<5>>();
    for (int i = 0; i < source.size(); i++) {
        //lower case
        char c = source[i];
        int asciiValue = (int) c;
        int zsciiValue = 0;
        if (c == ' ') {
            //space
            zsciiValue = 0;
        } else if (asciiValue > 96 && asciiValue && asciiValue < 123) {
            zsciiValue = asciiValue - 91;
        } else if (asciiValue > 64 && asciiValue < 91) {
            //upper case
            zcode.push_back(convertIntToBitset(4));
            zsciiValue = asciiValue - 59;
        } else {
            //punctuation and numbers
            zcode.push_back(convertIntToBitset(5));
            if (asciiValue > 47 && asciiValue < 58) {
                zsciiValue = asciiValue - 40;
            } else {
                switch (asciiValue) {
                    case '\n':
                        zsciiValue = 7;
                        break;
                    case '.':
                        zsciiValue = 18;
                        break;
                    case ',':
                        zsciiValue = 19;
                        break;
                    case '!':
                        zsciiValue = 20;
                        break;
                    case '?':
                        zsciiValue = 21;
                        break;
                    case '_':
                        zsciiValue = 22;
                        break;
                    case  '#':
                        zsciiValue = 23;
                        break;
                    case '\'':
                        zsciiValue = 24;
                        break;
                    case '"':
                        zsciiValue = 25;
                        break;
                    case '/':
                        zsciiValue = 26;
                        break;
                    case '\\':
                        zsciiValue = 27;
                        break;
                    case '-':
                        zsciiValue = 28;
                        break;
                    case ':':
                        zsciiValue = 29;
                        break;
                    case '(':
                        zsciiValue = 30;
                        break;
                    case ')':
                        zsciiValue = 31;
                        break;
                    default:
                        //TODO Unicode if we want to support
                        //For now lets print out a question mark.
                        zsciiValue = 21;
                        break;
                }
            }
        }
        bitset<5> bs = convertIntToBitset(zsciiValue);
        zcode.push_back(bs);
    }
    while (zcode.size() % 3 != 0) {
        zcode.push_back(convertIntToBitset(0));
    }
    vector<bitset<8>> akk = vector<bitset<8>>();
    for (unsigned long i = 0; i < zcode.size(); i += 3) {
        bitset<8> first = bitset<8>();
        bitset<8> second = bitset<8>();
        bitset<5> firstBs = zcode.at(i);
        bitset<5> secondBs = zcode.at(i + 1);
        bitset<5> thirdBs = zcode.at(i + 2);
        if (i == zcode.size() - 3) {
            first.set(0, true);
        }

        for (int pos = 1; pos < 6; pos++) {
            first.set(pos, firstBs[pos - 1]);
        }
        for (int pos = 6; pos < 8; pos++) {
            first.set(pos, secondBs[pos - 6]);
        }
        akk.push_back(first);
        for (int pos = 0; pos < 3; pos++) {
            second.set(pos, secondBs[pos + 2]);
        }
        for (int pos = 3; pos < 8; pos++) {
            second.set(pos, thirdBs[pos - 3]);
        }
        akk.push_back(second);
    }
    return akk;
}

bitset<5> ZCodeConverter::convertIntToBitset(int in) {
    bitset<5> bs = bitset<5>();
    if (in >= 16) {
        in -= 16;
        bs.set(0, true);
    }
    if (in >= 8) {
        in -= 8;
        bs.set(1, true);
    }
    if (in >= 4) {
        in -= 4;
        bs.set(2, true);
    }
    if (in >= 2) {
        in -= 2;
        bs.set(3, true);
    }
    if (in >= 1) {
        bs.set(4, true);
    }
    return bs;
}


int testAsciiToZscii() {
    ZCodeConverter z = ZCodeConverter();
    vector<bitset<8>> erg = z.convertStringToZSCII("H W!");
    for (bitset<8> bs : erg) {
        for (int i = 0; i < 8; i++) {
            std::cout << bs[i];
        }
    }
    return 0;
}