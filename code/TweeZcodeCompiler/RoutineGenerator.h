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
    std::bitset<8> numberToBitset(int number);

public:
    std::vector<std::bitset<8>> printPrintRoutine(std::string stringToPrint);
    enum Opcode{
        //Opcode for print operation; following by Z-character String
                PRINT = 178,
        //Opcode: quit the main; no arguments.
                QUIT = 186
    };
};



#endif //TWEEZCODECOMPILER_ROUTINEGENERATOR_H
