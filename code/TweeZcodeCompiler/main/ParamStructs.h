//
// Created by tobias on 6/20/15.
//

#ifndef PROJECT_PARAMSTRUCTS_H
#define PROJECT_PARAMSTRUCTS_H

#include <stdint.h>

enum ZParamType {
    VALUE, VARIABLE, STORE_ADDRESS, NAME, ROUTINE,

    // needed for special cases in RoutineGenerator:
            EMPTY, VARIABLE_OR_VALUE, VARIABLE_OR_VALUE_OR_NAME
};

struct ZParam {
    bool isStoreAddress = false, isNameParam = false, isRoutine = false;
    std::string name;

    virtual bool isVariableArgument() const = 0;

    virtual ZParamType getParamType() = 0;

    virtual ZParam *clone() = 0;

    uint16_t getZCodeValue() const {
        return valueOrAddress;
    }

protected:
    uint16_t valueOrAddress;
};

struct ZValueParam : public ZParam {
    ZValueParam(uint16_t value) {
        valueOrAddress = value;
    }

    ZParam *clone() { return new ZValueParam(*this); }

    bool isVariableArgument() const { return false; }

    ZParamType getParamType() {
        return VALUE;
    }
};

struct ZVariableParam : public ZParam {
    ZVariableParam(uint16_t variableAddr) {
        valueOrAddress = variableAddr;
    }

    bool isVariableArgument() const { return true; }

    ZParam *clone() { return new ZVariableParam(*this); }

    ZParamType getParamType() {
        return VARIABLE;
    }
};

struct ZStoreParam : public ZParam {
    ZStoreParam(uint16_t storeAddr) {
        valueOrAddress = storeAddr;
        isStoreAddress = true;
    }

    bool isVariableArgument() const { return true; }

    ZParam *clone() { return new ZStoreParam(*this); }

    ZParamType getParamType() {
        return STORE_ADDRESS;
    }
};

struct ZNameParam : public ZParam {
    ZNameParam(std::string name) {
        this->name = name;
        isNameParam = true;
    }

    bool isVariableArgument() const { return false; }

    ZParam *clone() { return new ZNameParam(*this); }

    ZParamType getParamType() {
        return NAME;
    }
};

struct ZRoutineParam : public ZParam {
    ZRoutineParam(std::string name) {
        this->name = name;
        this->isRoutine = true;
    }

    bool isVariableArgument() const { return false; }

    ZParam *clone() { return new ZRoutineParam(*this); }

    ZParamType getParamType() {
        return ROUTINE;
    }
};

#endif //PROJECT_PARAMSTRUCTS_H
