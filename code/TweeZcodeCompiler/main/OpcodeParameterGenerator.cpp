//
// Created by tobias on 6/8/15.
//

#include "OpcodeParameterGenerator.h"
#include <iostream>
#include <plog/Log.h>

using namespace std;

vector<bitset<8>> OpcodeParameterGenerator::generate1OPInstruction(unsigned int opcode, ZParam &param) {
    return generate1OPInstruction(opcode, param.getZCodeValue(), param.isVariableArgument());
}

vector<bitset<8>> OpcodeParameterGenerator::generate1OPInstruction(unsigned int opcode, uint16_t param,
                                                                   bool paramIsVariable) {
    vector<bitset<8>> instructions;

    bitset<8> opcodeByte = bitset<8>(opcode);
    bool oneByteParameter = true;

    // 10 = short form
    opcodeByte.set(7, true);
    opcodeByte.set(6, false);

    if (paramIsVariable) {
        opcodeByte.set(5, paramIsVariable); // type variable (10)
    } else {
        if (param < 256) {
            opcodeByte.set(4, true);  // type small constant (01)
        } else {
            oneByteParameter = false;   // type large constant (00)
        }
    }
    instructions.push_back(opcodeByte);

    if (oneByteParameter) {
        instructions.push_back(bitset<8>(param));
    } else {
        addLargeNumber((int16_t) param, instructions);
    }

    return instructions;
}

vector<bitset<8>> OpcodeParameterGenerator::generate2OPInstruction(unsigned int opcode, ZParam &param1,
                                                                   ZParam &param2) {
    return generate2OPInstruction(opcode, param1.getZCodeValue(), param2.getZCodeValue(), param1.isVariableArgument(),
                                  param2.isVariableArgument());
}

vector<bitset<8>> OpcodeParameterGenerator::generate2OPInstruction(unsigned int opcode, uint16_t param1,
                                                                   uint16_t param2, bool param1IsVariable,
                                                                   bool param2IsVariable) {
    bitset<8> opcodeByte = bitset<8>(opcode);
    bool longForm = true; // used for 2 operands

    if (param1 > 255 || param2 > 255) {
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

        parameters.push_back(bitset<8>(param1));
        parameters.push_back(bitset<8>(param2));
    } else {
        // variable form needed for large constants
        opcodeByte.set(7, true);
        opcodeByte.set(6, true);

        // variable form with 2 operands
        opcodeByte.set(5, false);

        vector<uint16_t> params;
        vector<bool> paramIsVariable;

        params.push_back(param1);
        paramIsVariable.push_back(param1IsVariable);
        params.push_back(param2);
        paramIsVariable.push_back(param2IsVariable);

        parameters = generateTypeBitsetAndParameterBitsets(params, paramIsVariable);
    }

    vector<bitset<8>> instructions;

    instructions.push_back(opcodeByte);
    for (bitset<8> bitset : parameters) {
        instructions.push_back(bitset);
    }

    return instructions;
}

vector<bitset<8>> OpcodeParameterGenerator::generateVarOPInstruction(unsigned int opcode,
                                                                     vector<unique_ptr<ZParam>> &params) {
    vector<uint16_t> paramValues;
    vector<bool> paramIsVariable;

    for (size_t i = 0; i < params.size(); i++) {
        paramValues.push_back((*params.at(i)).getZCodeValue());
        paramIsVariable.push_back((*params.at(i)).isVariableArgument());
    }

    return generateVarOPInstruction(opcode, paramValues, paramIsVariable);
}

vector<bitset<8>> OpcodeParameterGenerator::generateVarOPInstruction(unsigned int opcode, vector<uint16_t> params,
                                                                     vector<bool> paramIsVariable) {
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

    vector<bitset<8>> parameters = generateTypeBitsetAndParameterBitsets(params, paramIsVariable);

    vector<bitset<8>> instructions;
    instructions.push_back(opcodeByte);

    for (bitset<8> bitset : parameters) {
        instructions.push_back(bitset);
    }

    return instructions;
}

vector<bitset<8>> OpcodeParameterGenerator::generateTypeBitsetAndParameterBitsets(vector<uint16_t> params,
                                                                                  vector<bool> paramIsVariable) {
    vector<bitset<8>> instructions;
    bitset<8> paramTypes;

    if (params.size() > 4) {
        LOG_ERROR << "More than 4 operands are not allowed!";
        // this is a programming error, so we don't throw a specific exception
        throw exception();
    }

    size_t param = 0;
    for (int i = 7; i > 0; i -= 2) {
        if (param >= params.size()) {
            // if less than 4 parameter types needed set last bits to type omitted
            paramTypes.set(i, true);
            paramTypes.set(i - 1, true);
        } else if (paramIsVariable[param]) {
            // type variable
            paramTypes.set(i, true);
            paramTypes.set(i - 1, false);

            instructions.push_back(bitset<8>(params[param]));
        } else if (params[param] < 256) {
            // type small constant
            paramTypes.set(i, false);
            paramTypes.set(i - 1, true);

            instructions.push_back(bitset<8>(params[param]));
        } else {
            // type large constant
            paramTypes.set(i, false);
            paramTypes.set(i - 1, false);

            addLargeNumber((int16_t) params[param], instructions);
        }

        param++;
    }

    instructions.insert(instructions.begin(), paramTypes);

    return instructions;
}

void OpcodeParameterGenerator::addLargeNumber(int16_t val, vector<bitset<8>> &vectorBitset) {
    bitset<16> shortVal((unsigned long long int) val);
    bitset<8> firstHalf, secondHalf;

    for (size_t i = 0; i < 8; i++) {
        secondHalf.set(i, shortVal[i]);
    }

    for (size_t i = 8; i < 16; i++) {
        firstHalf.set(i - 8, shortVal[i]);
    }

    vectorBitset.push_back(firstHalf);
    vectorBitset.push_back(secondHalf);
}