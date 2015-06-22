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

void checkParamCount(vector<unique_ptr<ZParam>> &params, unsigned int paramCount1, unsigned int paramCount2 = 0,
                     unsigned int paramCount3 = 0, unsigned int paramCount4 = 0) {

    if (params.size() != paramCount1 && (paramCount2 != 0 && params.size() != paramCount2)
        && (paramCount3 != 0 && params.size() != paramCount3) && (paramCount4 != 0 && params.size() != paramCount4)) {

        cout << endl << endl << "Wrong parameter count" << endl << endl;
        throw;
    }
}

bool sameType(ZParamType paramType, ZParamType neededType) {
    if (neededType == VARIABLE_OR_VALUE) {
        return paramType == VARIABLE || paramType == VALUE;
    } else if (neededType == EMPTY || paramType == EMPTY) {
        return false;
    } else {
        return paramType == neededType;
    }
}

void checkParamType(vector<unique_ptr<ZParam>> &params, ZParamType type1, ZParamType type2 = EMPTY, ZParamType type3 = EMPTY,
                    ZParamType type4 = EMPTY, ZParamType type5 = EMPTY) {

    for (size_t i = 0; i < params.size(); i++) {
        switch (i) {
            case 0:
                if (!sameType((*params.at(0)).getParamType(), type1)) {
                    cout << endl << endl << "Wrong param type for parameter 1" << endl << endl;
                    throw;
                }
                break;
            case 1:
                if (!sameType((*params.at(1)).getParamType(), type2)) {
                    cout << endl << endl << "Wrong param type for parameter 2" << endl << endl;
                    throw;
                }
                break;
            case 2:
                if (!sameType((*params.at(2)).getParamType(), type3)) {
                    cout << endl << endl << "Wrong param type for parameter 3" << endl << endl;
                    throw;
                }
                break;
            case 3:
                if (!sameType((*params.at(3)).getParamType(), type4)) {
                    cout << endl << endl << "Wrong param type for parameter 4" << endl << endl;
                    throw;
                }
                break;
            case 4:
                if (!sameType((*params.at(4)).getParamType(), type5)) {
                    cout << endl << endl << "Wrong param type for parameter 5" << endl << endl;
                    throw;
                }
                break;
            default:
                cout << endl << endl << "Too many arguments!" << endl << endl;
                throw;
        }
    }

}

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

// params: stringToPrint
void RoutineGenerator::printString(vector<unique_ptr<ZParam>> params) {
    checkParamCount(params, 1);
    checkParamType(params, NAME);

    ZCodeConverter converter = ZCodeConverter();
    vector<bitset<8>> zsciiString = converter.convertStringToZSCII((*params.at(0)).name);

    unsigned long len = zsciiString.size();
    for (size_t i = 0; i < len; i++) {
        if (i % 96 == 0) {
            addOneByte(numberToBitset(PRINT));
        }
        if (i % 96 == 94) {
            zsciiString[i].set(7, true);
        }
        addOneByte(zsciiString[i]);
    }
}

// params: storeAddress
void RoutineGenerator::readChar(vector<unique_ptr<ZParam>> params) {
    // TODO: decide on whether to allow no args for read_char
    checkParamCount(params, 1);
    checkParamType(params, STORE_ADDRESS);

    // Read char needs '1' as first parameter (cannot be handled by OpcodeParameterGenerator)
    addOneByte(numberToBitset(READ_CHAR));
    addOneByte(numberToBitset(0xbf));
    addOneByte(numberToBitset(1));
    addOneByte(numberToBitset((*params.at(0)).getZCodeValue()));
}

// params: variable
void RoutineGenerator::printChar(vector<unique_ptr<ZParam>> params) {
    checkParamCount(params, 1);
    checkParamType(params, VARIABLE);

    vector<bitset<8>> instructions = opcodeGenerator.generateVarOPInstruction(PRINT_CHAR, params);
    addBitset(instructions);
}

// params: routineName, (arg1, (arg2, (arg3))) resultAddress
void RoutineGenerator::callVS(vector<unique_ptr<ZParam>> params) {
    checkParamCount(params, 2, 3, 4, 5);
    if (params.size() == 2) {
        checkParamType(params, NAME, STORE_ADDRESS);
    } else if (params.size() == 3) {
        checkParamType(params, NAME, VARIABLE_OR_VALUE, STORE_ADDRESS);
    } if (params.size() == 4) {
        checkParamType(params, NAME, VARIABLE_OR_VALUE, VARIABLE_OR_VALUE, STORE_ADDRESS);
    } if (params.size() == 5) {
        checkParamType(params, NAME, VARIABLE_OR_VALUE, VARIABLE_OR_VALUE, VARIABLE_OR_VALUE, STORE_ADDRESS);
    }

    string routineName = (*params.at(0)).name;
    u_int8_t storeAddress = (*params.at(params.size() - 1)).getZCodeValue();

    params.erase(params.begin() + params.size());
    params[0] = unique_ptr<ZValueParam>(new ZValueParam(3000));     // placeholder for call offset

    auto instructions = opcodeGenerator.generateVarOPInstruction(CALL_VS, params);
    RoutineGenerator::callTo[offsetOfRoutine + routineZcode.size() + 2] = routineName;
    addBitset(instructions);
    addOneByte(numberToBitset(storeAddress));
}

