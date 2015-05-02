//
// Created by philip on 01.05.15.
//

#include "ZCodeConverter.h"
#include <iostream>

//This Method converts a String of ASCII chars to a String of Z-characters
//The base of implementation is the documentation on http://inform-fiction.org/zmachine/standards/z1point1/sect03.html
//This Method use the default alphabet Table of the z-machine (see section 3.5.3) to translate stored Z-characters to ZSCII.
vector<bitset<8>> ZCodeConverter::convertStringToZSCII(string source) {
    //Z-characters are 5 bits long
    vector<bitset<5>> zcode = vector<bitset<5>>();
    //this will contains the final 8 bit Z-character string
    vector<bitset<8>> returnVector = vector<bitset<8>>();

    //First Iterate over the characters of "source" and create a vector of 5-bit Z-Chars
    for (int i = 0; i < source.size(); i++) {

        char c = source[i];
        int asciiValue = (int) c;
        int zsciiValue = 0;

        //There are three alphabets of z-characters. To switch between the alphabets use the Z-Character 4 or 5 (on version 5 or later).
        if (c == ' ') {
            //space is 00000 in binary form (see 3.5.1)
            zsciiValue = 0;
        } else if (asciiValue > 96 && asciiValue && asciiValue < 123) {
        //lower case: this is the default alphabet; we do not have to switch it.
            zsciiValue = asciiValue - 91;
        } else if (asciiValue > 64 && asciiValue < 91) {
            //upper case
            //This is the second alphabet. We have to push '00100' to switch to capital letters.
            zcode.push_back(convertIntToBitset(4));
            zsciiValue = asciiValue - 59;
        } else {
            //punctuation and numbers
            //This is the third alphabet. We have to push '00101' to switch to punctuation letters.
            zcode.push_back(convertIntToBitset(5));
            if (asciiValue > 47 && asciiValue < 58) {
                //range of characters from '0' to '9'
                zsciiValue = asciiValue - 40;
            } else {
                //the z-characters of the following signs are not in the same order as their ascii equivalent
                //we have to check them hard coded
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
        //add the calculated Z-character to the character string
        bitset<5> bs = convertIntToBitset(zsciiValue);
        zcode.push_back(bs);
    }

    //the following code translate 5 bit Z-characters into a 8 bit string, as we store it

    //we use padding to expand the size of Z-characters to multiple of 3
    //the Z-machine ignores the zeros at the end
    while (zcode.size() % 3 != 0) {
        zcode.push_back(convertIntToBitset(0));
    }

    for (unsigned long i = 0; i < zcode.size(); i += 3) {
        // Each two 8 bit byte stores three 5 bit Z-characters
        bitset<8> firstByte = bitset<8>();
        bitset<8> secondByte = bitset<8>();
        bitset<5> firstZChar = zcode.at(i);
        bitset<5> secondZChar = zcode.at(i + 1);
        bitset<5> thirdZChar = zcode.at(i + 2);

        //the first bit of the two bytes indicates the end of string
        //if this bit is 1, the string ends after the current two bytes
        if (i == zcode.size() - 3) {
            firstByte.set(0, true);
        }
        //write first Z-char
        for (unsigned long pos = 1; pos < 6; pos++) {
            firstByte.set(pos, firstZChar[pos - 1]);
        }
        //write second Z-char
        for (unsigned long pos = 6; pos < 8; pos++) {
            firstByte.set(pos, secondZChar[pos - 6]);
        }
        returnVector.push_back(firstByte);
        for (unsigned long pos = 0; pos < 3; pos++) {
            secondByte.set(pos, secondZChar[pos + 2]);
        }
        //write third Z-char
        for (unsigned long pos = 3; pos < 8; pos++) {
            secondByte.set(pos, thirdZChar[pos - 3]);
        }
        returnVector.push_back(secondByte);
    }
    return returnVector;
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