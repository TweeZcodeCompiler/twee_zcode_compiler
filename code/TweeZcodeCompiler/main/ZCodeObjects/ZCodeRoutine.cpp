//
// Created by philip on 27.06.15.
//

#include "ZCodeRoutine.h"
#include "../Utils.h"
#include "../ParamStructs.h"

using std::vector
using std::bitset
using std::shared_ptr

std::map<std::string,std::shared_ptr<ZCodeRoutine>> ZCodeRoutine::routines = std::map<std::string,std::shared_ptr<ZCodeRoutine>>();

std::shared_ptr<ZCodeRoutine> ZCodeRoutine::getOrCreateRoutine(std::string name, std::uint8_t locVariables) {
    if (routines.count(name) == 0) {
        auto routine = std::shared_ptr<ZCodeRoutine>(new ZCodeRoutine(locVariables));
        routine ->displayName = name;
        routines.insert(std::pair<std::string, std::shared_ptr<ZCodeRoutine>>(name,routine));
        return routine;
    } else {
        auto routine = routines.at(name);
        if(routine->localVariables < locVariables){
            routine->localVariables = locVariables;
        }
        return routine;
    }
}

void ZCodeRoutine::print(std::vector<std::bitset<8>> &code) {
   code.push_back(std::bitset<8>(localVariables));
    ZCodeContainer::print(code);
}

vector<bitset<8>> ZCodeRoutine::generate1OPInstruction(unsigned int opcode, ZParam &param) {
    vector<bitset<8>> instructions;
    bool paramIsVariable = param.isVariableArgument();
    bool paramIsName = param.isNameParam;
    bitset<8> opcodeByte = bitset<8>(opcode);
    bool oneByteParameter = true;

    // 10 = short form
    opcodeByte.set(7, true);
    opcodeByte.set(6, false);
    if(paramIsName){
        instructions.push_back(opcodeByte);
        std::string name = param.name;
        this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(instructions)));

    }
    uint16_t paramValue = param.isVariableArgument();
    if (paramIsVariable) {
        opcodeByte.set(5, paramIsVariable); // type variable (10)
    } else{

        if (paramValue < 256) {
            opcodeByte.set(4, true);  // type small constant (01)
        } else {
            oneByteParameter = false;   // type large constant (00)
        }
    }
    instructions.push_back(opcodeByte);

    if (oneByteParameter) {
        instructions.push_back(bitset<8>(paramValue));
    } else {
        Utils::addTwoBytes((int16_t) paramValue, instructions);
    }
     this->add(std::shared_ptr<ZCodeObject>(new ZCodeInstruction(instructions)));
    return instructions;
}