//
// Created by tobias on 5/2/15.
//

#ifndef TWEEZCODECOMPILER_HEADER_H
#define TWEEZCODECOMPILER_HEADER_H

#include <bitset>
#include <vector>

class Header {

private:
    unsigned short fileLength;
    unsigned short fileChecksum;
    unsigned short routinesOffset;
    unsigned short staticStringsOffset;

    // flags show if header is initialized
    bool fileLengthSet = false;
    bool routinesOffsetSet = false;
    bool staticStringsOffsetSet = false;

    // set special parts of header
    void setFlags1(std::vector<std::bitset<16>> *header);
    void setAddresses(std::vector<std::bitset<16>> *header);
    void setFlags2(std::vector<std::bitset<16>> *header);

    // split short values upt to 2 bytes
    void setShortVal(unsigned short val, std::vector<std::bitset<16>> *header);

public:
    // Flags 1 in Hex position 1 to 3
    bool colAvail = false;
    bool picDisplayAvail = false;
    bool boldfaceAvail = false;
    bool italicAvail = false;
    bool fixedSpaceStyleAvail = false;
    bool soundEffectsAvail = false;
    bool timedKeyboardAvail = false;

    // byte addresses
    unsigned short baseOfHighMem;     // 2 bytes
    unsigned char initValOfPC;           // initial value of program counter
    unsigned char packedAddressOfMain;   // packed address of initial "main" routine
    unsigned short locOfDict;            // location of dictionary
    unsigned short locOfObjTable;        // location of object table
    unsigned short locOfGlobVarTable;    // location of global variable table
    unsigned short baseOfStatMem;        // base of static memory

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

    unsigned short locOfAbbrTable;              // location of abbreviation Table (byte address)

    unsigned char screenHeight = 255;           // 255 = infinite
    unsigned char screenWidthCharacters;
    unsigned short screenWidthUnits;
    unsigned short screenHeightUnits;
    unsigned char fontHeightUnits;
    unsigned char fontWidthUnits;

    unsigned char defBackgroundColor;
    unsigned char defForegroundColor;
    unsigned char addressOfCharTable;               // address of terminating characters table (bytes)
    unsigned short totalWidthInPixels;              // total width in pixels of text sent to output stream 3
    unsigned short alphabetTableAddress = 0;        // Alphabet table address (bytes), 0 for default
    unsigned short headerExtensionTableAddress = 0;

    // helper methods - needed to be invoked before getHeader()
    void setFileLength(unsigned int length, unsigned short checksum);
    void setRoutinesOffset(unsigned int offset);
    void setStaticStringsOffset(unsigned int offset);

    // get complete header as vector<bitset>
    std::vector<std::bitset<16>>* getHeader();

};

#endif //TWEEZCODECOMPILER_HEADER_H
