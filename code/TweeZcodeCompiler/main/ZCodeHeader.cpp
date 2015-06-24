//
// Created by tobias on 5/2/15.
//
// See: http://inform-fiction.org/zmachine/standards/z1point1/sect11.html
//

#include <iostream>
#include <limits>
#include "ZCodeHeader.h"
#include "Utils.h"
#include <plog/Log.h>

using namespace std;

#define VERSION 8               // Z-code version
#define INTERPRETER_NUMBER 0    // e.g. 4 for Amiga
#define INTERPRETER_VERSION 0
#define STANDARD_REVISION_MAIN 1 // revision number of supperted document, here
#define STANDARD_REVISION_SUB 1  // 1.1 (STANDARD_REVISION_MAIN.STANDARD_REVISION_SUB)

vector<bitset<8>> ZCodeHeader::getHeaderBits() {
    if (!fileLengthSet) {
        LOG_DEBUG << "No file size specified!";
        throw;
    } else if (!routinesOffsetSet) {
        LOG_DEBUG << "No routine offset specified!";
        throw;
    } else if (!staticStringsOffsetSet) {
        LOG_DEBUG << "No static string offset specified!";
        throw;
    }

    vector<std::bitset<8>> headerBits;

    headerBits.push_back(VERSION);                  // Hex 0
    setFlags1(headerBits);                           // Hex 1 - 3
    setAddresses(headerBits);                        // Hex 4 - F
    setFlags2(headerBits);                           // Hex 10 - 17
    Utils::addTwoBytes(locOfAbbrTable, headerBits);         // Hex 18 - 19
    Utils::addTwoBytes(fileLength, headerBits);             // Hex 1A - 1B
    Utils::addTwoBytes(fileChecksum, headerBits);           // Hex 1C - 1D
    headerBits.push_back(INTERPRETER_NUMBER);       // Hex 1E
    headerBits.push_back(INTERPRETER_VERSION);      // Hex 1F
    headerBits.push_back(screenHeight);             // Hex 20
    headerBits.push_back(screenWidthCharacters);    // Hex 21
    Utils::addTwoBytes(screenWidthUnits, headerBits);       // Hex 22 - 23
    Utils::addTwoBytes(screenHeightUnits, headerBits);      // Hex 24 - 25
    headerBits.push_back(fontWidthUnits);           // Hex 26
    headerBits.push_back(fontHeightUnits);         // Hex 27
    Utils::addTwoBytes(routinesOffset, headerBits);         // Hex 28 - 29
    Utils::addTwoBytes(staticStringsOffset, headerBits);    // Hex 2A - 2B
    headerBits.push_back(defBackgroundColor);       // Hex 2C
    headerBits.push_back(defForegroundColor);       // Hex 2D
    Utils::addTwoBytes(addressOfCharTable, headerBits);     // Hex 2E - 2F
    Utils::addTwoBytes(totalWidthInPixels, headerBits);     // Hex 30 - 31
    headerBits.push_back(STANDARD_REVISION_MAIN);   // Hex 32
    headerBits.push_back(STANDARD_REVISION_SUB);    // Hex 33
    Utils::addTwoBytes(alphabetTableAddress, headerBits);   // Hex 34 - 35
    Utils::addTwoBytes(headerExtensionTableAddress, headerBits);// Hex 36 - 38

    for (size_t i = 0; i < 7; i++) {
        headerBits.push_back(0);           // Hex 39 - 3F
    }

    return headerBits;
}

/* helper method to calculate length and to
 * avoid not setting these values before calling getHeaderBits()
 * */
void ZCodeHeader::setFileLength(uint64_t length, uint16_t checksum) {
    uint64_t len = length / 8;

    if (len > numeric_limits<uint16_t>::max()) {
        LOG_DEBUG << "File too large";
        throw;
    } else {
        fileLength = (uint16_t) len;
        fileChecksum = checksum;
        fileLengthSet = true;
    }
}

/* helper method to calculate routines offset and to
 * avoid not setting this value before calling getHeaderBits()
 * */
void ZCodeHeader::setRoutinesOffset(uint64_t offset) {
    uint64_t off = offset / 8;

    if (off > numeric_limits<uint16_t>::max()) {
        LOG_DEBUG << "Routine offset too large";
        throw;
    } else {
        routinesOffset = (uint16_t) off;
        routinesOffsetSet = true;
    }
}

/* helper method to calculate static string offset and to
 * avoid not setting this value before calling getHeaderBits()
 * */
void ZCodeHeader::setStaticStringsOffset(uint64_t offset) {
    uint64_t off = offset / 8;

    if (off > numeric_limits<uint16_t>::max()) {
        LOG_DEBUG << "Routine offset too large";
        throw;
    } else {
        staticStringsOffset = (uint16_t) off;
        staticStringsOffsetSet = true;
    }
}

// sets bytes 1 - 3
void ZCodeHeader::setFlags1(vector<bitset<8>> &header) {
    header.push_back(0);                        //Hex 1
    header.push_back(0);                        //Hex 2

    bitset<8> flags1;
    flags1.set(0, colAvail);
    flags1.set(1, picDisplayAvail);
    flags1.set(2, boldfaceAvail);
    flags1.set(3, italicAvail);
    flags1.set(4, fixedSpaceStyleAvail);
    flags1.set(5, soundEffectsAvail);
    flags1.set(7, timedKeyboardAvail);
    header.push_back(flags1);                   // Hex 3
}

// sets bytes 4 - F
void ZCodeHeader::setAddresses(vector<bitset<8>> &header) {
    Utils::addTwoBytes(baseOfHighMem, header);         // Hex 4 - 5
    Utils::addTwoBytes(initValOfPC, header);           // Hex 6 - 7
    Utils::addTwoBytes(locOfDict, header);             // Hex 8 - 9
    Utils::addTwoBytes(locOfObjTable, header);         // Hex A - B
    Utils::addTwoBytes(locOfGlobVarTable, header);     // Hex C - D
    Utils::addTwoBytes(baseOfStatMem, header);         // Hex E - F
}

// sets bytes 10 - 17
void ZCodeHeader::setFlags2(std::vector<std::bitset<8>> &header) {
    // Hex 10 - 15
    for (int i = 0; i < 6; i++) {
        header.push_back(0);
    }

    bitset<8> flags2;                           // Hex 16
    flags2.set(0, transcripting);
    flags2.set(1, forcePrintingInFixedPitchFont);
    flags2.set(2, requestScreenRedraw);
    flags2.set(3, usePictures);
    flags2.set(4, useUndoOpcodes);
    flags2.set(5, useMouse);
    flags2.set(6, useColors);
    flags2.set(7, useSoundEffects);
    header.push_back(flags2);

    flags2.reset();                             // Hex 17
    flags2.set(0, useMenus);
    header.push_back(flags2);
}
