//
// Created by philip on 27.06.15.
//

#include <plog/Log.h>
#include "ZCodeRoutine.h"
#include "../Utils.h"
#include "../ParamStructs.h"
#include "ZCodeCallAdress.h"

using std::vector;
using std::bitset;
using std::shared_ptr;

std::map<std::string, std::shared_ptr<ZCodeRoutine>> ZCodeRoutine::routines = std::map<std::string, std::shared_ptr<ZCodeRoutine>>();

std::shared_ptr<ZCodeRoutine> ZCodeRoutine::getOrCreateRoutine(std::string name, std::uint8_t locVariables) {
    if (routines.count(name) == 0) {
        auto routine = std::shared_ptr<ZCodeRoutine>(new ZCodeRoutine(locVariables));
        routine->displayName = name;
        routines.insert(std::pair<std::string, std::shared_ptr<ZCodeRoutine>>(name, routine));
        return routine;
    } else {
        auto routine = routines.at(name);
        if (routine->localVariables < locVariables) {
            routine->localVariables = locVariables;
        }
        return routine;
    }
}

void ZCodeRoutine::print(std::vector<std::bitset<8>> &code) {
    code.push_back(std::bitset<8>(localVariables));
    ZCodeContainer::print(code);
}

void ZCodeRoutine::generate1OPInstruction(unsigned int opcode, ZParam &param,std::string showName) {
    vector<bitset<8>> instructions;
    bool paramIsVariable = param.isVariableArgument();
    bool paramIsName = param.isNameParam;
    bitset<8> opcodeByte = bitset<8>(opcode);
    bool oneByteParameter = true;

    // 10 = short form
    opcodeByte.set(7, true);
    opcodeByte.set(6, false);
    if (paramIsName) {
        //Parameter is directive
        instructions.push_back(opcodeByte);
        std::string name = param.name;
        this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(instructions, showName)));
        this->add(shared_ptr<ZCodeObject>(new ZCodeCallAdress(Utils::dynamicMemory->getOrCreateLabel(name), false)));

    } else if (param.isRoutine) {
        instructions.push_back(opcodeByte);
        this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(instructions, showName)));
        this->add(shared_ptr<ZCodeObject>(new ZCodeCallAdress(ZCodeRoutine::getOrCreateRoutine(param.name ,0), true)));
    } else {
        uint16_t paramValue = param.getZCodeValue();
        if (paramIsVariable) {
            opcodeByte.set(5, paramIsVariable); // type variable (10)
        } else {

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
        this->add(std::shared_ptr<ZCodeObject>(new ZCodeInstruction(instructions, showName)));
    }
}

void ZCodeRoutine::generate2OPInstruction(unsigned int opcode, ZParam &param1, ZParam &param2, std::string showName) {
    bool param1IsVariable = param1.isVariableArgument();
    bool param2IsVariable = param2.isVariableArgument();
    bool param1IsName = param1.isNameParam || param1.isRoutine;
    bool param2IsName = param2.isNameParam || param2.isRoutine;
    bitset<8> opcodeByte = bitset<8>(opcode);
    bool longForm = true; // used for 2 operands

    if (param1IsName || param1.getZCodeValue() > 255 || param2IsName || param2.getZCodeValue() > 255) {
        // variable form needed
        longForm = false;
    }

    vector<bitset<8>> parameters;

    if (longForm) {
        opcodeByte.set(7, false);

        // bit 5 & 6 represent if param2 or param1 are variables or small constants
        if (param1IsVariable) {
            opcodeByte.set(6, true);
        }
        if (param2IsVariable) {
            opcodeByte.set(5, true);
        }
        this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(opcodeByte,showName)));
        parameters.push_back(bitset<8>(param1.getZCodeValue()));
        parameters.push_back(bitset<8>(param2.getZCodeValue()));
        this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(parameters)));
    } else {
        // variable form needed for large constants
        opcodeByte.set(7, true);
        opcodeByte.set(6, true);

        // variable form with 2 operands
        opcodeByte.set(5, false);

        vector<std::unique_ptr<ZParam>> params;
        params.push_back(std::unique_ptr<ZParam>(param1.clone()));
        params.push_back(std::unique_ptr<ZParam>(param2.clone()));
        this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(opcodeByte,showName)));
        generateTypeBitsetAndParameterBitsets(params);
    }
}

