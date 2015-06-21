// Implementation of RoutineGenerator.cpp
// Created by philip on 10.05.15.
//

#include "RoutineGenerator.h"
#include "ZCodeConverter.h"
#include <iostream>
#include <algorithm>

using namespace std;

map<string, size_t> RoutineGenerator::routines = map<string, size_t>();
std::map<size_t, std::string> RoutineGenerator::callTo = std::map<size_t, std::string>();

vector<bitset<8>> RoutineGenerator::getRoutine() {
    jumps.calculateOffsets();

    vector<bitset<8>> zcode;
    for (map<int, bitset<8>>::iterator entry = routineZcode.begin(); entry != routineZcode.end(); ++entry) {
        zcode.push_back(entry->second);
    }

    return zcode;
}

void RoutineGenerator::addBitset(vector<bitset<8>> bitsets) {
    for (bitset<8> bitset : bitsets) {
        routineZcode[routineZcode.size()] = bitset;
    }
}

void RoutineGenerator::setTextStyle(bool roman, bool reverseVideo, bool bold, bool italic, bool fixedPitch) {
    vector<uint16_t> param;
    param.push_back(roman ? 0 : (reverseVideo*1) + (bold*2) + (italic*4) + (fixedPitch*8));

    vector<bool> paramBools;
    paramBools.push_back(false);

    vector<bitset<8>> command = opcodeGenerator.generateVarOPInstruction(SET_TEXT_STYLE, param, paramBools);
    addBitset(command);
}

void RoutineGenerator::printString(std::string stringToPrint) {
    ZCodeConverter converter = ZCodeConverter();
    vector<bitset<8>> zsciiString = converter.convertStringToZSCII(stringToPrint);

    unsigned long len = zsciiString.size();
    for (int i = 0; i < len; i++) {
        if (i % 96 == 0) {
            addOneByte(numberToBitset(PRINT));
        }
        if (i % 96 == 94) {
            zsciiString[i].set(7, true);
        }
        addOneByte(zsciiString[i]);
    }

    if (printLogs) {
        cout << "RoutineGenerator: print " << stringToPrint;
    }
}

void RoutineGenerator::readChar(uint8_t var) {
    addOneByte(numberToBitset(READ_CHAR));
    addOneByte(numberToBitset(0xbf));
    addOneByte(numberToBitset(1));
    addOneByte(numberToBitset(var));

    if (printLogs) {
        cout << "RoutineGenerator: readChar " << var;
    }
}

void RoutineGenerator::printChar(uint8_t var) {
    addOneByte(numberToBitset(PRINT_CHAR));
    addOneByte(numberToBitset(0xbf));
    addOneByte(numberToBitset(var));

    if (printLogs) {
        cout << "RoutineGenerator: printChar " << var;
    }
}

void RoutineGenerator::callRoutine(std::string routineName, const uint8_t storeTarget, const ZParam *param1,
                                   const ZParam *param2, const ZParam *param3)
{

    vector<const ZParam*> inputParams = {param1, param2, param3};
    vector<bool> isVariable;
    vector<uint16_t> params;

    params.push_back(3000);
    isVariable.push_back(false);

    for(auto p = inputParams.begin(); p != inputParams.end(); p++) {
        if(*p) {
            params.push_back((*p)->getZCodeValue());
            isVariable.push_back((*p)->isVariableArgument());
        }
    }

    auto instructions = opcodeGenerator.generateVarOPInstruction(CALL_VS, params, isVariable);
    RoutineGenerator::callTo[offsetOfRoutine + routineZcode.size() + 2] = routineName;
    addBitset(instructions);
    addOneByte(numberToBitset(storeTarget));
}

void RoutineGenerator::callRoutine1n(string routineName) {
    vector<bitset<8>> instructions = opcodeGenerator.generate1OPInstruction(CALL_1N, (u_int16_t) 3000, false);
    addBitset(instructions);
    RoutineGenerator::callTo[offsetOfRoutine + routineZcode.size() - 2] = routineName;
    std::cout << "Call Routine at:::" << offsetOfRoutine + routineZcode.size() - 2 << "\n";

    if (printLogs) {
        cout << "RoutineGenerator: callRoutine " << routineName;
    }
}

std::bitset<8> RoutineGenerator::numberToBitset(unsigned int number) {
    return bitset<8>(number);
}

void RoutineGenerator::newLine() {
    addOneByte(numberToBitset(NEW_LINE));
}

void RoutineGenerator::quitRoutine() {
    addOneByte(numberToBitset(QUIT));

    if (printLogs) {
        cout << "RoutineGenerator: quit ";
    }
}

// adds label and next instruction address to 'branches' map
void RoutineGenerator::newLabel(string label) {
    jumps.newLabel(label);

    if (printLogs) {
        cout << "RoutineGenerator: newLabel " << label;
    }
}