// params: variableOrConstant
void RoutineGenerator::call1n(vector<unique_ptr<ZParam>> params) {
    checkParamCount(params, 1);
    checkParamType(params, NAME);

    vector<bitset<8>> instructions = opcodeGenerator.generate1OPInstruction(CALL_1N, (u_int16_t) 3000, false);
    addBitset(instructions);
    RoutineGenerator::callTo[offsetOfRoutine + routineZcode.size() - 2] = (*params.at(0)).name;
    std::cout << "Call Routine at:::" << offsetOfRoutine + routineZcode.size() - 2 << "\n";
}

std::bitset<8> RoutineGenerator::numberToBitset(unsigned int number) {
    return bitset<8>(number);
}

void RoutineGenerator::newLine() {
    addOneByte(numberToBitset(NEW_LINE));
}

void RoutineGenerator::quitRoutine() {
    addOneByte(numberToBitset(QUIT));
}

// adds label and next instruction address to 'branches' map
void RoutineGenerator::newLabel(string label) {
    jumps.newLabel(label);
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
void RoutineGenerator::jumpZero(vector<unique_ptr<ZParam>> params) {
    checkParamCount(params, 2);
    checkParamType(params, VARIABLE_OR_VALUE, NAME);

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
    checkParamCount(params, 3, 4, 5);
    if (params.size() == 3) {
        checkParamType(params, VARIABLE_OR_VALUE, VARIABLE_OR_VALUE, NAME);
    } else if (params.size() == 4) {
        checkParamType(params, VARIABLE_OR_VALUE, VARIABLE_OR_VALUE, VARIABLE_OR_VALUE, NAME);
    } else {
        checkParamType(params, VARIABLE_OR_VALUE, VARIABLE_OR_VALUE, VARIABLE_OR_VALUE, VARIABLE_OR_VALUE, NAME);
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
    checkParamCount(params, 3);
    checkParamType(params, VARIABLE_OR_VALUE, VARIABLE_OR_VALUE, NAME);

    string label;
    bool jumpIfTrue;
    setLabelValues(*params.at(params.size() - 1), label, jumpIfTrue);

    conditionalJump(JL, label, jumpIfTrue, *params.at(0), *params.at(1));
}

// params: param1, param2, label
void RoutineGenerator::jumpGreaterThan(vector<unique_ptr<ZParam>> params) {
    checkParamCount(params, 3);
    checkParamType(params, VARIABLE_OR_VALUE, VARIABLE_OR_VALUE, NAME);

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

// params: address, value
// both parameters need to be constansts!!! (first argument is not an addressParameter!)
void RoutineGenerator::store(vector<unique_ptr<ZParam>> params) {
    checkParamCount(params, 2);
    checkParamType(params, VARIABLE_OR_VALUE, VALUE);

    unique_ptr<ZParam> address (new ZValueParam((*params.at(0)).getZCodeValue()));

    vector<bitset<8>> instructions = opcodeGenerator.generate2OPInstruction(STORE, *address, *params.at(1));
    addBitset(instructions);
}

// params: address, resultAddress
void RoutineGenerator::load(vector<unique_ptr<ZParam>> params) {
    checkParamCount(params, 2);
    checkParamType(params, VARIABLE, STORE_ADDRESS);

    // TODO: Test again

    vector<bitset<8>> instructions = opcodeGenerator.generate1OPInstruction(LOAD, *params.at(0));
    addBitset(instructions);

    addOneByte(numberToBitset((*params.at(1)).getZCodeValue()));
}

// params: address
void RoutineGenerator::printAddress(vector<unique_ptr<ZParam>> params) {
    checkParamCount(params, 1);
    checkParamType(params, VARIABLE_OR_VALUE);

    // TODO: Test this opcode with extra string table

    vector<bitset<8>> instructions = opcodeGenerator.generate1OPInstruction(PRINT_ADDR, *params.at(0));
    addBitset(instructions);
}

void RoutineGenerator::setLocalVariable(string name, int16_t value) {
    if (++addedLocalVariables > maxLocalVariables) {
        cout << "Added " << addedLocalVariables << " local variables to routine but only "
            << maxLocalVariables << " specified at routine start!";
        throw;
    }

    size_t size = locVariables.size() + 1;
    locVariables[name] = size;   // first local variable is at address 1 in stack

    vector<unique_ptr<ZParam>> params;
    params.push_back(unique_ptr<ZParam> (new ZVariableParam(locVariables[name])));
    params.push_back(unique_ptr<ZParam> (new ZValueParam(value)));

    store(move(params));
}

// params: address
void RoutineGenerator::printNum(vector<unique_ptr<ZParam>> params) {
    checkParamCount(params, 1);
    checkParamType(params, VARIABLE_OR_VALUE);

    vector<bitset<8>> instructions = opcodeGenerator.generateVarOPInstruction(PRINT_SIGNED_NUM, params);
    addBitset(instructions);
}

u_int8_t RoutineGenerator::getAddressOfVariable(string name) {
    if (locVariables.count(name) == 0) {
        cout << "Undefined local variable used: " << name << endl;
        throw;
    } else {
        return locVariables[name];
    }
}


bool RoutineGenerator::containsLocalVariable(string name) {
    return locVariables.count(name);
}

// params: variableOrConstant
void RoutineGenerator::returnValue(vector<unique_ptr<ZParam>> params) {
    checkParamCount(params, 1);
    checkParamType(params, VARIABLE_OR_VALUE);

    vector<bitset<8>> instructions = opcodeGenerator.generate1OPInstruction(RET_VALUE, *params.at(0));
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