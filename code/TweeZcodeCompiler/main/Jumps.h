//
// Created by tobias on 6/7/15.
//

#ifndef PROJECT_JUMPS_H
#define PROJECT_JUMPS_H


#include <map>
#include <bitset>

class Jumps {
private:
    std::map<int, std::bitset<8>> *routineOpcodes;
    std::map<std::string, int> branches;        // maps branch label to instruction address
    std::map<int, std::string> jumpToBranch;    // keys = addresses that need to be filled with branch offset, values = jump destination label


    int addCondBranchOffset(size_t position, int16_t offset, bool jumpIfCondTrue);
    int getOffset(int jumpPosition, int labelPosition);

    void addLargeNumber(int16_t number, int pos);
    void addBitset(std::bitset<8> byte, int pos = -1);
public:
    void newLabel(std::string label);
    void newJump(std::string toLabel);
    void calculateOffsets();
    void setRoutineBitsetMap(std::map<int, std::bitset<8>> &opcodes);
};


#endif //PROJECT_JUMPS_H
