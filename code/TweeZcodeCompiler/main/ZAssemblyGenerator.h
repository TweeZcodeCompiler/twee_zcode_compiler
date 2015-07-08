//
// Created by Danni on 09.06.15.
//

#ifndef PROJECT_ZASSEMBLYGENERATOR_H
#define PROJECT_ZASSEMBLYGENERATOR_H

#include <ostream>
#include <map>
#include <string>
#include <utility>
#include <initializer_list>
#include <vector>
#include "optional.hpp"

typedef const std::string INST_TYPE;

struct ZRoutineArgument {
    ZRoutineArgument(std::string argName) : argName(argName), value(std::experimental::nullopt) { }

    ZRoutineArgument(std::string argName, std::string value) : argName(argName), value(value) { }

    std::string argName;
    std::experimental::optional<std::string> value;
};

/*
 * This class will not check if generated code is sane.
 */
class ZAssemblyGenerator {
public:
    static const std::string STACK_POINTER;
    // to remedy some compat quirks with ZAPF
    static const bool ZAPF_MODE;

    static std::string makeArgs(std::initializer_list<std::string> args);

    ZAssemblyGenerator(std::ostream &out);

    ZAssemblyGenerator &addLabel(std::string labelName);

    ZAssemblyGenerator &addByteArray(std::string name, unsigned size);

    ZAssemblyGenerator &call_vs(std::string routineName, std::experimental::optional<std::string> args, std::string storeTarget);

    ZAssemblyGenerator &call_1n(std::string routineName);

    ZAssemblyGenerator &markStart();

    ZAssemblyGenerator &addRoutine(std::string routineName,
                                   std::vector<ZRoutineArgument> args = std::vector<ZRoutineArgument>());

    ZAssemblyGenerator &addGlobal(std::string globalName);

    ZAssemblyGenerator &jump(std::string label);

    ZAssemblyGenerator &jumpEquals(std::string args, std::string targetLabel);

    ZAssemblyGenerator &jumpNotEquals(std::string args, std::string targetLabel);
    
    ZAssemblyGenerator &jumpGreater(std::string args, std::string targetLabel);

    ZAssemblyGenerator &jumpLess(std::string args, std::string targetLabel);

    ZAssemblyGenerator &jumpGreaterEquals(std::string args, std::string targetLabel);

    ZAssemblyGenerator &jumpLower(std::string args, std::string targetLabel);

    ZAssemblyGenerator &jumpLowerEquals(std::string args, std::string targetLabel);

    ZAssemblyGenerator &read_char(std::string storeTarget);

    ZAssemblyGenerator &quit();

    ZAssemblyGenerator &random();

    ZAssemblyGenerator &ret(std::string arg);

    ZAssemblyGenerator &newline();

    ZAssemblyGenerator &setTextStyle(std::string values);

    ZAssemblyGenerator &print(std::string str);
    
    ZAssemblyGenerator &println(std::string str);

    ZAssemblyGenerator &print_num(std::string value);

    ZAssemblyGenerator &push(std::string value);

    ZAssemblyGenerator &pop();

    ZAssemblyGenerator &variable(std::string variable);
    ZAssemblyGenerator &store(std::string variableName, int value);

    ZAssemblyGenerator &loadb(std::string arrayName, unsigned int index, std::string storeTarget);

    ZAssemblyGenerator &storeb(std::string arrayName, unsigned int index, int value);

    ZAssemblyGenerator &load(std::string source, std::string target);

    ZAssemblyGenerator &store(std::string target, std::string value);

    ZAssemblyGenerator &add(std::string left, std::string right, std::string storeTarget);

    ZAssemblyGenerator &sub(std::string left, std::string right, std::string storeTarget);

    ZAssemblyGenerator &mul(std::string left, std::string right, std::string storeTarget);

    ZAssemblyGenerator &div(std::string left, std::string right, std::string storeTarget);

    ZAssemblyGenerator &mod(std::string left, std::string right, std::string storeTarget);

    ZAssemblyGenerator &land(std::string left, std::string right, std::string storeTarget);

    ZAssemblyGenerator &lor(std::string left, std::string right, std::string storeTarget);

    ZAssemblyGenerator &lnot(std::string variable, std::string storeTarget);

    ZAssemblyGenerator &nop();

    ZAssemblyGenerator &random(std::string range, std::string storeTarget);


private:
    std::ostream &out;
    std::map<std::string, int> passageName2id;

    ZAssemblyGenerator &addInstruction(INST_TYPE instruction,
                                       std::experimental::optional<std::string> args,
                                       std::experimental::optional<std::pair<std::string, bool>> targetLabelAndNeg,
                                       std::experimental::optional<std::string> storeTarget);

    ZAssemblyGenerator &addDirective(std::string directiveName, std::experimental::optional<std::string> args);

};


#endif //PROJECT_ZASSEMBLYGENERATOR_H