void setLabelValues(ZParam &labelParam, string &label, bool &jumpIfTrue) {
    label = labelParam.name;
    jumpIfTrue = true;
    if (label.at(0) == '~') {
        jumpIfTrue = false;
        label = label.substr(1);
    }
}

// params: label
void RoutineGenerator::jump(vector<unique_ptr<ZParam>> params) {
    if (params.size() != 1) {
        cout << "Wrong param count for jump zero" << endl;
        throw;
    } else if (!(*params.at(0)).isNameParam) {
        cout << "No label for jump zero available" << endl;;
        throw;
    }

    string label;
    bool jumpIfTrue;
    setLabelValues(*params.at(0), label, jumpIfTrue);

    addOneByte(numberToBitset(JUMP));
    jumps.newJump(label);
    addTwoBytes(1 << (JUMP_UNCOND_OFFSET_PLACEHOLDER + 7)); // placeholder, will be replaced in getRoutine()
}

// params: param1, label
void RoutineGenerator::jumpZero(std::vector<std::unique_ptr<ZParam>> params) {
    if (params.size() != 2) {
        cout << "Wrong param count for jump zero" << endl;
        throw;
    } else if (!(*params.at(params.size() - 1)).isNameParam) {
        cout << "No label for jump zero available" << endl;;
        throw;
    }

    string label;
    bool jumpIfTrue;
    setLabelValues(*params.at(params.size() - 1), label, jumpIfTrue);

    vector<bitset<8>> instructions = opcodeGenerator.generate1OPInstruction(JZ, *params.at(0));
    addBitset(instructions);

    jumps.newJump(label);

    bitset<8> offsetFirstBits;
    offsetFirstBits.set(JUMP_COND_TRUE, jumpIfTrue);
    offsetFirstBits.set(JUMP_COND_OFFSET_PLACEHOLDER, true);
    offsetFirstBits.set(JUMP_UNCOND_OFFSET_PLACEHOLDER, false);

    // placeholder, will be replaced in getRoutine()
    addOneByte(offsetFirstBits);
    addOneByte(numberToBitset(0));
}

// params: param1, param2, (param3, (param4,)) label
void RoutineGenerator::jumpEquals(vector<unique_ptr<ZParam>> params) {
    if (params.size() < 3 || params.size() > 5) {
        cout << "Wrong param count for jump equals" << endl;
        throw;
    } else if (!(*params.at(params.size() - 1)).isNameParam) {
        cout << "No label for jump equals available" << endl;;
        throw;
    }

    string label;
    bool jumpIfTrue;
    setLabelValues(*params.at(params.size() - 1), label, jumpIfTrue);

    if (params.size() == 3) {
        conditionalJump(JE, label, jumpIfTrue, (*params.at(0)), (*params.at(1)));
    } else {
        params.erase(params.end()); // erase label param
        auto instructions = opcodeGenerator.generateVarOPInstruction(JE, params);
        addBitset(instructions);

        jumps.newJump(label);

        bitset<8> offsetFirstBits;
        offsetFirstBits.set(JUMP_COND_TRUE, jumpIfTrue);
        offsetFirstBits.set(JUMP_COND_OFFSET_PLACEHOLDER, true);
        offsetFirstBits.set(JUMP_UNCOND_OFFSET_PLACEHOLDER, false);

        // placeholder, will be replaced in getRoutine()
        addOneByte(offsetFirstBits);
        addOneByte(numberToBitset(0));
    }
}

// params: param1, param2, label
void RoutineGenerator::jumpLessThan(vector<unique_ptr<ZParam>> params) {
    if (params.size() != 3) {
        cout << "Wrong param count for jump less than" << endl;
        throw;
    } else if (!(*params.at(params.size() - 1)).isNameParam) {
        cout << "No label for jump less than available" << endl;;
        throw;
    }

    string label;
    bool jumpIfTrue;
    setLabelValues(*params.at(params.size() - 1), label, jumpIfTrue);

    conditionalJump(JL, label, jumpIfTrue, *params.at(0), *params.at(1));
}

// params: param1, param2, label
void RoutineGenerator::jumpGreaterThan(vector<unique_ptr<ZParam>> params) {
    if (params.size() != 3) {
        cout << "Wrong param count for jump greater than" << endl;
        throw;
    } else if (!(*params.at(params.size() - 1)).isNameParam) {
        cout << "No label for jump greater than available" << endl;;
        throw;
    }

    string label;
    bool jumpIfTrue;
    setLabelValues(*params.at(params.size() - 1), label, jumpIfTrue);

    conditionalJump(JG, label, jumpIfTrue, *params.at(0), *params.at(1));
}

