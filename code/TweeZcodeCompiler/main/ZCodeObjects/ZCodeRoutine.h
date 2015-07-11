//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODEROUTINE_H
#define PROJECT_ZCODEROUTINE_H


#include "ZCodeContainer.h"
#include "ZCodeInstruction.h"
#include "../ParamStructs.h"
#include <map>

class ZCodeRoutine : public ZCodeContainer {
private:
    static std::map<std::string, std::shared_ptr<ZCodeRoutine>> routines;
    uint8_t localVariables;
public:
    void generateTypeBitsetAndParameterBitsets(std::vector<std::unique_ptr<ZParam>> &params);
    void print(std::vector<std::bitset<8>> &code);
    void generate1OPInstruction(unsigned int opcode, ZParam &param, std::string name);
    void generate2OPInstruction(unsigned int opcode, ZParam &param1,ZParam &param2, std::string name = "UNKNOWN");
    void generateVarOPInstruction(unsigned int opcode,std::vector<std::unique_ptr<ZParam>> &params, std::string name = "UNKNOWN");
    void storeAdress(uint8_t adrr);

    ZCodeRoutine(uint8_t locVariables) {
        containerOffset = 1;
        this->localVariables = locVariables;
    }

    static std::shared_ptr<ZCodeRoutine> getOrCreateRoutine(std::string name, uint8_t locVariables);
};


#endif //PROJECT_ZCODEROUTINE_H
