//
// Created by philip on 10.05.15.
//

#ifndef TWEEZCODECOMPILER_ROUTINEGENERATOR_H
#define TWEEZCODECOMPILER_ROUTINEGENERATOR_H


#include <vector>
#include <string>
#include <bitset>

class RoutineGenerator {


private:
    std::vector<std::bitset<8>> akk = std::vector<std::bitset<8>>();
    std::bitset<8> numberToBitset(unsigned int number);

public:
    std::vector<std::bitset<8>> getRoutine();
    void printPrintRoutine(std::string stringToPrint);
    void newLine();
    void quitRoutine();

    RoutineGenerator() {
        akk.push_back(numberToBitset(0));  // just a work around; we do not know why it does not work without this
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
