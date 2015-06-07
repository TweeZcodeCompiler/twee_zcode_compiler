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
    std::bitset<8> numberToBitset(unsigned int number);

public:
    std::vector<std::bitset<8>> printPrintRoutine(std::string stringToPrint);
    std::vector<std::bitset<8>> printReadCharInstruction(uint8_t var);
    std::vector<std::bitset<8>> printPrintCharInstruction(uint8_t var);
    std::vector<std::bitset<8>> printPrintStringInstruction(std::string stringToPrint);
    std::vector<std::bitset<8>> printCallToMainAndMain(int offset, int locVar);
    enum Opcode{
        //Opcode for print operation; following by Z-character String
                PRINT = 178,
        //Opcode: quit the main; no arguments.
                QUIT = 186,
        //Opcode: VAR:246 16 4 read_char 1 time routine -> (result)
            READ_CHAR = 246,
        //Opcode: VAR:229 5 print_char output-character-code
            PRINT_CHAR = 229,
        //Opcode: 1OP:143 F 5 call_1n routine
            CALL_1N =143,
        //Opcode: 0OP:187 B new_line
        NEW_LINE = 187
    };
};



#endif //TWEEZCODECOMPILER_ROUTINEGENERATOR_H