void RoutineGenerator::conditionalJump(unsigned int opcode, std::string toLabel, bool jumpIfTrue, ZParam& param1, ZParam& param2) {
    vector<bitset<8>> instructions = opcodeGenerator.generate2OPInstruction(opcode, param1, param2);
    addBitset(instructions);

    jumps.newJump(toLabel);

    bitset<8> offsetFirstBits;
    offsetFirstBits.set(JUMP_COND_TRUE, jumpIfTrue);
    offsetFirstBits.set(JUMP_COND_OFFSET_PLACEHOLDER, true);
    offsetFirstBits.set(JUMP_UNCOND_OFFSET_PLACEHOLDER, false);

    // placeholder, will be replaced in getRoutine()
    addOneByte(offsetFirstBits);
    addOneByte(numberToBitset(0));
}

void RoutineGenerator::store(u_int8_t address, u_int16_t value) {
    vector<bitset<8>> instructions = opcodeGenerator.generate2OPInstruction(STORE, address, value, false, false);
    addBitset(instructions);

    if (printLogs) {
        cout << "RoutineGenerator: store (address: " << address << ", value: " << value << ")";
    }
}

void RoutineGenerator::load(u_int8_t address, u_int8_t resultAddress) {
    vector<bitset<8>> instructions = opcodeGenerator.generate1OPInstruction(LOAD, address, true);
    addBitset(instructions);

    addOneByte(numberToBitset(resultAddress));

    if (printLogs) {
        cout << "RoutineGenerator: load (address: " << address << ", resultAddress: " << resultAddress << ")";
    }
}

void RoutineGenerator::printStringAtAddress(u_int8_t address) {
    vector<bitset<8>> instructions = opcodeGenerator.generate1OPInstruction(PRINT_ADDR, address, true);
    addBitset(instructions);

    if (printLogs) {
        cout << "RoutineGenerator: printStringAtAddress " << address;
    }
}

void RoutineGenerator::setLocalVariable(string name, int16_t value) {
    if (++addedLocalVariables > maxLocalVariables) {
        cout << "Added " << addedLocalVariables << " local variables to routine but only "
            << maxLocalVariables << " specified at routine start!";
        throw;
    }

    size_t size = locVariables.size() + 1;
    locVariables[name] = size;   // first local variable is at address 1 in stack
    store(locVariables[name], value);
}

void RoutineGenerator::printNum(unsigned int address) {
    vector<uint16_t> addresses;
    addresses.push_back(address);

    vector<bool> isParam;
    isParam.push_back(true);

    vector<bitset<8>> instructions = opcodeGenerator.generateVarOPInstruction(PRINT_SIGNED_NUM, addresses, isParam);
    addBitset(instructions);
}

u_int8_t RoutineGenerator::getAddressOfVariable(std::string name) {
    if (locVariables[name] == NULL) {
        cout << "Undefined local variable used: " << name << endl;
        throw;
    } else {
        return locVariables[name];
    }
}


bool RoutineGenerator::containsLocalVariable(string name) {
    return locVariables.count(name);
}

void RoutineGenerator::returnValue(const ZParam& param) {
    vector<bitset<8>> instructions = opcodeGenerator.generate1OPInstruction(RET_VALUE, param.getZCodeValue(), param.isVariableArgument());
    addBitset(instructions);
}

void RoutineGenerator::resolveCallInstructions(std::vector<std::bitset<8>> &zCode) {
    typedef map<size_t, string>::iterator it_type;
    for (it_type it = RoutineGenerator::callTo.begin(); it != RoutineGenerator::callTo.end(); it++) {
        size_t calledRoutineOffset = RoutineGenerator::routines[it->second];
        size_t callOffset = it->first;
        vector<bitset<8>> callAdress = vector<bitset<8>>();
        Utils::addTwoBytes(calledRoutineOffset / 8, callAdress);
        zCode[callOffset] = callAdress[0];
        zCode[callOffset + 1] = callAdress[1];
    }
}

void RoutineGenerator::addTwoBytes(int16_t number, int pos) {
    bitset<16> shortVal((unsigned long long int) number);
    bitset<8> firstHalf, secondHalf;

    for (size_t i = 0; i < 8; i++) {
        secondHalf.set(i, shortVal[i]);
    }

    for (size_t i = 8; i < 16; i++) {
        firstHalf.set(i - 8, shortVal[i]);
    }

    if (pos >= 0) {
        addOneByte(firstHalf, pos);
        addOneByte(secondHalf, pos + 1);
    } else {
        addOneByte(firstHalf);
        addOneByte(secondHalf);
    }
}

void RoutineGenerator::addOneByte(std::bitset<8> byte, int pos) {
    routineZcode[pos < 0 ? routineZcode.size() : (unsigned long) pos] = byte;
}