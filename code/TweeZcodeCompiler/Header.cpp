//
// Created by tobias on 5/2/15.
//
// See: http://inform-fiction.org/zmachine/standards/z1point1/sect11.html
//

#include <iostream>
#include <limits>
#include "Header.h"

using namespace std;

#define VERSION 8               // Z-code version
#define INTERPRETER_NUMBER 0    // e.g. 4 for Amiga
#define INTERPRETER_VERSION 0
#define STANDARD_REVISION_MAIN 1 // revision number of supperted document, here
#define STANDARD_REVISION_SUB 1  // 1.1 (STANDARD_REVISION_MAIN.STANDARD_REVISION_SUB)

vector<bitset<16>>* Header::getHeader() {
    if (!fileLengthSet) {
        cout << "No file size specified!";
        throw;
    } else if (!routinesOffsetSet) {
        cout << "No routine offset specified!";
        throw;
    } else if (!staticStringsOffsetSet) {
        cout << "No static string offset specified!";
        throw;
    }

    vector<bitset<16>> *header = new vector<bitset<16>>();

    header->push_back(VERSION);                  // Hex 0
    setFlags1(header);                           // Hex 1 - 3
    setAddresses(header);                        // Hex 4 - F
    setFlags2(header);                           // Hex 10 - 17
    setShortVal(locOfAbbrTable, header);         // Hex 18 - 19
    setShortVal(fileLength, header);             // Hex 1A - 1B
    setShortVal(fileChecksum, header);           // Hex 1C - 1D
    header->push_back(INTERPRETER_NUMBER);       // Hex 1E
    header->push_back(INTERPRETER_VERSION);      // Hex 1F
    header->push_back(screenHeight);             // Hex 20
    header->push_back(screenWidthCharacters);    // Hex 21
    setShortVal(screenWidthUnits, header);       // Hex 22 - 23
    setShortVal(screenHeightUnits, header);      // Hex 24 - 25
    header->push_back(fontWidthUnits);           // Hex 26
    header-> push_back(fontHeightUnits);         // Hex 27
    setShortVal(routinesOffset, header);         // Hex 28 - 29
    setShortVal(staticStringsOffset, header);    // Hex 2A - 2B
    header->push_back(defBackgroundColor);       // Hex 2C
    header->push_back(defForegroundColor);       // Hex 2D
    setShortVal(addressOfCharTable, header);     // Hex 2E - 2F
    setShortVal(totalWidthInPixels, header);     // Hex 30 - 31
    header->push_back(STANDARD_REVISION_MAIN);   // Hex 32
    header->push_back(STANDARD_REVISION_SUB);    // Hex 33
    setShortVal(alphabetTableAddress, header);   // Hex 34 - 35
    setShortVal(headerExtensionTableAddress, header);// Hex 36 - 38

    return header;
}

/* helper method to calculate length and to
 * avoid not setting these values before calling getHeader()
 * */
void Header::setFileLength(unsigned int length, unsigned short checksum) {
    int len = length / 8;

    if (len > numeric_limits<unsigned short>::max()) {
        cout << "File too large";
        throw;
    } else {
        fileLength = len;
        fileChecksum = checksum;
        fileLengthSet = true;
    }
}

/* helper method to calculate routines offset and to
 * avoid not setting this value before calling getHeader()
 * */
void Header::setRoutinesOffset(unsigned int offset) {
    int off = offset / 8;

    if (off > numeric_limits<unsigned short>::max()) {
        cout << "Routine offset too large";
        throw;
    } else {
        routinesOffset = off;
        routinesOffsetSet = true;
    }
}

/* helper method to calculate static string offset and to
 * avoid not setting this value before calling getHeader()
 * */
void Header::setStaticStringsOffset(unsigned int offset) {
    int off = offset / 8;

    if (off > numeric_limits<unsigned short>::max()) {
        cout << "Routine offset too large";
        throw;
    } else {
        staticStringsOffset = off;
        staticStringsOffsetSet = true;
    }
}

// sets bytes 1 - 3
void Header::setFlags1(vector<bitset<16>> *header) {
    header->push_back(0);                        //Hex 1
    header->push_back(0);                        //Hex 2

    bitset<16> flags1;
    flags1.set(0, colAvail);
    flags1.set(1, picDisplayAvail);
    flags1.set(2, boldfaceAvail);
    flags1.set(3, italicAvail);
    flags1.set(4, fixedSpaceStyleAvail);
    flags1.set(5, soundEffectsAvail);
    flags1.set(7, timedKeyboardAvail);
    header->push_back(flags1);                   // Hex 3
}

// sets bytes 4 - F
void Header::setAddresses(vector<bitset<16>> *header) {
    setShortVal(baseOfHighMem, header);         // Hex 4 - 5
    header->push_back(initValOfPC);             // Hex 6
    header->push_back(packedAddressOfMain);     // Hex 7
    setShortVal(locOfDict, header);             // Hex 8 - 9
    setShortVal(locOfObjTable, header);         // Hex A - B
    setShortVal(locOfGlobVarTable, header);     // Hex C - D
    setShortVal(baseOfStatMem, header);         // Hex E - F
}

// sets bytes 10 - 17
void Header::setFlags2(std::vector<std::bitset<16>> *header) {
    // Hex 10 - 16
    for (int i = 0; i < 7; i++)
        header->push_back(0);

    bitset<16> flags2;                           // Hex 17
    flags2.set(0, transcripting);
    flags2.set(1, forcePrintingInFixedPitchFont);
    flags2.set(2, requestScreenRedraw);
    flags2.set(3, usePictures);
    flags2.set(4, useUndoOpcodes);
    flags2.set(5, useMouse);
    flags2.set(6, useColors);
    flags2.set(7, useSoundEffects);
    flags2.set(8, useMenus);
    header->push_back(flags2);
}

// splits short value up to 2 bytes and pushes them into the header
void Header::setShortVal(unsigned short val, vector<bitset<16>> *header) {
    bitset<32> shortVal (val);
    bitset<16> firstHalf, secondHalf;

    for (size_t i = 0; i < 16; i++) {
        firstHalf.set(i, shortVal[i]);
    }

    for (size_t i = 16; i < 32; i++) {
        secondHalf.set(i-16, shortVal[i]);
    }

    header->push_back(firstHalf);
    header->push_back(secondHalf);
}
