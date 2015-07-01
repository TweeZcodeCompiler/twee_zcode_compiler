// Implementation of ZCodeConverter.h
// Created by philip on 01.05.15.
//

#include "ZCodeConverter.h"
#include <iostream>
#include <plog/Log.h>

using std::bitset;
using std::vector;

int ZCodeConverter::unicode_table[] = {
0xe4,
0xf6,// 	o-diaeresis 	ö	oe
0xfc,// 	u-diaeresis 	ü	ue
0xc4,// 	A-diaeresis 	Ä	Ae
0xd6,// 	O-diaeresis 	Ö	Oe
0xdc,// 	U-diaeresis 	Ü	Ue
0xdf ,//	sz-ligature 	ß	ss
0xbb,// 	quotation 	»	>> or "
0xab,// 	marks 	«	<< or "
0xeb,// 	e-diaeresis 	ë	e
0xef,// 	i-diaeresis 	ï	i
0xff,// 	y-diaeresis 	ÿ	y
0xcb,// 	E-diaeresis 	Ë	E
0xcf,// 	I-diaeresis 	Ï	I
0xe1,// 	a-acute 	á	a
0xe9,// 	e-acute 	é	e
0xed,// 	i-acute 	í	i
0xf3,// 	o-acute 	ó	o
0xfa,// 	u-acute 	ú	u
0xfd,// 	y-acute 	ý	y
0xc1,// 	A-acute 	Á	A
0xc9,// 	E-acute 	É	E
0xcd,// 	I-acute 	Í	I
0xd3,// 	O-acute 	Ó	O
0xda,// 	U-acute 	Ú	U
0xdd,// 	Y-acute 	Ý	Y
0xe0,// 	a-grave 	à	a
0xe8,// 	e-grave 	è	e
0xec,// 	i-grave 	ì	i
0xf2 ,//	o-grave 	ò	o
0xf9,// 	u-grave 	ù	u
0xc0,// 	A-grave 	À	A
0xc8,// 	E-grave 	È	E
0xcc ,//	I-grave 	Ì	I
0xd2,// 	O-grave 	Ò	O
0xd9,// 	U-grave 	Ù	U
0xe2,// 	a-circumflex 	â	a
0xea,// 	e-circumflex 	ê	e
0xee,// 	i-circumflex 	î	i
0xf4,// 	o-circumflex 	ô	o
0xfb,// 	u-circumflex 	û	u
0xc2,// 	A-circumflex 	Â	A
0xca,// 	E-circumflex 	Ê	E
0xce,// 	I-circumflex 	Î	I
0xd4,// 	O-circumflex 	Ô	O
0xdb,// 	U-circumflex 	Û	U
0xe5,// 	a-ring 	å	a
0xc5,// 	A-ring 	Å	A
0xf8,// 	o-slash 	ø	o
0xd8,// 	O-slash 	Ø	O
0xe3,// 	a-tilde 	ã	a
0xf1,// 	n-tilde 	ñ	n
0xf5,// 	o-tilde 	õ	o
0xc3,// 	A-tilde 	Ã	A
0xd1,// 	N-tilde 	Ñ	N
0xd5,// 	O-tilde 	Õ	O
0xe6,// 	ae-ligature 	æ	ae
0xc6,// 	AE-ligature 	Æ	AE
0xe7,// 	c-cedilla 	ç	c
0xc7,// 	C-cedilla 	Ç	C
0xfe,// 	Icelandic thorn 	þ	th
0xf0,// 	Icelandic eth 	ð	th
0xde,// 	Icelandic Thorn 	Þ	Th
0xd0,// 	Icelandic Eth 	Ð	Th
0xa3,// 	pound symbol 	£	L
153,// 	oe-ligature 	œ	oe
152,// 	OE-ligature 	Œ	OE
0xa1,// 	inverted ! 	¡	!
0xbf};

//This Method converts a String of ASCII chars to a String of Z-characters
//The base of implementation is the documentation on http://inform-fiction.org/zmachine/standards/z1point1/sect03.html
//This Method use the default alphabet Table of the z-machine (see section 3.5.3) to translate stored Z-characters to ZSCII.
vector<bitset<8>> ZCodeConverter::convertStringToZSCII(std::string source) {
    //Z-characters are 5 bits long
    vector<bitset<5>> zcode = vector<bitset<5>>();

    //First Iterate over the characters of "source" and create a vector of 5-bit Z-Chars
    for (unsigned i = 0; i < source.size(); i++) {

        char c = source[i];
        int asciiValue = (int) c;
        int zsciiValue = 0;

        //There are three alphabets of Z-characters. To switch between the alphabets use the Z-Character 4 or 5 (on version 5 or later).
        switch (getAlphabet(asciiValue)) {

            case 0:
                //lower case is the default case
                zsciiValue = asciiValue - 91;
                zcode.push_back(convertIntToBitset(zsciiValue));
                break;

            case 1:
                //upper case: switch alphabet to A1
                zcode.push_back(convertIntToBitset(4));
                zsciiValue = asciiValue - 59;
                zcode.push_back(convertIntToBitset(zsciiValue));
                break;

            case 2:
                //punctuation: switch alphabet to A2
                zcode.push_back(convertIntToBitset(5));
                int converted;
                converted = calculateSpecialCharacters(asciiValue);
                zsciiValue = (converted == -1) ? 21 : converted;
                zcode.push_back(convertIntToBitset(zsciiValue));
                break;

            case 4:
                //ascii value
                //zwitch to alphabet A2 and request ZSCII-Value
                zcode.push_back(convertIntToBitset(5));
                zcode.push_back(convertIntToBitset(6));
                appendZSCII(zcode, asciiValue);
                break;

            default:
                int unicode = calculateUnicode(asciiValue);
                if(unicode != -1){
                    zcode.push_back(convertIntToBitset(5));
                    zcode.push_back(convertIntToBitset(6));
                    appendZSCII(zcode, unicode);
                }else {
                    //ignore Character
                }
                break;
        }


    }
    return convert5BitTo8Bit(zcode);
}

