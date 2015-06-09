//
// Created by Danni on 09.06.15.
//

#include "ZAssemblyGenerator.h"

using namespace std;
using namespace std::experimental;

static const std::string INST_START = "\t";
static const std::string INST_END = "\n";
static const std::string INST_LABEL_MARKER = "?";
static const std::string INST_JUMP_NEG_MARKER = "~";
static const std::string INST_STORE_TARGET_MARKER = "->";
static const std::string INST_SEPARATOR = " ";
static const std::string LABEL_MARKER = ":";

static const std::string DIRECTIVE_START = ".";

namespace directive {
    INST_TYPE ROUTINE = "FUNCT";
    INST_TYPE GLOBAL_VAR = "GVAR";
}

namespace instruction {
    INST_TYPE CALL = "call_vs";
#ifdef ZAPF_MODE
    INST_TYPE JUMP_EQUALS = "jeq";
#else
    INST_TYPE JUMP_EQUALS = "je";
#endif
    INST_TYPE JUMP = "jump";
    INST_TYPE QUIT = "quit";
    INST_TYPE RETURN = "ret";
    INST_TYPE NEWLINE = "new_line";
    INST_TYPE PRINT = "print";
    INST_TYPE READ_CHAR = "read_char";
    INST_TYPE JUMP_GREATER = "jg";
}

ZAssemblyGenerator::ZAssemblyGenerator(ostream& out) : out(out) { }

const std::string ZAssemblyGenerator::STACK_POINTER = "sp";

ZAssemblyGenerator& ZAssemblyGenerator::addLabel(std::string labelName) {
    out << labelName << LABEL_MARKER << INST_END;
    return *this;
}

#pragma mark - directives

void ZAssemblyGenerator::addDirective(std::string directiveName, std::experimental::optional<std::string> args) {
    out << DIRECTIVE_START
        << directiveName;

    if(args) {
        out << INST_SEPARATOR;
        out << *args;
    }

    out << INST_END;
}

ZAssemblyGenerator& ZAssemblyGenerator::addRoutine(std::string routineName) {
    addDirective(directive::ROUTINE, routineName);
    return *this;
}

ZAssemblyGenerator& ZAssemblyGenerator::addGlobal(std::string globalName) {
    addDirective(directive::GLOBAL_VAR, globalName);
    return *this;
}

#pragma mark - instructions

void ZAssemblyGenerator::addInstruction(INST_TYPE instruction,
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
}

ZAssemblyGenerator &ZAssemblyGenerator::jump(std::string targetLabel) {
    addInstruction(instruction::JUMP, nullopt, make_pair(targetLabel, false), nullopt);
    return *this;
}

ZAssemblyGenerator &ZAssemblyGenerator::call(std::string routineName) {
    addInstruction(instruction::CALL, routineName, nullopt, nullopt);
    return *this;
}

ZAssemblyGenerator &ZAssemblyGenerator::jumpEquals(std::string args, std::string targetLabel) {
    addInstruction(instruction::JUMP_EQUALS, args, make_pair(targetLabel, false), nullopt);
    return *this;
}

ZAssemblyGenerator &ZAssemblyGenerator::jumpGreater(std::string args, std::string targetLabel) {
    addInstruction(instruction::JUMP_GREATER, args, make_pair(targetLabel, false), nullopt);
    return *this;
}

ZAssemblyGenerator &ZAssemblyGenerator::quit() {
    addInstruction(instruction::QUIT, nullopt, nullopt, nullopt);
    return *this;
}

ZAssemblyGenerator &ZAssemblyGenerator::ret(std::string arg) {
    addInstruction(instruction::RETURN, arg, nullopt, nullopt);
    return *this;
}

ZAssemblyGenerator &ZAssemblyGenerator::newline() {
    addInstruction(instruction::NEWLINE, nullopt, nullopt, nullopt);
    return *this;
}

ZAssemblyGenerator &ZAssemblyGenerator::print(std::string str) {
    addInstruction(instruction::PRINT, str, nullopt, nullopt);
    return *this;
}

ZAssemblyGenerator &ZAssemblyGenerator::read_char(std::string storeTarget) {
    addInstruction(instruction::READ_CHAR, string("1"), nullopt, storeTarget);
    return *this;
}
