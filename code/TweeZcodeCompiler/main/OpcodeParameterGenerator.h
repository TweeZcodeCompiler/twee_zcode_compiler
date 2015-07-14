//
// Created by tobias on 6/8/15.
//

#ifndef PROJECT_OPCODEPARAMETERGENERATOR_H
#define PROJECT_OPCODEPARAMETERGENERATOR_H


#include <stdint.h>
#include <bitset>
#include <vector>
#include <memory>
#include "ParamStructs.h"

class OpcodeParameterGenerator {
private:
    void addLargeNumber(int16_t val, std::vector<std::bitset<8>> &vectorBitset);

    std::vector<std::bitset<8>> generateTypeBitsetAndParameterBitsets(std::vector<uint16_t> params,
                                                                      std::vector<bool> paramIsVariable);

public:
    std::vector<std::bitset<8>> generate1OPInstruction(unsigned int opcode, uint16_t param, bool paramIsVariable);

    std::vector<std::bitset<8>> generate1OPInstruction(unsigned int opcode, ZParam &param);

    std::vector<std::bitset<8>> generate2OPInstruction(unsigned int opcode, uint16_t param1, uint16_t param2,
                                                       bool param1IsVariable, bool param2IsVariable);

    std::vector<std::bitset<8>> generate2OPInstruction(unsigned int opcode, ZParam &param1, ZParam &param2);

    std::vector<std::bitset<8>> generateVarOPInstruction(unsigned int opcode, std::vector<uint16_t> params,
                                                         std::vector<bool> paramIsVariable);

    std::vector<std::bitset<8>> generateVarOPInstruction(unsigned int opcode,
                                                         std::vector<std::unique_ptr<ZParam>> &params);

    std::vector<std::bitset<8>> generateExtOPInstruction(unsigned int opcode, std::vector<uint16_t> params,
                                                         std::vector<bool> paramIsVariable);

    enum OPERAND_TYPES {
        LARGE, SMALL, VARIABLE, OMITTED
    };

    enum INSTRUCTION_FORM {
        LONG, SHORT, VAR, EXTENDED
    };

    enum OperandTypes {
        // Operand is a variable
                TYPE_VARIABLE = 5,
        // Operand is 1 byte constant
                TYPE_SMALL_CONSTANT = 4
    };

    std::vector<std::bitset<8>> generateExtOPInstruction(unsigned int opcode,
                                                         const std::vector<std::unique_ptr<ZParam>> &params);
};


#endif //PROJECT_OPCODEPARAMETERGENERATOR_H
