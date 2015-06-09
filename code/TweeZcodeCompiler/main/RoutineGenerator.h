//
// Created by philip on 10.05.15.
//

#ifndef TWEEZCODECOMPILER_ROUTINEGENERATOR_H
#define TWEEZCODECOMPILER_ROUTINEGENERATOR_H


#include <vector>
#include <string>
#include <bitset>
#include <map>
#include "Jumps.h"
#include "OpcodeParameterGenerator.h"

class RoutineGenerator {

private:
    bool quitOpcodePrinted = false;                 // every routine needs to call quit opcode
    std::map<int, std::bitset<8>> routineZcode;     // keys = offset in routine, bitset = Opcodes etc
    Jumps jumps;
    OpcodeParameterGenerator opcodeGenerator;

    std::bitset<8> numberToBitset(unsigned int number);
    void addBitset(std::vector<std::bitset<8>> bitsets);
    void conditionalJump(unsigned int opcode, std::string toLabel, bool jumpIfTrue, int16_t param1, u_int16_t param2, bool param1IsVariable, bool param2IsVariable);

public:
    std::vector<std::bitset<8>> getRoutine();
    void newLine();

    void addLargeNumber(int16_t number);    // signed number over 2 bytes
    void addLargeNumber(int16_t number, int pos);    // signed number over 2 bytes

    void addBitset(std::bitset<8> byte, int pos = -1);

    void newLabel(std::string label);
    void jump(std::string toLabel);
    void jumpZero(std::string toLabel, bool jumpIfTrue, int16_t variable, bool parameterIsVariable);
    void jumpLessThan(std::string toLabel, bool jumpIfTrue, u_int16_t param1, u_int16_t param2, bool param1IsVariable, bool param2IsVariable);
    void jumpGreaterThan(std::string toLabel, bool jumpIfTrue, u_int16_t param1, u_int16_t param2, bool param1IsVariable, bool param2IsVariable);
    void jumpEquals(std::string toLabel, bool jumpIfTrue, u_int16_t param1, u_int16_t param2, bool param1IsVariable, bool param2IsVariable);
    void jumpEquals(std::string toLabel, bool jumpIfTrue, u_int16_t param, bool paramIsVariable);

    void readChar(uint8_t var);
    void printChar(uint8_t var);
    void printString(std::string stringToPrint);
    void printStringAtAddress(u_int8_t address);
    void callToMainRoutine(size_t offset, unsigned int locVar);

    void store(u_int8_t address, u_int16_t value);
    void load(u_int8_t address, u_int8_t result_address);

    void quitRoutine();

    RoutineGenerator() {
        jumps.setRoutineBitsetMap(routineZcode);
    }

    enum Opcode : unsigned int {
        //Opcode: VAR:246 16 4 read_char 1 time routine -> (result)
                READ_CHAR = 246,
        //Opcode: VAR:229 5 print_char output-character-code
                PRINT_CHAR = 229,
        //Opcode: 1OP:143 F 5 call_1n routine
                CALL_1N = 143,
        // Print new line
                NEW_LINE = 187,
        // Opcodes for jump instructions
                JE = 1,
                JL = 2,
                JG = 3,
                JZ = 128,
                JUMP = 140,
        // Opcode for print operation; following by Z-character String
                PRINT = 178,
        // Opcode: quit the main; no arguments.
                QUIT = 186,
        // Opcode: store variable
                STORE = 13,
        // Opcode: load a variable
                LOAD = 142,
        // Opcode: print zscii encoded string at address
                PRINT_ADDR = 135
    };

    enum BranchOffset{
        // Offset placeholder used for unconditional jumps
                JUMP_UNCOND_OFFSET_PLACEHOLDER = 1,
        // Offset placeholder used for conditional jumps
                JUMP_COND_OFFSET_PLACEHOLDER = 2,
        // Conditional jump: bit 7 of first byte of offset determines whether to jump if condition is true (1) or false (0).
        // If set to true, value of first byte is at least 128 (2^7) in decimal
                JUMP_COND_TRUE = 7,
        // Conditional jump: bit 6 of first byte of offset determines whether the offset is only between 0 and 63 over the
        // bit 5 to 0 or signed offset over 14 bits (set to 1 -> offset only 6 bits)
                JUMP_OFFSET_5_BIT = 6
    };
};



#endif //TWEEZCODECOMPILER_ROUTINEGENERATOR_H
