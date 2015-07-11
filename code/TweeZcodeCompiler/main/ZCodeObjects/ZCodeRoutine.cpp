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

void ZCodeRoutine::generate1OPInstruction(unsigned int opcode, ZParam &param,std::string name = "UNKNOWN") {
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
        this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(instructions)));
        this->add(shared_ptr<ZCodeObject>(new ZCodeCallAdress(Utils::dynamicMemory->getOrCreateLabel(name), false)));
        return;
    }
    uint16_t paramValue = param.isVariableArgument();
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
    this->add(std::shared_ptr<ZCodeObject>(new ZCodeInstruction(instructions,name)));
}

void ZCodeRoutine::generate2OPInstruction(unsigned int opcode, ZParam &param1, ZParam &param2, std::string name) {
    bool param1IsVariable = param1.isVariableArgument();
    bool param2IsVariable = param2.isVariableArgument();
    bool param1IsName = param1.isNameParam;
    bool param2IsName = param2.isNameParam;
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
        this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(opcodeByte)));
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
        params.push_back(std::move(std::unique_ptr<ZParam>(&param1)));
        params.push_back(std::move(std::unique_ptr<ZParam>(&param2)));
        this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(opcodeByte)));
        generateTypeBitsetAndParameterBitsets(params);
    }
}

void ZCodeRoutine::generateTypeBitsetAndParameterBitsets(std::vector<std::unique_ptr<ZParam>> &params) {
    vector<shared_ptr<ZCodeObject>> instructions;
    bitset<8> paramTypes;

    if (params.size() > 4) {
        LOG_ERROR << "More than 4 operands are not allowed!";
        // this is a programming error, so we don't throw a specific exception
        throw std::exception();
    }

    size_t param = 0;
    for (int i = 7; i > 0; i -= 2) {
        if (param >= params.size()|| params.at(param)->isStoreAddress) {
            // if less than 4 parameter types needed set last bits to type omitted
            paramTypes.set(i, true);
            paramTypes.set(i - 1, true);
        } else if (params.at(param)->isVariableArgument()) {
            // type variable
            paramTypes.set(i, true);
            paramTypes.set(i - 1, false);
            instructions.push_back(shared_ptr<ZCodeObject>(new ZCodeInstruction(params.at(param)->getZCodeValue())));
        } else if (params[param]->isNameParam) {
            //directive
            paramTypes.set(i, false);
            paramTypes.set(i - 1, false);
            std::string name = params[param]->name;
            instructions.push_back(
                    shared_ptr<ZCodeObject>(new ZCodeCallAdress(Utils::dynamicMemory->getOrCreateLabel(name), false)));
        } else if (params[param]->getZCodeValue() < 256) {
            // type small constant
            paramTypes.set(i, false);
            paramTypes.set(i - 1, true);

            instructions.push_back(shared_ptr<ZCodeObject>(new ZCodeInstruction(params.at(param)->getZCodeValue())));
        } else {
            // type large constant
            paramTypes.set(i, false);
            paramTypes.set(i - 1, false);
            vector<bitset<8>> two;
            Utils::addTwoBytes((int16_t) params[param]->getZCodeValue(), two);
            instructions.push_back(shared_ptr<ZCodeObject>(new ZCodeInstruction(two)));
        }

        param++;
    }
    vector<bitset<8>> paramTypeVector;
    paramTypeVector.push_back(paramTypes);
    this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(paramTypeVector)));
    for (shared_ptr<ZCodeObject> obj: instructions) {
        this->add(obj);
    }
    instructions.clear();
}

void ZCodeRoutine::generateVarOPInstruction(unsigned int opcode,vector<std::unique_ptr<ZParam>> &params, std::string name) {

    bitset<8> opcodeByte = bitset<8>(opcode);

    // variable form with 2OP instruction (2OP Opcodes start with 00)
    if (!opcodeByte.test(7) && !opcodeByte.test(6)) {
        opcodeByte.set(5, false);
    } else {
        opcodeByte.set(5, true);
    }

    // variable form
    opcodeByte.set(7, true);
    opcodeByte.set(6, true);
    this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(opcodeByte,name)));
    generateTypeBitsetAndParameterBitsets(params);
}

void ZCodeRoutine::storeAdress(uint8_t addr) {
    this->add(shared_ptr<ZCodeObject>(new ZCodeInstruction(addr,"store adress: "+addr)));
}