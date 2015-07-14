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
#include "exceptions.h"
#include "ZCodeObjects/ZCodeRoutine.h"
#include "ZCodeObjects/ZCodeLabel.h"
#include <memory>
#include <plog/Log.h>

class RoutineGenerator {

private:
    std::map<int, std::bitset<8>> routineZcode;     // keys = offset in routine, bitset = Opcodes etc
    std::map<std::string, u_int8_t> locVariables;   // keys = variable name, value = number in stack
    static std::map<std::string, size_t> routines;  // keys = name of routine, value = offset.

    std::shared_ptr<ZCodeRoutine> routine;

    size_t maxLocalVariables = 0;
    size_t addedLocalVariables = 0;
    size_t offsetOfRoutine = 0;
    Jumps jumps;
    OpcodeParameterGenerator opcodeGenerator;

    size_t nextLocVarInitLabelNumber = 0;

    std::bitset<8> numberToBitset(unsigned int number);

    void addBitset(std::vector<std::bitset<8>> bitsets, std::string name);

    void conditionalJump(unsigned int opcode, std::string toLabel, bool jumpIfTrue, ZParam &param1, ZParam &param2);

    void debug(std::string);

public:

    bool debugmode = false;

    std::shared_ptr<ZCodeLabel> getOrCreateLabel(std::string name);

    // this constructor padding zCode to the next package adress and initialize this routine with the name 'name'
    RoutineGenerator(std::string name, unsigned int locVar) : routine(ZCodeRoutine::getOrCreateRoutine(name, locVar)) {

        maxLocalVariables = locVar;
        if (locVar > 15) {
            LOG_DEBUG << "Cannot add more than 15 local variables to routine " << name << "!";
            throw AssemblyException(AssemblyException::ErrorType::INVALID_DIRECTIVE);
        }
    }

    ~RoutineGenerator() {
    }

    // returns complete zcode of Routine as a bitset vector
    std::shared_ptr<ZCodeRoutine> getRoutine();

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

    void readChar(std::vector<std::unique_ptr<ZParam>> params);

    void printChar(std::vector<std::unique_ptr<ZParam>> params);

    void setTextStyle(std::vector<std::unique_ptr<ZParam>> params);

    void printString(std::vector<std::unique_ptr<ZParam>> params);

    void printAddress(std::vector<std::unique_ptr<ZParam>> params);

    void printNum(std::vector<std::unique_ptr<ZParam>> params);

    //Call to a routine with spezific name
    void call1n(std::vector<std::unique_ptr<ZParam>> params);

    void callVS(std::vector<std::unique_ptr<ZParam>> params);

    void callVN(std::vector<std::unique_ptr<ZParam>> params);

    void store(std::vector<std::unique_ptr<ZParam>> params);

    void load(std::vector<std::unique_ptr<ZParam>> params);

    void load(ZParam &param1, ZParam &param2);

    void quitRoutine();

    void returnValue(std::vector<std::unique_ptr<ZParam>> params);

    void base2OpOperation(unsigned int opcode, std::vector<std::unique_ptr<ZParam>> &params);

    void add(std::vector<std::unique_ptr<ZParam>> params);

    void sub(std::vector<std::unique_ptr<ZParam>> params);

    void mul(std::vector<std::unique_ptr<ZParam>> params);

    void div(std::vector<std::unique_ptr<ZParam>> params);

    void mod(std::vector<std::unique_ptr<ZParam>> params);

    void doAND(std::vector<std::unique_ptr<ZParam>> params);

    void doOR(std::vector<std::unique_ptr<ZParam>> params);

    void doNOT(std::vector<std::unique_ptr<ZParam>> params);

    void random(std::vector<std::unique_ptr<ZParam>> params);

    void returnTrue();

    void returnFalse();

    void retPopped();

    void pop();

    void printRet(std::vector<std::unique_ptr<ZParam>> params);

    void restart();

    void verify(std::vector<std::unique_ptr<ZParam>> params);

    void pull(std::vector<std::unique_ptr<ZParam>> params);

    void loadb(std::vector<std::unique_ptr<ZParam>> &params, std::shared_ptr<ZCodeContainer> dynamicMemor);

    void loadw(std::vector<std::unique_ptr<ZParam>> &params, std::shared_ptr<ZCodeContainer> dynamicMemor);

