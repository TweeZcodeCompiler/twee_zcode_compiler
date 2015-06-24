//
// Created by Danni on 09.06.15.
//

#include "ZAssemblyGenerator.h"
#include <sstream>
#include <vector>

using namespace std;
using namespace std::experimental;

static const std::string INST_START = "\t";
static const std::string INST_END = "\n";
static const std::string INST_LABEL_MARKER = "?";
static const std::string INST_JUMP_NEG_MARKER = "~";
static const std::string INST_STORE_TARGET_MARKER = "->";
static const std::string INST_SEPARATOR = " ";
static const std::string ARGS_EQ = "=";
static const std::string ARGS_SEPARATOR = ",";
static const std::string LABEL_MARKER = ":";

static const std::string DIRECTIVE_START = ".";

namespace directive {
    INST_TYPE ROUTINE = "FUNCT";
    INST_TYPE GLOBAL_VAR = "GVAR";
}

namespace instruction {
    INST_TYPE CALL = "call";
    INST_TYPE JUMP_EQUALS = ZAssemblyGenerator::ZAPF_MODE ? "jeq" : "je";
    INST_TYPE JUMP = "jump";
    INST_TYPE QUIT = "quit";
    INST_TYPE RETURN = "ret";
    INST_TYPE NEWLINE = "new_line";
    INST_TYPE PRINT = "print";
    INST_TYPE READ_CHAR = "read_char";
    INST_TYPE JUMP_GREATER = "jg";
    INST_TYPE SET_TEXT_STYLE = "set_text_style";
}

ZAssemblyGenerator::ZAssemblyGenerator(ostream& out) : out(out) { }

const std::string ZAssemblyGenerator::STACK_POINTER = "sp";
const bool ZAssemblyGenerator::ZAPF_MODE = false;

std::string ZAssemblyGenerator::makeArgs(std::initializer_list<std::string> args) {
    stringstream ss;
    for(auto arg = args.begin(); arg != args.end(); ++arg) {
        ss << *arg;
        if(arg != args.end() && arg+1 != args.end()) {
            ss << INST_SEPARATOR;
        }
    }
    return ss.str();
}


ZAssemblyGenerator& ZAssemblyGenerator::addLabel(std::string labelName) {
    out << labelName << LABEL_MARKER << INST_END;
    return *this;
}

#pragma mark - directives

ZAssemblyGenerator& ZAssemblyGenerator::addDirective(std::string directiveName, std::experimental::optional<std::string> args) {
    out << DIRECTIVE_START
        << directiveName;

    if(args) {
        out << INST_SEPARATOR;
        out << *args;
    }

    out << INST_END;

    return *this;
}

ZAssemblyGenerator& ZAssemblyGenerator::addRoutine(std::string routineName,
                                                   std::vector<ZRoutineArgument> args) {
    stringstream ss;
    ss << routineName << INST_SEPARATOR;
    for(auto it = args.begin(); it != args.end(); ++it) {
        ss << it->argName;
        if(it->value) {
            ss << ARGS_EQ << *(it->value);
        }

        if(it + 1 != args.end()) {
            ss << ARGS_SEPARATOR;
        }
    }
    addDirective(directive::ROUTINE, ss.str());
    return *this;
}

ZAssemblyGenerator& ZAssemblyGenerator::addGlobal(std::string globalName) {
    addDirective(directive::GLOBAL_VAR, globalName);
    return *this;
}

#pragma mark - instructions

ZAssemblyGenerator& ZAssemblyGenerator::addInstruction(INST_TYPE instruction,
                                        optional<std::string> args,
                                        optional<pair<std::string, bool>> targetLabelAndNeg,
                                        optional<std::string> storeTarget)
{
    out << INST_START;

    out << instruction;

    if(args) {
        out << INST_SEPARATOR
            << *args;
    }

    if(targetLabelAndNeg) {
        out << INST_SEPARATOR
        << INST_LABEL_MARKER;

        if(targetLabelAndNeg->second) {
            out << INST_JUMP_NEG_MARKER;
        }
        out << targetLabelAndNeg->first;
    }

    if(storeTarget) {
        out << INST_SEPARATOR
        << INST_STORE_TARGET_MARKER
        << INST_SEPARATOR
        << *storeTarget;
    }

    out << INST_END;

    return *this;
}

ZAssemblyGenerator &ZAssemblyGenerator::jump(std::string targetLabel) {
    if(ZAPF_MODE)
        return addInstruction(instruction::JUMP, targetLabel, nullopt, nullopt);
    else
        return addInstruction(instruction::JUMP, nullopt, make_pair(targetLabel, false), nullopt);
}


ZAssemblyGenerator &ZAssemblyGenerator::markStart() {
    if(ZAPF_MODE) {
        out << "START::" << INST_END;
    }
    return *this;
}

ZAssemblyGenerator &ZAssemblyGenerator::call(std::string routineName) {
    return addInstruction(instruction::CALL, routineName, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::call(std::string routineName, std::string storeTarget) {
    return addInstruction(instruction::CALL, routineName, nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::jumpEquals(std::string args, std::string targetLabel) {
    return addInstruction(instruction::JUMP_EQUALS, args, make_pair(targetLabel, false), nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::jumpGreater(std::string args, std::string targetLabel) {
    return addInstruction(instruction::JUMP_GREATER, args, make_pair(targetLabel, false), nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::quit() {
    return addInstruction(instruction::QUIT, nullopt, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::ret(std::string arg) {
    return addInstruction(instruction::RETURN, arg, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::newline() {
    return addInstruction(instruction::NEWLINE, nullopt, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::print(std::string str) {
    return addInstruction(instruction::PRINT, string("\"") + str + string("\""), nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::setTextStyle(bool italic, bool bold, bool underlined) {
    std::string result;
    if(italic) {
        result += "i";
    }
    if ( bold ) {
        result += "b";
    }
    if (underlined) {
        result += "u";
    }
    if (!(italic && bold && underlined)) {
        result = "r";
    }
    return addInstruction(instruction::SET_TEXT_STYLE, result, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::read_char(std::string storeTarget) {
    return addInstruction(instruction::READ_CHAR, string("1"), nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::println(std::string str) {
    return addInstruction(instruction::PRINT, string("\"") + str + string("\""), nullopt, nullopt).newline();
}

