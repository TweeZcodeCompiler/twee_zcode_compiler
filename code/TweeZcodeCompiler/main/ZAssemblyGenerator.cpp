//
// Created by Danni on 09.06.15.
//

#include "ZAssemblyGenerator.h"
#include <sstream>
#include <vector>

using namespace std;
using namespace experimental;

static const string INST_START = "\t";
static const string INST_END = "\n";
static const string INST_LABEL_MARKER = "?";
static const string INST_JUMP_NEG_MARKER = "~";
static const string INST_STORE_TARGET_MARKER = "->";
static const string INST_SEPARATOR = " ";
static const string ARGS_EQ = "=";
static const string ARGS_SEPARATOR = ",";
static const string LABEL_MARKER = ":";

static const string DIRECTIVE_START = ".";

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
    INST_TYPE PUSH = "push";
    INST_TYPE NOTHING = "";
}

ZAssemblyGenerator::ZAssemblyGenerator(ostream &out) : out(out) { }

const string ZAssemblyGenerator::STACK_POINTER = "sp";
const bool ZAssemblyGenerator::ZAPF_MODE = false;

string ZAssemblyGenerator::makeArgs(initializer_list<string> args) {
    stringstream ss;
    for (auto arg = args.begin(); arg != args.end(); ++arg) {
        ss << *arg;
        if (arg != args.end() && arg + 1 != args.end()) {
            ss << INST_SEPARATOR;
        }
    }
    return ss.str();
}


ZAssemblyGenerator &ZAssemblyGenerator::addLabel(string labelName) {
    out << labelName << LABEL_MARKER << INST_END;
    return *this;
}

#pragma mark - directives

ZAssemblyGenerator &ZAssemblyGenerator::addDirective(string directiveName, experimental::optional<string> args) {
    out << DIRECTIVE_START
    << directiveName;

    if (args) {
        out << INST_SEPARATOR;
        out << *args;
    }

    out << INST_END;

    return *this;
}

ZAssemblyGenerator &ZAssemblyGenerator::addRoutine(string routineName,
                                                   vector<ZRoutineArgument> args) {
    stringstream ss;
    ss << routineName << INST_SEPARATOR;
    for (auto it = args.begin(); it != args.end(); ++it) {
        ss << it->argName;
        if (it->value) {
            ss << ARGS_EQ << *(it->value);
        }

        if (it + 1 != args.end()) {
            ss << ARGS_SEPARATOR;
        }
    }
    addDirective(directive::ROUTINE, ss.str());
    return *this;
}

ZAssemblyGenerator &ZAssemblyGenerator::addGlobal(string globalName) {
    addDirective(directive::GLOBAL_VAR, globalName);
    return *this;
}

#pragma mark - instructions

ZAssemblyGenerator &ZAssemblyGenerator::addInstruction(INST_TYPE instruction,
                                                       optional<string> args,
                                                       optional<pair<string, bool>> targetLabelAndNeg,
                                                       optional<string> storeTarget) {
    out << INST_START;

    out << instruction;

    if (args) {
        out << INST_SEPARATOR
        << *args;
    }

    if (targetLabelAndNeg) {
        out << INST_SEPARATOR
        << INST_LABEL_MARKER;

        if (targetLabelAndNeg->second) {
            out << INST_JUMP_NEG_MARKER;
        }
        out << targetLabelAndNeg->first;
    }

    if (storeTarget) {
        out << INST_SEPARATOR
        << INST_STORE_TARGET_MARKER
        << INST_SEPARATOR
        << *storeTarget;
    }

    out << INST_END;

    return *this;
}

ZAssemblyGenerator &ZAssemblyGenerator::jump(string targetLabel) {
    if (ZAPF_MODE)
        return addInstruction(instruction::JUMP, targetLabel, nullopt, nullopt);
    else
        return addInstruction(instruction::JUMP, nullopt, make_pair(targetLabel, false), nullopt);
}


ZAssemblyGenerator &ZAssemblyGenerator::markStart() {
    if (ZAPF_MODE) {
        out << "START::" << INST_END;
    }
    return *this;
}

ZAssemblyGenerator &ZAssemblyGenerator::call(string routineName) {
    return addInstruction(instruction::CALL, routineName, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::call(string routineName, string storeTarget) {
    return addInstruction(instruction::CALL, routineName, nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::jumpEquals(string args, string targetLabel) {
    return addInstruction(instruction::JUMP_EQUALS, args, make_pair(targetLabel, false), nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::jumpNotEquals(string args, string targetLabel) {
    return addInstruction(instruction::JUMP_EQUALS, args, make_pair(targetLabel, true), nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::jumpGreater(string args, string targetLabel) {
    return addInstruction(instruction::JUMP_GREATER, args, make_pair(targetLabel, false), nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::quit() {
    return addInstruction(instruction::QUIT, nullopt, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::push(string arg) {
    return addInstruction(instruction::PUSH, arg, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::ret(string arg) {
    return addInstruction(instruction::RETURN, arg, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::newline() {
    return addInstruction(instruction::NEWLINE, nullopt, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::print(string str) {
    return addInstruction(instruction::PRINT, string("\"") + str + string("\""), nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::setTextStyle(bool italic, bool bold, bool underlined) {
    string result;
    if (italic) {
        result += "i";
    }
    if (bold) {
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

ZAssemblyGenerator &ZAssemblyGenerator::read_char(string storeTarget) {
    return addInstruction(instruction::READ_CHAR, string("1"), nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::println(string str) {
    return addInstruction(instruction::PRINT, string("\"") + str + string("\""), nullopt, nullopt).newline();
}

ZAssemblyGenerator &ZAssemblyGenerator::variable(string variable) {
    return addInstruction(instruction::NOTHING, variable.substr(1), nullopt, nullopt);
}