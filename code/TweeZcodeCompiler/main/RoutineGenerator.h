//
// Created by philip on 10.05.15.
//

#ifndef TWEEZCODECOMPILER_ROUTINEGENERATOR_H
#define TWEEZCODECOMPILER_ROUTINEGENERATOR_H


#include <vector>
#include <string>
#include <bitset>
#include <map>
#include <iostream>
#include "Jumps.h"
#include "OpcodeParameterGenerator.h"
#include "Utils.h"
#include <memory>

class RoutineGenerator {

private:
    bool printLogs = true;

    std::map<int, std::bitset<8>> routineZcode;     // keys = offset in routine, bitset = Opcodes etc
    std::map<std::string, u_int8_t> locVariables;   // keys = variable name, value = number in stack
    static std::map<std::string, size_t> routines;  // keys = name of routine, value = offset.

    size_t maxLocalVariables = 0;
    size_t addedLocalVariables = 0;
    size_t offsetOfRoutine = 0;
    Jumps jumps;
    OpcodeParameterGenerator opcodeGenerator;

    std::bitset<8> numberToBitset(unsigned int number);

    void addBitset(std::vector<std::bitset<8>> bitsets);

    void addTwoBytes(int16_t number, int pos = -1);     // splits 16 bit value up to 2 bytes and adds them to routineZcode

    void addOneByte(std::bitset<8> byte, int pos = -1);  // add one byte to routineZcode

    void conditionalJump(unsigned int opcode, std::string toLabel, bool jumpIfTrue, ZParam& param1, ZParam& param2);

public:
    // constructor needed to create first jump to main call
    RoutineGenerator(size_t routineOffset) {
        jumps.setRoutineBitsetMap(routineZcode);

        size_t pkgAdrr = Utils::calculateNextPackageAddress((size_t) (routineOffset + 3));

        std::vector<std::bitset<8>> instructions = opcodeGenerator.generate1OPInstruction(CALL_1N, (u_int16_t) (pkgAdrr / 8), false);
        addBitset(instructions);

        size_t padding = Utils::paddingToNextPackageAddress(routineZcode.size(), routineOffset);

        for (size_t i = 0; i < padding; i++) {
            addOneByte(numberToBitset(0));
        }
    }

    // this constructor padding zCode to the next package adress and initialize this routine with the name 'name'
    RoutineGenerator(std::string name,unsigned int locVar, std::vector<std::bitset<8>> &zCode, size_t offsetOfZCode){
        size_t padding = Utils::paddingToNextPackageAddress(zCode.size(), offsetOfZCode);
        Utils::fillWithBytes(zCode, 0, padding);

        RoutineGenerator::routines[name] = zCode.size() + offsetOfZCode;
        this->offsetOfRoutine = zCode.size() + offsetOfZCode;
        std::cout << padding << "/" << this->offsetOfRoutine << "\n";
        jumps.setRoutineBitsetMap(routineZcode);
        jumps.routineOffset = this->offsetOfRoutine;

        addOneByte(numberToBitset(locVar));
        maxLocalVariables = locVar;

        if (locVar > 15) {
            std::cout << "Cannot add more than 15 local variables to routine " << name << "!";
            throw;
        }
    }

    // returns complete zcode of Routine as a bitset vector
    std::vector<std::bitset<8>> getRoutine();

    /*
     *      methods to handle call routine offsets:
     */

    static void resolveCallInstructions(std::vector<std::bitset<8>> &zCode);

    static std::map<size_t, std::string> callTo;    //keys = offset of call, value = name of routine

    /*
     *      methods to access/set local variables
     */

    void setLocalVariable(std::string name, int16_t value = 0);

    u_int8_t getAddressOfVariable(std::string name);

    bool containsLocalVariable(std::string name);

    /*
     *      methods to add intermediate code instructions to routine
     */

    void newLine();

    void newLabel(std::string label);

    void jump(std::vector<std::unique_ptr<ZParam>> params);

    void jumpZero(std::vector<std::unique_ptr<ZParam>> params);

    void jumpLessThan(std::vector<std::unique_ptr<ZParam>> params);

    void jumpGreaterThan(std::vector<std::unique_ptr<ZParam>> params);

    void jumpEquals(std::vector<std::unique_ptr<ZParam>> params);

    void readChar(uint8_t var);

    void printChar(uint8_t var);

    void setTextStyle(bool roman, bool reverseVideo, bool bold, bool italic, bool fixedPitch);

    void printString(std::string stringToPrint);

    void printStringAtAddress(u_int8_t address);

    void printNum(unsigned int address);

    //Call to a routine with spezific name
    void callRoutine1n(std::string routineName);

    void callRoutine(std::string routineName, const uint8_t storeTarget, const ZParam *param1,
                                       const ZParam *param2, const ZParam *param3);

    void store(u_int8_t address, u_int16_t value);

    void load(u_int8_t address, u_int8_t resultAddress);

    void quitRoutine();

    void returnValue(const ZParam &param);


    /*
     *      Enumerations
     */

    enum Opcode : unsigned int {
        //Opcode: VAR:246 16 4 read_char 1 time routine -> (result)
                READ_CHAR = 246,
        //Opcode: VAR:229 5 print_char output-character-code
                PRINT_CHAR = 229,
        //Opcode: 1OP:143 F 5 call_1n routine
                CALL_1N = 143,
                CALL = 224,
                CALL_VS = 224,
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
                PRINT_ADDR = 135,
        // Opcode: print signed num value in decimal
                PRINT_SIGNED_NUM = 230,
        // Opcode: return value
                RET_VALUE = 139,
        // Opcode: set the text style
                SET_TEXT_STYLE = 241
    };

    enum BranchOffset {
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
