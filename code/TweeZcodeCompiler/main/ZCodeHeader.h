//
// Created by tobias on 5/2/15.
//

#ifndef TWEEZCODECOMPILER_HEADER_H
#define TWEEZCODECOMPILER_HEADER_H

#include <bitset>
#include <vector>

class ZCodeHeader {

private:
    //std::vector<std::bitset<8>> headerBits;

    unsigned short fileLength = 0;
    unsigned short fileChecksum = 0;
    unsigned short routinesOffset = 0;
    unsigned short staticStringsOffset = 0;

    // flags show if headerBits is initialized
    bool fileLengthSet = false;
    bool routinesOffsetSet = false;
    bool staticStringsOffsetSet = false;

    // set special parts of headerBits
    void setFlags1(std::vector<std::bitset<8>> &header);
    void setAddresses(std::vector<std::bitset<8>> &header);
    void setFlags2(std::vector<std::bitset<8>> &header);

    // split short values upt to 2 bytes
    void setShortVal(unsigned short val, std::vector<std::bitset<8>> &header);

public:
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
    unsigned short baseOfHighMem = 0;    // 2 bytes
    unsigned char initValOfPC = 0;       // initial value of program counter
    unsigned char packedAddressOfMain = 0;// packed address of initial "main" routine
    unsigned short locOfDict = 0;        // location of dictionary
    unsigned short locOfObjTable = 0;    // location of object table
    unsigned short locOfGlobVarTable = 0;// location of global variable table
    unsigned short baseOfStatMem = 0;    // base of static memory

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

    unsigned short locOfAbbrTable = 0;          // location of abbreviation Table (byte address)

    unsigned char screenHeight = 255;           // 255 = infinite
    unsigned char screenWidthCharacters = 0;
    unsigned short screenWidthUnits = 0;
    unsigned short screenHeightUnits = 0;
    unsigned char fontHeightUnits = 0;
    unsigned char fontWidthUnits = 0;

    unsigned char defBackgroundColor = 0;
    unsigned char defForegroundColor = 0;
    unsigned short addressOfCharTable = 0;          // address of terminating characters table (bytes)
    unsigned short totalWidthInPixels = 0;          // total width in pixels of text sent to output stream 3
    unsigned short alphabetTableAddress = 0;        // Alphabet table address (bytes), 0 for default
    unsigned short headerExtensionTableAddress = 0;

    // helper methods - needed to be invoked before getHeaderBits()
    void setFileLength(unsigned int length, unsigned short checksum);
    void setRoutinesOffset(unsigned int offset);
    void setStaticStringsOffset(unsigned int offset);

    // get complete headerBits as vector<bitset>
    std::vector<std::bitset<8>> getHeaderBits();

};

#endif //TWEEZCODECOMPILER_HEADER_H