//This method converts a number to a 5 bit bitset
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

//This methods calculate the Z-character value for punctuations which are not in the same order as ascii-characters
//if "asciiValue" does not match an valid value for an Z-character, this method will return -1
int ZCodeConverter::calculateSpecialCharacters(int asciiValue) {
    if (asciiValue > 47 && asciiValue < 58) {
        //range of characters from '0' to '9'
        return asciiValue - 40;
    }
    switch (asciiValue) {
        case '\n':
            return 7;
        case '.':
            return 18;
        case ',':
            return 19;
        case '!':
            return 20;
        case '?':
            return 21;
        case '_':
            return 22;
        case  '#':
            return 23;
        case '\'':
            return 24;
        case '"':
            return 25;
        case '/':
            return 26;
        case '\\':
            return 27;
        case '-':
            return 28;
        case ':':
            return 29;
        case '(':
            return 30;
        case ')':
            return 31;
        default:
            return -1;
    }
}

int ZCodeConverter::calculateUnicode(int ascii) {
    for (size_t i = 0; i < 69; i++) {
        if(ascii == ZCodeConverter::unicode_table[i]){
            return i+155;
        }
    }
    return -1;
}

//This method calculates the alphabet.
//return 0 : lower case
//return 1 : upper case
//return 2 : punctuation/ unknown
//return 3 : space
//return 4 : ascii value
int ZCodeConverter::getAlphabet(int asciiValue) {
    if (asciiValue > 96 && asciiValue && asciiValue < 123) {
        return 0;
    } else if (asciiValue > 64 && asciiValue < 91) {
        return 1;
    } else if (asciiValue == ' ') {
        return 3;
    } else if (asciiValue >= 32 && asciiValue < 127) {
        return 4;
    } else {
        return 2;
    }
}

//the following code translate 5 bit Z-characters into a 8 bit string, as we store it
vector<bitset<8>>  ZCodeConverter::convert5BitTo8Bit(vector<bitset<5>> source) {
    vector<bitset<8>> returnVector = vector<bitset<8>>();

    //use padding to expand the size of Z-characters to multiple of 3
    //the Z-machine ignores the zeros at the end
    while (source.size() % 3 != 0) {
        source.push_back(convertIntToBitset(5));
    }

    for (unsigned long i = 0; i < source.size(); i += 3) {
        // Each two 8 bit byte stores three 5 bit Z-characters
        bitset<8> firstByte = bitset<8>();
        bitset<8> secondByte = bitset<8>();
        bitset<5> firstZChar = source.at(i);
        bitset<5> secondZChar = source.at(i + 1);
        bitset<5> thirdZChar = source.at(i + 2);
        //the first bit of the two bytes indicates the end of string
        //if this bit is 1, the string ends after the current two bytes
        if (i == source.size() - 3) {
            firstByte.set(7, true);
        }
        //write first Z-char
        for (unsigned long pos = 2; pos < 7; pos++) {
            firstByte.set(pos, firstZChar[6 - pos]);
        }
        //write second Z-char
        for (unsigned long pos = 0; pos < 2; pos++) {
            firstByte.set(pos, secondZChar[1 - pos]);
        }
        returnVector.push_back(firstByte);
        for (unsigned long pos = 5; pos < 8; pos++) {
            secondByte.set(pos, secondZChar[9 - pos]);
        }
        //write third Z-char
        for (unsigned long pos = 0; pos < 5; pos++) {
            secondByte.set(pos, thirdZChar[4 - pos]);
        }
        returnVector.push_back(secondByte);
    }
    return returnVector;
};

int testStringToZCharacterString() {
    ZCodeConverter z = ZCodeConverter();
    vector<bitset<8>> erg = z.convertStringToZSCII("H W!");
    for (bitset<8> bs : erg) {
        for (int i = 0; i < 8; i++) {
            LOG_DEBUG << bs[i];
        }
    }
    return 0;
}

void ZCodeConverter::appendZSCII(std::vector<std::bitset<5>> &zcode, int asciiValue) {
    bitset<10> val = bitset<10>(asciiValue);
    bitset<5> topByte;
    bitset<5> bottomByte;
    for (size_t i = 9; i >= 5; i--) {
        topByte.set(9 - i, val[i]);
    }
    for (int i = 4; i >= 0; i--) {
        bottomByte.set(4 - i, val[i]);
    }
    zcode.push_back(topByte);
    zcode.push_back(bottomByte);
}
