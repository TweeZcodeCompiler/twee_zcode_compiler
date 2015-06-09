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
#include "optional.hpp"

typedef const std::string INST_TYPE;

/*
 * This class will not check if generated code is sane.
 */
class ZAssemblyGenerator {
public:
    ZAssemblyGenerator(std::ostream& out);

    ZAssemblyGenerator& addLabel(std::string labelName);

    ZAssemblyGenerator& addRoutine(std::string routineName);
    ZAssemblyGenerator& addGlobal(std::string globalName);

    ZAssemblyGenerator &jump(std::string label);
    ZAssemblyGenerator &call(std::string routineName);
    ZAssemblyGenerator &call(std::string routineName, std::string storeTarget);
    ZAssemblyGenerator &jumpEquals(std::string args, std::string targetLabel);
    ZAssemblyGenerator &jumpGreater(std::string args, std::string targetLabel);
    ZAssemblyGenerator &quit();
    ZAssemblyGenerator &ret(std::string arg);
    ZAssemblyGenerator &newline();
    ZAssemblyGenerator &print(std::string str);
    ZAssemblyGenerator &read_char(std::string storeTarget);

    static std::string makeArgs(std::initializer_list<std::string> args);

    static const std::string STACK_POINTER;
private:
    std::ostream& out;
    std::map<std::string, int> passageName2id;

    void addInstruction(INST_TYPE instruction,
                                      std::experimental::optional<std::string> args,
                                      std::experimental::optional<std::pair<std::string, bool>> targetLabelAndNeg,
                                      std::experimental::optional<std::string> storeTarget);

    void addDirective(std::string directiveName, std::experimental::optional<std::string> args);

};


#endif //PROJECT_ZASSEMBLYGENERATOR_H
