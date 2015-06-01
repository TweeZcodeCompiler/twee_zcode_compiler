//
// Created by philip on 10.05.15.
//

#ifndef TWEEZCODECOMPILER_ROUTINEGENERATOR_H
#define TWEEZCODECOMPILER_ROUTINEGENERATOR_H


#include <vector>
#include <string>
#include <bitset>
#include <map>

class RoutineGenerator {


private:
    bool quitOpcodePrinted = false;                 // every routine needs to call quit opcode
    int firstInstructionAddress = -1;
    std::vector<std::bitset<8>> akk = std::vector<std::bitset<8>>();
    std::map<std::string, u_int> branches;        // maps branch label to instruction address
    std::map<u_int, std::string> jumpToBranch;    // keys = addresses that need to be filled with branch offset, values = jump destination label

    std::bitset<8> numberToBitset(unsigned int number);
    void addLargeNumber(int16_t number);    // signed number over 2 bytes
    void addLargeNumber(int16_t number, int pos);    // signed number over 2 bytes

public:
    std::vector<std::bitset<8>> getRoutine();
    void printPrintRoutine(std::string stringToPrint);
    void newLine();
    void jump(std::string toLabel);
    void quitRoutine();

    void addBranch(std::string label);

    RoutineGenerator(int instructionNumber) {
        this->firstInstructionAddress = instructionNumber;
        akk.push_back(numberToBitset(0));                   // number of local variables in routine
    }

    enum Opcode{
        //Print new line
                NEW_LINE = 187,
        //Opcodes for jump instructions
                JE = 1,
                JL = 2,
                JG = 3,
                JZ = 128,
                JUMP = 140,
        //Opcode for print operation; following by Z-character String
                PRINT = 178,
        //Opcode: quit the main; no arguments.
                QUIT = 186
    };
};



#endif //TWEEZCODECOMPILER_ROUTINEGENERATOR_H