    void storeb(std::vector<std::unique_ptr<ZParam>> &params, std::shared_ptr<ZCodeContainer> dynamicMemor);

    void storew(std::vector<std::unique_ptr<ZParam>> &params, std::shared_ptr<ZCodeContainer> dynamicMemor);

    void push(std::vector<std::unique_ptr<ZParam>> params);

    void mouseWindow(std::vector<std::unique_ptr<ZParam>> params);

    void readMouse(std::string array, std::shared_ptr<ZCodeContainer> dynamicMemory);

    void getCursor(std::string array, std::shared_ptr<ZCodeContainer> dynamicMemory);

    void setCursor(std::vector<std::unique_ptr<ZParam>> params);

    void getWindowProperty(std::vector<std::unique_ptr<ZParam>> params);

    void loadb(std::vector<std::unique_ptr<ZParam>> &params);

    void windowStyle(std::vector<std::unique_ptr<ZParam>> params);

    void scrollWindow(std::vector<std::unique_ptr<ZParam>> params);

    void putWindProp(std::vector<std::unique_ptr<ZParam>> params);

    void setWindow(std::vector<std::unique_ptr<ZParam>> params);

    void windowSize(std::vector<std::unique_ptr<ZParam>> params);

    void setMargins(std::vector<std::unique_ptr<ZParam>> params);

    void eraseWindow(std::vector<std::unique_ptr<ZParam>> params);

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
                CALL_VS = 224,
                CALL_VN = 249,
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
                SET_TEXT_STYLE = 241,
        // Opcode : 2OP:20 14 add a b -> (result)
                ADD = 20,
        // Opcode : 2OP:21 15 sub a b -> (result)
                SUB = 21,
        // Opcode : 2OP:22 16 mul a b -> (result)
                MUL = 22,
        // Opcode : 2OP:23 17 div a b -> (result)
                DIV = 23,
        // Opcode : 2OP:24 18 mod a b -> (result)
                MOD = 24,
        // Opcode : 2OP:9 9 and a b -> (result)
                AND = 9,
        // Opcode : 2OP:8 8 or a b -> (result)
                OR = 8,
        // Opcode : VAR:248 18 not value -> (result)
                NOT = 248,
        // Opcode: return true
                RETURN_TRUE = 176,
        // Opcode: return false
                RETURN_FALSE = 177,
        // Opcode: Print the quoted (literal) Z-encoded string, then print a new-line and then return true (i.e., 1).
                PRINT_RET = 179,
        // Opcode: Restart game
                RESTART = 183,
        // Opcode: Pops top of stack and returns that. (This is equivalent to ret sp, but is one byte cheaper.)
                RET_POPPED = 184,
        //Throws away the top item on the stack. (This was useful to lose unwanted routine call results in early Versions.)
                POP = 185,
        // Opcode: Verification
                VERIFY = 189,
        // Opcode: VAR:226 2 storeb array byte-index value
                STOREB = 226,
        //OPCODE: VAR:225 1 storew array word-index value
                STOREW = 225,
        // Opcode: 2OP:16 10 loadb array byte-index -> (result)
                LOADB = 16,
        // Opcode: 2OP:15 F loadw array word-index -> (result)
                LOADW = 15,
        // Opcode: Pushes value onto the game stack
                PUSH = 232,
        //VAR:233 9 1 pull (variable)
                PULL = 233,
        //VAR:231 7 random range -> (result)
                RANDOM = 231,
        //OPCODE: store cursor position in array
                GET_CURSOR = 240,
        //OPCODE: set cursor position
                SET_CURSOR = 239,
        //OPCODE: set used window to print
                SET_WINDOW = 235,
        //OPCODE: clears window
                ERASE_WINDOW = 237


        /*
         * Not implemented:
         *
         * Zero operand Opcodes:
         * nop, save ?(label), save -> (result), restore ?(label), restore -> (result), pop,
         * catch -> (result), show_status, piracy ?(label)
         */
    };

    enum ExtendedOpcode : unsigned int {
                MOUSE_WINDOW = 23,
                READ_MOUSE = 22,
                GET_WINDOW_PROPERTY = 19,
                WINDOW_STYLE = 18,
                SCROLL_WINDOW = 20,
                PUT_WIND_PROP = 25,
                WINDOW_SIZE = 17,
                SET_MARGINS = 8
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
