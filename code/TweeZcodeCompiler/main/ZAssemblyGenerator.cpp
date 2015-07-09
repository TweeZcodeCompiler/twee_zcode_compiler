//
// Created by Danni on 09.06.15.
//

#include "ZAssemblyGenerator.h"
#include <sstream>
#include <vector>
#include <plog/Log.h>

using namespace std;
using namespace experimental;

static const string INST_START = "\t";
static const string INST_END = "\n";
static const string INST_LABEL_MARKER = "?";
static const string INST_JUMP_NEG_MARKER = "~";
static const string INST_STORE_TARGET_MARKER = "->";
static const string INST_SEPARATOR = " ";
static const string ARGS_EQ = "=";
static const string ARGS_SEPARATOR = ", ";
static const string LABEL_MARKER = ":";

static const string DIRECTIVE_START = ".";

namespace directive {
    INST_TYPE ROUTINE = "FUNCT";
    INST_TYPE GLOBAL_VAR = "GVAR";
    INST_TYPE BYTE_ARRAY = "BYTEARRAY";
    INST_TYPE WORD_ARRAY = "WORDARRAY";
}

namespace instruction {
    INST_TYPE CALL_VS = "call_vs";
    INST_TYPE JUMP_EQUALS = ZAssemblyGenerator::ZAPF_MODE ? "jeq" : "je";
    INST_TYPE JUMP = "jump";
    INST_TYPE QUIT = "quit";
    INST_TYPE RETURN = "ret";
    INST_TYPE NEWLINE = "new_line";
    INST_TYPE PRINT = "print";
    INST_TYPE READ_CHAR = "read_char";
    INST_TYPE JUMP_GREATER = "jg";
    INST_TYPE SET_TEXT_STYLE = "set_text_style";
    INST_TYPE STOREBYTE = "storeb";
    INST_TYPE NEW_LINE_COMMAND = "new_line";
    INST_TYPE PRINT_COMMAND = "print";
    INST_TYPE JE_COMMAND = "je";
    INST_TYPE JG_COMMAND = "jg";
    INST_TYPE JL_COMMAND = "jl";
    INST_TYPE JZ_COMMAND = "jz";
    INST_TYPE QUIT_COMMAND = "quit";
    INST_TYPE READ_CHAR_COMMAND = "read_char";
    INST_TYPE PRINT_CHAR_COMMAND = "print_char";
    INST_TYPE PRINT_NUM_COMMAND = "print_num";
    INST_TYPE PRINT_ADDR_COMMAND = "print_addr";
    INST_TYPE PUSH_COMMAND = "push";
    INST_TYPE JUMP_COMMAND = "jump";
    INST_TYPE RET_COMMAND = "ret";
    INST_TYPE CALL_VS_COMMAND = "call_vs";
    INST_TYPE CALL_1N_COMMAND = "call_1n";
    INST_TYPE STORE_COMMAND = "store";
    INST_TYPE LOAD_COMMAND = "load";
    INST_TYPE ADD_COMMAND = "add";
    INST_TYPE SUB_COMMAND = "sub";
    INST_TYPE MUL_COMMAND = "mul";
    INST_TYPE DIV_COMMAND = "div";
    INST_TYPE MOD_COMMAND = "mod";
    INST_TYPE AND_COMMAND = "and";
    INST_TYPE OR_COMMAND = "or";
    INST_TYPE NOT_COMMAND = "not";
    INST_TYPE RET_TRUE_COMMAND = "rtrue";
    INST_TYPE RET_FALSE_COMMAND = "rfalse";
    INST_TYPE PRINT_RET_COMMAND = "print_ret";
    INST_TYPE RESTART_COMMAND = "restart";
    INST_TYPE RET_POPPED_COMMAND = "ret_popped";
    INST_TYPE POP_COMMAND = "pop";
    INST_TYPE VERIFY_COMMAND = "verify";
    INST_TYPE RANDOM_COMMAND = "random";
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


ZAssemblyGenerator &ZAssemblyGenerator::addByteArray(std::string name, unsigned size) {
    return addDirective(directive::BYTE_ARRAY, makeArgs({name, string("[") + to_string(size) + string("]")}));
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

ZAssemblyGenerator &ZAssemblyGenerator::storeb(string arrayName, unsigned index, int value) {
    return addInstruction(instruction::STOREBYTE, makeArgs({arrayName, to_string(index), to_string(value)}), nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::storeb(string arrayName, string var, int value) {
    return addInstruction(instruction::STOREBYTE, makeArgs({arrayName, var, to_string(value)}), nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::loadb(string arrayName, unsigned index, string storeTarget) {
    return addInstruction(instruction::STOREBYTE, to_string(index), nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::jump(string targetLabel) {
    if (ZAPF_MODE)
        return addInstruction(instruction::JUMP_COMMAND, targetLabel, nullopt, nullopt);
    else
        return addInstruction(instruction::JUMP_COMMAND, nullopt, make_pair(targetLabel, false), nullopt);
}


ZAssemblyGenerator &ZAssemblyGenerator::markStart() {
    if (ZAPF_MODE) {
        out << "START::" << INST_END;
    }
    return *this;
}

ZAssemblyGenerator &ZAssemblyGenerator::call_vs(string routineName, optional<string> args, string storeTarget) {
    return addInstruction(instruction::CALL_VS, routineName + (args ? (INST_SEPARATOR + *args) : ""), nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::call_1n(string routineName) {
    return addInstruction(instruction::CALL_1N_COMMAND, routineName, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::jumpEquals(string args, string targetLabel) {
    return addInstruction(instruction::JE_COMMAND, args, make_pair(targetLabel, false), nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::jumpNotEquals(string args, string targetLabel) {
    return addInstruction(instruction::JE_COMMAND, args, make_pair(targetLabel, true), nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::jumpGreater(string args, string targetLabel) {
    return addInstruction(instruction::JG_COMMAND, args, make_pair(targetLabel, false), nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::jumpLess(string args, string targetLabel) {
    return addInstruction(instruction::JL_COMMAND, args, make_pair(targetLabel, false), nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::jumpGreaterEquals(string args, string targetLabel) {
    return addInstruction(instruction::JL_COMMAND, args, make_pair(targetLabel, true), nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::jumpLower(string args, string targetLabel) {
    return addInstruction(instruction::JL_COMMAND, args, make_pair(targetLabel, false), nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::jumpLowerEquals(string args, string targetLabel) {
    return addInstruction(instruction::JG_COMMAND, args, make_pair(targetLabel, true), nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::quit() {
    return addInstruction(instruction::QUIT_COMMAND, nullopt, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::random(std::string range, std::string storeTarget) {
    return addInstruction(instruction::RANDOM_COMMAND, range, nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::ret(string arg) {
    return addInstruction(instruction::RET_COMMAND, arg, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::newline() {
    return addInstruction(instruction::NEW_LINE_COMMAND, nullopt, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::print(string str) {
    return addInstruction(instruction::PRINT_COMMAND, string("\"") + str + string("\""), nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::setTextStyle(std::string values) {
    return addInstruction(instruction::SET_TEXT_STYLE, values, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::read_char(string storeTarget) {
    return addInstruction(instruction::READ_CHAR_COMMAND, string("1"), nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::println(string str) {
    return addInstruction(instruction::PRINT_COMMAND, string("\"") + str + string("\""), nullopt, nullopt).newline();
}

ZAssemblyGenerator &ZAssemblyGenerator::print_num(string source) {
    return addInstruction(instruction::PRINT_NUM_COMMAND, source, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::push(string value) {
    return addInstruction(instruction::PUSH_COMMAND, value, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::pop() {
    return addInstruction(instruction::POP_COMMAND, nullopt, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::variable(string variable) {
    return addInstruction(instruction::NOTHING, variable.substr(1), nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::load(std::string source, std::string target) {
    return addInstruction(instruction::LOAD_COMMAND, source, nullopt, target);
}

ZAssemblyGenerator &ZAssemblyGenerator::store(std::string target, std::string value) {
    // Do not change this!
    return addInstruction(instruction::STORE_COMMAND, target + " " + value, nullopt, nullopt);
}

ZAssemblyGenerator &ZAssemblyGenerator::add(std::string left, std::string right, std::string storeTarget) {
    return addInstruction(instruction::ADD_COMMAND, makeArgs({left, right}), nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::sub(std::string left, std::string right, std::string storeTarget) {
    return addInstruction(instruction::SUB_COMMAND, makeArgs({left, right}), nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::mul(std::string left, std::string right, std::string storeTarget) {
    return addInstruction(instruction::MUL_COMMAND, makeArgs({left, right}), nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::div(std::string left, std::string right, std::string storeTarget) {
    return addInstruction(instruction::DIV_COMMAND, makeArgs({left, right}), nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::mod(std::string left, std::string right, std::string storeTarget) {
    return addInstruction(instruction::MOD_COMMAND, left + " " + right, nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::land(std::string left, std::string right, std::string storeTarget) {
    return addInstruction(instruction::AND_COMMAND, makeArgs({left, right}), nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::lor(std::string left, std::string right, std::string storeTarget) {
    return addInstruction(instruction::OR_COMMAND, makeArgs({left, right}), nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::lnot(std::string variable, std::string storeTarget) {
    return addInstruction(instruction::NOT_COMMAND, variable, nullopt, storeTarget);
}

ZAssemblyGenerator &ZAssemblyGenerator::nop() {
    // TODO: better use the official nop instruction...
    return addInstruction(instruction::PUSH_COMMAND, string("0"), nullopt, nullopt)
            .addInstruction(instruction::POP_COMMAND, nullopt, nullopt, nullopt);
}

