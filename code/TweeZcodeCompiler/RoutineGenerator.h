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
    std::bitset<8> numberOfLocalVariables(int number);
    std::bitset<8> getOpcode(int number);

public:
    std::vector<std::bitset<8>> getHelloWorldRoutine(std::string stringToPrint);

};


#endif //TWEEZCODECOMPILER_ROUTINEGENERATOR_H
