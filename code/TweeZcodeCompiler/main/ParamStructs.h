//
// Created by tobias on 6/20/15.
//

#ifndef PROJECT_PARAMSTRUCTS_H
#define PROJECT_PARAMSTRUCTS_H

#include <bits/stringfwd.h>
#include <bits/basic_string.h>
#include <stdint.h>

struct ZParam {
    bool isStoreAddress = false, isNameParam = false;
    std::string name;
    virtual bool isVariableArgument() const = 0;

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

    bool isVariableArgument() const { return false; }
};

struct ZVariableParam : public ZParam {
    ZVariableParam(uint16_t variableAddr) {
        valueOrAddress = variableAddr;
    }

    bool isVariableArgument() const { return true; }
};

struct ZStoreParam : public ZParam {
    ZStoreParam(uint16_t storeAddr) {
        valueOrAddress = storeAddr;
        isStoreAddress = true;
    }

    bool isVariableArgument() const { return true; }
};

struct ZNameParam : public ZParam {
    ZNameParam(std::string name) {
        this->name = name;
        isNameParam = true;
    }

    bool isVariableArgument() const { return false; }
};

#endif //PROJECT_PARAMSTRUCTS_H