void ZCodeRoutine::generateTypeBitsetAndParameterBitsets(std::vector<std::unique_ptr<ZParam>> &params) {
    vector<shared_ptr<ZCodeObject>> instructions;
    bitset<8> paramTypes;

    // some instructions can take up to 8 parameters
    bool extraLongVars = params.size() > 4;

    if(params.size() > 8) {
        throw std::exception();
    }

    size_t paramIndex = 0;
    for (int i = 7 * (extraLongVars ? 2 : 1); i > 0; i -= 2) {
        if (paramIndex >= params.size()|| params.at(paramIndex)->isStoreAddress) {
            // if less than 4 parameter types needed set last bits to type omitted
            paramTypes.set(i, true);
            paramTypes.set(i - 1, true);
        } else if (params.at(paramIndex)->isVariableArgument()) {
            // type variable
            paramTypes.set(i, true);
            paramTypes.set(i - 1, false);
            uint8_t val = params.at(paramIndex)->getZCodeValue();
            instructions.push_back(shared_ptr<ZCodeObject>(new ZCodeInstruction(val,"VAR:"+std::to_string(val))));
        } else if (params[paramIndex]->isNameParam) {
            //directive
            paramTypes.set(i, false);
            paramTypes.set(i - 1, false);
            std::string name = params[paramIndex]->name;
            instructions.push_back(
                    shared_ptr<ZCodeObject>(new ZCodeCallAdress(Utils::dynamicMemory->getOrCreateLabel(name), false)));
        } else if (params[paramIndex]->isRoutine) {
            //directive
            paramTypes.set(i, false);
            paramTypes.set(i - 1, false);
            std::string name = params[paramIndex]->name;
            instructions.push_back(
                    shared_ptr<ZCodeObject>(new ZCodeCallAdress(ZCodeRoutine::getOrCreateRoutine(name,0), true)));
        } else if (params[paramIndex]->getZCodeValue() < 256) {
            // type small constant
            paramTypes.set(i, false);
            paramTypes.set(i - 1, true);

            instructions.push_back(shared_ptr<ZCodeObject>(new ZCodeInstruction(params.at(paramIndex)->getZCodeValue(),"S:"+std::to_string(params.at(paramIndex)->getZCodeValue()))));
        } else {
            // type large constant
            paramTypes.set(i, false);
            paramTypes.set(i - 1, false);
            vector<bitset<8>> two;
            Utils::addTwoBytes((int16_t) params[paramIndex]->getZCodeValue(), two);
            instructions.push_back(shared_ptr<ZCodeObject>(new ZCodeInstruction(two,"L:"+std::to_string(params.at(paramIndex)->getZCodeValue()))));
        }

        paramIndex++;
    }
    vector<bitset<8>> paramTypeVector;
    paramTypeVector.push_back(paramTypes);
    this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(paramTypeVector, "-"+paramTypes.to_string()+"-")));
    for (shared_ptr<ZCodeObject> obj: instructions) {
        this->add(obj);
    }
    instructions.clear();
}

void ZCodeRoutine::generateVarOPInstruction(unsigned int opcode,vector<std::unique_ptr<ZParam>> &params, std::string showName) {

    bitset<8> opcodeByte = bitset<8>(opcode);

    // variable form with 2OP instruction (2OP Opcodes start with 00)
    if (!opcodeByte.test(7) && !opcodeByte.test(6)) {
        opcodeByte.set(5, false);
    } else {
        opcodeByte.set(5, true);
    }


    if (params.size() > 4) {
        LOG_ERROR << "More than 4 operands are not allowed!";
        // this is a programming error, so we don't throw a specific exception
        throw std::exception();
    }

    // variable form
    opcodeByte.set(7, true);
    opcodeByte.set(6, true);
    this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(opcodeByte,showName)));
    generateTypeBitsetAndParameterBitsets(params);
}

void ZCodeRoutine::storeAdress(uint8_t addr) {
    this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(addr,"store adress: "+addr)));
}


void ZCodeRoutine::generateExtOPInstruction(unsigned int opcode, vector<std::unique_ptr<ZParam>> &params, std::string showName)
{
    this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(190,"EXT")));
    this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(opcode,showName)));
    generateTypeBitsetAndParameterBitsets(params);
}