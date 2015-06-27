//
// Created by tobias on 5/2/15.
//

#ifndef TWEEZCODECOMPILER_HEADER_H
#define TWEEZCODECOMPILER_HEADER_H

#include <bitset>
#include <vector>
#include <cstdint>
#include "ZCodeObjects/ZCodeObject.h"

class ZCodeHeader : public ZCodeObject{
private:
    //std::vector<std::bitset<8>> headerBits;

    uint16_t fileLength = 0;
    uint16_t fileChecksum = 0;
    uint16_t routinesOffset = 0;
    uint16_t staticStringsOffset = 0;

    // flags show if headerBits is initialized
    bool fileLengthSet = false;
    bool routinesOffsetSet = false;
    bool staticStringsOffsetSet = false;

    // set special parts of headerBits
    void setFlags1(std::vector<std::bitset<8>> &header);

    void setAddresses(std::vector<std::bitset<8>> &header);

    void setFlags2(std::vector<std::bitset<8>> &header);


public:
    void print(std::vector<std::bitset<8>> &code);
    bool revalidate();
    //HEADER POSITIONS
    static const int HEADER_FILE_SIZE_POSITION = 26;
    // Flags 1 in Hex position 1 to 3
    bool colAvail = false;
    bool picDisplayAvail = false;
    bool boldfaceAvail = false;
    bool italicAvail = false;
    bool fixedSpaceStyleAvail = false;
    bool soundEffectsAvail = false;
    bool timedKeyboardAvail = false;

    // byte addresses
    uint16_t baseOfHighMem = 0;        // 2 bytes
    uint16_t initValOfPC = 0;           // initial value of program counter
    uint16_t locOfDict = 0;            // location of dictionary
    uint16_t locOfObjTable = 0;        // location of object table
    uint16_t locOfGlobVarTable = 0;    // location of global variable table
    uint16_t baseOfStatMem = 0;        // base of static memory

    // Flags 2 in Hex position 10 to 17
    bool transcripting = false;
    bool forcePrintingInFixedPitchFont = false;
    bool requestScreenRedraw = false;
    bool usePictures = false;
    bool useUndoOpcodes = false;
    bool useMouse = false;
    bool useColors = false;
    bool useSoundEffects = false;
    bool useMenus = false;

    uint16_t locOfAbbrTable = 0;              // location of abbreviation Table (byte address)

    uint8_t screenHeight = 255;           // 255 = infinite
    uint8_t screenWidthCharacters = 0;
    uint16_t screenWidthUnits = 0;
    uint16_t screenHeightUnits = 0;
    uint8_t fontHeightUnits = 0;
    uint8_t fontWidthUnits = 0;

    uint8_t defBackgroundColor = 0;
    uint8_t defForegroundColor = 0;
    uint8_t addressOfCharTable = 0;               // address of terminating characters table (bytes)
    uint16_t totalWidthInPixels = 0;              // total width in pixels of text sent to output stream 3
    uint16_t alphabetTableAddress = 0;        // Alphabet table address (bytes), 0 for default
    uint16_t headerExtensionTableAddress = 0;

    // helper methods - needed to be invoked before getHeaderBits()
    void setFileLength(uint64_t length, uint16_t checksum);

    void setRoutinesOffset(uint64_t offset);

    void setStaticStringsOffset(uint64_t offset);

    // get complete headerBits as vector<bitset>
    std::vector<std::bitset<8>> &getHeaderBits();

};

#endif //TWEEZCODECOMPILER_HEADER_H
