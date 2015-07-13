//
// Created by manuel on 08.06.15.
//

#include "AssemblyParser.h"
#include "exceptions.h"
#include "ZCodeObjects/ZCodePkgAdrrPadding.h"
#include "ZCodeObjects/ZCodeCallAdress.h"
#include "ZCodeObjects/ZCodeMemorySpace.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <plog/Log.h>
#include <bitset>
#include <cstdint>
#include "optional.hpp"

using namespace std;
using namespace std::experimental;

const string AssemblyParser::ROUTINE_DIRECTIVE = ".FUNCT";
const string AssemblyParser::GVAR_DIRECTIVE = ".GVAR";
const string AssemblyParser::BYTEARRAY = ".BYTEARRAY";
const string AssemblyParser::WORDARRAY = ".WORDARRAY";

const string AssemblyParser::NEW_LINE_COMMAND = "new_line";
const string AssemblyParser::PRINT_COMMAND = "print";
const string AssemblyParser::JE_COMMAND = "je";
const string AssemblyParser::JG_COMMAND = "jg";
const string AssemblyParser::JL_COMMAND = "jl";
const string AssemblyParser::JZ_COMMAND = "jz";
const string AssemblyParser::QUIT_COMMAND = "quit";
const string AssemblyParser::READ_CHAR_COMMAND = "read_char";
const string AssemblyParser::PRINT_CHAR_COMMAND = "print_char";
const string AssemblyParser::PRINT_NUM_COMMAND = "print_num";
const string AssemblyParser::PRINT_ADDR_COMMAND = "print_addr";
const string AssemblyParser::JUMP_COMMAND = "jump";
const string AssemblyParser::RET_COMMAND = "ret";
const string AssemblyParser::SET_TEXT_STYLE = "set_text_style";
const string AssemblyParser::CALL_VS_COMMAND = "call_vs";
const string AssemblyParser::CALL_1N_COMMAND = "call_1n";
const string AssemblyParser::CALL_VN_COMMAND = "call_vn";
const string AssemblyParser::STORE_COMMAND = "store";
const string AssemblyParser::LOAD_COMMAND = "load";
const string AssemblyParser::ADD_COMMAND = "add";
const string AssemblyParser::SUB_COMMAND = "sub";
const string AssemblyParser::MUL_COMMAND = "mul";
const string AssemblyParser::DIV_COMMAND = "div";
const string AssemblyParser::MOD_COMMAND = "mod";
const string AssemblyParser::AND_COMMAND = "and";
const string AssemblyParser::OR_COMMAND = "or";
const string AssemblyParser::NOT_COMMAND = "not";
const string AssemblyParser::RET_TRUE_COMMAND = "rtrue";
const string AssemblyParser::RET_FALSE_COMMAND = "rfalse";
const string AssemblyParser::PRINT_RET_COMMAND = "print_ret";
const string AssemblyParser::RESTART_COMMAND = "restart";
const string AssemblyParser::RET_POPPED_COMMAND = "ret_popped";
const string AssemblyParser::POP_COMMAND = "pop";
const string AssemblyParser::VERIFY_COMMAND = "verify";
const string AssemblyParser::STOREB_COMMAND = "storeb";
const string AssemblyParser::STOREW_COMMAND = "storew";
const string AssemblyParser::LOADB_COMMAND = "loadb";
const string AssemblyParser::LOADW_COMMAND = "loadw";
const string AssemblyParser::PUSH_COMMAND = "push";
const string AssemblyParser::PULL_COMMAND = "pull";
const string AssemblyParser::RANDOM_COMMAND = "random";
const string AssemblyParser::READ_MOUSE = "read_mouse";
const string AssemblyParser::MOUSE_WINDOW = "mouse_window";
const string AssemblyParser::GET_CURSOR = "get_cursor";
const string AssemblyParser::SET_CURSOR = "set_cursor";
const string AssemblyParser::GET_WINDOW_PROPERTY = "get_wind_prop";
const string AssemblyParser::WINDOW_STYLE = "window_style";
const string AssemblyParser::SCROLL_WINDOW = "scroll_window";
const string AssemblyParser::PUT_WIND_PROP = "put_wind_prop";
const string AssemblyParser::SET_WINDOW = "set_window";

const char AssemblyParser::SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND = ' ';
const char AssemblyParser::STRING_DELIMITER = '\"';
const string AssemblyParser::ASSIGNMENT_OPERATOR = "->";


string trim(const string &str,
            const string &whitespace = " \t") {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

void AssemblyParser::performRoutineDirectiveCommand(vector<string> lineComps, shared_ptr<ZCodeContainer> highMemory) {
    LOG_DEBUG << "found routine";

    if (lineComps.size() < 2) {
        cerr << "invalid routine declaration (no name specified)";
        throw AssemblyException(AssemblyException::ErrorType::INVALID_ROUTINE);
    }
    string routineName = lineComps.at(1);

    // currentGenerator exists, so we can get its code
    if (currentGenerator) {
        finishRoutine(highMemory);
    }

    // have to be cleared after each routine
    registeredJumpsAtLines = vector<pair<string, unsigned>>();
    registeredLabels = vector<string>();

    unsigned locVariablesCount = (unsigned) (lineComps.size() - 2);
    currentGenerator.reset(new RoutineGenerator(routineName, locVariablesCount));

    vector<pair<string, optional<int>>> locals;
    bool withoutComma = true;
    for (; locVariablesCount > 0; locVariablesCount--) {     // parse locale variables
        string var = lineComps[locVariablesCount + 1];

        size_t nameEnd = var.find_first_of("=");
        size_t varEnd = var.size();

        if (withoutComma) {         // last locale variable has no comma as last char
            withoutComma = false;
        } else {
            varEnd -= 1;
        }

        if (nameEnd != string::npos) {
            int val;
            string valueString = var.substr(nameEnd + 1, varEnd - 1 - nameEnd);

            try {
                val = stoi(valueString);
            } catch (const invalid_argument &invaldArgument) {
                LOG_ERROR << "Given value for local variable is not an integer: " << valueString;
                throw AssemblyException(AssemblyException::ErrorType::INVALID_INSTRUCTION);
            } catch (const out_of_range &outOfRange) {
                LOG_ERROR << "Given value for local variable too large or too small: " << valueString;
                throw AssemblyException(AssemblyException::ErrorType::INVALID_INSTRUCTION);
            }

            if (val > INT16_MAX || val < INT16_MIN) {
                LOG_ERROR << "Given value for local variable too large or too small: " << to_string(val);
                throw AssemblyException(AssemblyException::ErrorType::INVALID_INSTRUCTION);
            }

            string name = var.substr(0, nameEnd);
            locals.push_back(make_pair(name, val));
        } else {
            string name = var.substr(0, varEnd);
            locals.push_back(make_pair(name, nullopt));
        }
    }

    for(auto localPair = locals.rbegin(); localPair != locals.rend(); ++localPair) {
        if(localPair->second) {
            currentGenerator->setLocalVariable(localPair->first, *(localPair->second));
        } else {
            currentGenerator->setLocalVariable(localPair->first);
        }
    }
}

void AssemblyParser::performRoutineGlobalVarCommand(string line) {
    LOG_DEBUG << "found gvar";
    vector<string> lineComps;

    this->split(line, SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND, lineComps);
    if (lineComps.size() < 2) {
        cerr << "empty gvar declaration";
    }

    string gvar = lineComps.at(1);

    addGlobal(gvar);
}


void AssemblyParser::readAssembly(istream &input, shared_ptr<ZCodeContainer> dynamicMemory,
                                  shared_ptr<ZCodeContainer> staticMemory, shared_ptr<ZCodeContainer> highMemory) {

    LOG_DEBUG << "Compiler: Parse Assembly File\n";

    string line;
    currentLineNumber = 1;
    try {
        for (;getline(input, line);) {

            line = trim(line);
            vector<string> lineComps;
            this->split(line, SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND, lineComps);

            if (lineComps.size()) {
                string firstComp = lineComps.at(0);

                if (line.at(0) == '.') { // routine directive
                    if (firstComp.compare(ROUTINE_DIRECTIVE) == 0) {
                        performRoutineDirectiveCommand(lineComps, highMemory);
                    } else if (firstComp.compare(GVAR_DIRECTIVE) == 0) { //global variable directive
                        performRoutineGlobalVarCommand(line);
                    } else if (firstComp.compare(BYTEARRAY) == 0) { //Bytearray
                        performByteArrayDirective(line, dynamicMemory);
                    } else if (firstComp.compare(WORDARRAY) == 0) { //wordarray
                        performWordArrayDirective(line, dynamicMemory);
                    } else {
                        cerr << "unknown directive";
                        throw;
                    }
                } else {
                    executeCommand(line, *currentGenerator, dynamicMemory);
                }

                ++currentLineNumber;
            }
        }
        if (currentGenerator) {
            finishRoutine(highMemory);
        }
    } catch (AssemblyException &assemblyException) {
        // only set if not known already
        if (assemblyException.lineNumber == 0) {
            assemblyException.lineNumber = currentLineNumber;
            assemblyException.line = line;
        }
        throw;
    }

}

void AssemblyParser::finishRoutine(shared_ptr<ZCodeContainer> highMemoryZcode) {
    LOG_DEBUG << "adding routine to zcode";
    auto routine = currentGenerator->getRoutine();
    // check if all labels were valid
    bool labelFound;
    for (auto jump = registeredJumpsAtLines.begin(); jump != registeredJumpsAtLines.end(); ++jump) {
        labelFound = false;
        string jumpFirst = jump->first;

        for (auto label = registeredLabels.begin(); label != registeredLabels.end(); ++label) {
            if (label->compare(jumpFirst) == 0) {
                labelFound = true;
                break;
            }
            //set labelFound true when label is x but jump->first is ~x for jump not equals
            string jumpSubString = jumpFirst.substr(1, jumpFirst.size()-1);
            bool beginsWithTilde = (jumpFirst.at(0) == '~' );
            bool restIsSame = (label->compare( jumpSubString) == 0 ) ;
            if ( beginsWithTilde && restIsSame ) {
                labelFound = true;
                break;
            }
        }

        if (!labelFound) {
            InvalidLabelException e(jump->first);
            e.lineNumber = jump->second;
            throw e;
        }
    }

    if (firstRoutine) {
        //Call to first Routine
        auto call = shared_ptr<ZCodeObject>(new ZCodeInstruction(RoutineGenerator::CALL_1N, "call to first routine"));
        highMemoryZcode->add(call);
        auto adress = shared_ptr<ZCodeObject>(new ZCodeCallAdress(routine));
        highMemoryZcode->add(adress);
        firstRoutine = false;
    }
    auto padding = shared_ptr<ZCodeObject>(new ZCodePkgAdrrPadding());
    highMemoryZcode->add(padding);
    highMemoryZcode->add(routine);
}

vector<unique_ptr<ZParam>> AssemblyParser::parseArguments(const string instruction) {
    vector<string> commandParts = this->split(instruction, AssemblyParser::SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND);
    vector<unique_ptr<ZParam>> params;

    if (commandParts.size() < 2) {  // first argument is opcode instruction
        return params;
    } else if ((count(instruction.begin(), instruction.end(), AssemblyParser::STRING_DELIMITER) % 2) != 0) {
        // checks if there are always 2 quotation marks for hard coded string
        LOG_ERROR << "String does not end!";
        throw AssemblyException(AssemblyException::ErrorType::INVALID_INSTRUCTION);
    }

    // if this is a call instruction first argument is routine name
    bool isCallInstruction = false;
    if (instruction.size() > 2 && instruction.at(0) == 'c' && instruction.at(1) == 'a'
        && instruction.at(2) == 'l' && instruction.at(3) == 'l') {

        isCallInstruction = true;
        params.push_back(unique_ptr<ZNameParam>(new ZNameParam(commandParts.at(1))));
    }

    int paramsCount;   // count of parameters to parse (wihthout first instruction and store address parts)
    bool containsStoreAddress = false, containsLabel = false;
    if (instruction.find(AssemblyParser::ASSIGNMENT_OPERATOR) !=
        string::npos) {   // contains instruction store address?
        paramsCount = commandParts.size() - 3;  // - instruction - "->" - storeAddress
        containsStoreAddress = true;
    } else {
        paramsCount = commandParts.size() - 1;  // - instruction
    }

    int start = 1;
    int parsedCommandChars = commandParts.at(0).size() + 1;
    if (isCallInstruction) {
        start += 1;
        parsedCommandChars += commandParts.at(1).size() + 1;
    }

    int stringEndChar = 0;  // position of last char in 'instruction' of parsed string parameter
    for (auto part = commandParts.begin() + start; part != commandParts.begin() + paramsCount + 1; ++part) {
        string paramString = *part;

        // skip commandParts if they are part of a string parameter
        if (parsedCommandChars < stringEndChar) {
            parsedCommandChars += (paramString.size() + 1);
            continue;
        }

        // is this commandPart start of a string parameter?
        if (paramString.at(0) == AssemblyParser::STRING_DELIMITER) {
            int stringStartChar = parsedCommandChars;
            stringEndChar = parsedCommandChars;

            // get index of next quotation mark
            while (++stringEndChar != (instruction.size() - 1)) {
                if (instruction.at(stringEndChar) == AssemblyParser::STRING_DELIMITER) {
                    break;
                }
            }

            string stringParam = instruction.substr(stringStartChar + 1, stringEndChar - stringStartChar - 1);
            params.push_back(unique_ptr<ZNameParam>(new ZNameParam(stringParam)));

            stringEndChar += 2;     // point to first char of next commandPart
            if (stringEndChar >=
                instruction.size()) {  // break if there is no next argument after this string parameter
                break;
            }
            parsedCommandChars += paramString.size() + 1;
            continue;
        }

        // label parameter?
        if (paramString.at(0) == '?') {
            containsLabel = true;
            break;
        }

        params.push_back(createZParam(paramString));
        parsedCommandChars += paramString.size() + 1;
    }

    if (containsStoreAddress) {
        string storeAddress = commandParts.at(commandParts.size() - 1);
        auto address = getAddressForId(storeAddress);
        if (address) {
            params.push_back(unique_ptr<ZStoreParam>(new ZStoreParam((uint16_t) *address)));
        } else {
            LOG_ERROR << "Unknown store address: " << storeAddress;
            throw AssemblyException(AssemblyException::ErrorType::INVALID_INSTRUCTION);
        }
    } else if (containsLabel) {
        auto label = split(instruction, '?').at(1);
        params.push_back(unique_ptr<ZNameParam>(new ZNameParam(label)));
    }

    return params;
}

unique_ptr<ZParam> AssemblyParser::createZParam(const string &paramString) {
    unique_ptr<ZParam> param;

    try {
        int paramNum = stoi(paramString);
        ZValueParam *valueParam = new ZValueParam((uint16_t) paramNum);
        param.reset(valueParam);

        return param;
    } catch (const invalid_argument &invalidArgument) {

    }

    // not catching exception on purpose so we always return a proper value
    auto paramId = getAddressForId(paramString);

    if (paramId != NULL) {
        ZVariableParam *variableParam = new ZVariableParam((uint16_t) *paramId);
        param.reset(variableParam);
    } else {
        LOG_ERROR << "Could not parse parameter: " << paramString;
        throw AssemblyException(AssemblyException::ErrorType::INVALID_VARIABLE);
    }


    return param;
}


void AssemblyParser::addGlobal(string globalName) {
    // TODO: check if maximum gvar limit is reached
    unsigned index = (unsigned) globalVariables.size();
    if (globalVariables.find(globalName) != globalVariables.end()) {
        LOG_ERROR << "two global variable have the same name";
        throw AssemblyException(AssemblyException::ErrorType::INVALID_GLOBAL);
    }
    LOG_DEBUG << "adding gvar " << globalName << " at index " << to_string(index);
    this->globalVariables[globalName] = index;
}

void AssemblyParser::registerJump(const vector<unique_ptr<ZParam>> &params) {
    for (auto it = params.rbegin(); it != params.rend(); it++) {
        ZParam *param = it->get();
        if (param->getParamType() == NAME) {
            ZNameParam *nameParam = dynamic_cast<ZNameParam *>(param);
            registeredJumpsAtLines.push_back(make_pair(nameParam->name, currentLineNumber));
            return;
        }
    }
}

void AssemblyParser::executePRINTCommand(const string &printCommand, RoutineGenerator &routineGenerator) {
    routineGenerator.printString(parseArguments(printCommand));
}

void AssemblyParser::executeREADCommand(const string &readCommand, RoutineGenerator &routineGenerator) {
    routineGenerator.readChar(parseArguments(readCommand));
}

void AssemblyParser::executeJECommand(const string &jeCommand, RoutineGenerator &routineGenerator) {
    auto args = parseArguments(jeCommand);
    registerJump(args);
    routineGenerator.jumpEquals(move(args));
}

void AssemblyParser::executeJGCommand(const string &jeCommand, RoutineGenerator &routineGenerator) {
    auto args = parseArguments(jeCommand);
    registerJump(args);
    routineGenerator.jumpGreaterThan(move(args));
}

void AssemblyParser::executeJLCommand(const string &jeCommand, RoutineGenerator &routineGenerator) {
    auto args = parseArguments(jeCommand);
    registerJump(args);
    routineGenerator.jumpLessThan(move(args));
}

void AssemblyParser::executeJUMPCommand(const string &jumpCommand, RoutineGenerator &routineGenerator) {
    auto args = parseArguments(jumpCommand);
    registerJump(args);
    routineGenerator.jump(move(args));
}

void AssemblyParser::executeJZCommand(const string &jumpCommand, RoutineGenerator &routineGenerator) {
    auto args = parseArguments(jumpCommand);
    registerJump(args);
    routineGenerator.jumpZero(move(args));
}

void AssemblyParser::executeCALL_VSCommand(const string &callCommand, RoutineGenerator &routineGenerator) {
    routineGenerator.callVS(parseArguments(callCommand));
}

void AssemblyParser::executeCALL1nCommand(const string &callCommand, RoutineGenerator &routineGenerator) {
    routineGenerator.call1n(parseArguments(callCommand));
}

void AssemblyParser::executeRETCommand(const string &callCommand, RoutineGenerator &routineGenerator) {
    routineGenerator.returnValue(parseArguments(callCommand));
}

void AssemblyParser::executeCommand(const string &command, RoutineGenerator &routineGenerator,
                                    shared_ptr<ZCodeContainer> dynamicMemory) {
    vector<string> commandParts = this->split(command, AssemblyParser::SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND);

    if (!commandParts.size()) return;

    string commandPart = commandParts.at(0);

    if (commandPart.compare("MOUSE_POS") == 0) {
        vector<unique_ptr<ZParam>> params;
        params.push_back(unique_ptr<ZParam>(new ZNameParam("HeaderExtTable")));
        params.push_back(unique_ptr<ZParam>(new ZValueParam(1)));
        params.push_back(unique_ptr<ZParam>(new ZVariableParam(0))); // sp
        routineGenerator.loadw(params, dynamicMemory);

        vector<unique_ptr<ZParam>> params2;
        params2.push_back(unique_ptr<ZParam>(new ZVariableParam(0)));
        routineGenerator.printNum(move(params2));

        routineGenerator.newLine();

        vector<unique_ptr<ZParam>> params3;
        params3.push_back(unique_ptr<ZParam>(new ZNameParam("HeaderExtTable")));
        params3.push_back(unique_ptr<ZParam>(new ZValueParam(2)));
        params3.push_back(unique_ptr<ZParam>(new ZVariableParam(0))); // sp
        routineGenerator.loadw(params3, dynamicMemory);

        vector<unique_ptr<ZParam>> params4;
        params4.push_back(unique_ptr<ZParam>(new ZVariableParam(0)));
        routineGenerator.printNum(move(params4));
    } else if (commandPart.compare(AssemblyParser::NEW_LINE_COMMAND) == 0) {
        routineGenerator.newLine();
        LOG_DEBUG << ":::::: new line";
    } else if (commandPart.compare(AssemblyParser::PRINT_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new print ";
        executePRINTCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::JE_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new je ";
        executeJECommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::JG_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new jg ";
        executeJGCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::JL_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new jl ";
        executeJLCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::JZ_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new jz ";
        executeJZCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::QUIT_COMMAND) == 0) {
        routineGenerator.quitRoutine();
        LOG_DEBUG << ":::::: new quit";
    } else if (commandPart.compare(AssemblyParser::READ_CHAR_COMMAND) == 0) {
        executeREADCommand(command, routineGenerator);
        LOG_DEBUG << ":::::: new read";
    } else if (commandPart.compare(AssemblyParser::PRINT_CHAR_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new print_char";
        routineGenerator.printChar(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::PRINT_NUM_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new print_num";
        routineGenerator.printNum(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::PRINT_ADDR_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new print_addr";
        routineGenerator.printAddress(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::CALL_VS_COMMAND) == 0
               || (commandPart.compare("call") ==
                   0)) {                                // TODO: Generate specific call instruction
        LOG_DEBUG << ":::::: new call_vs ";
        executeCALL_VSCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::CALL_VN_COMMAND) == 0) {
        routineGenerator.callVN(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::CALL_1N_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new call_1n";
        executeCALL1nCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::JUMP_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new jump ";
        executeJUMPCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::RET_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new return routine ";
        executeRETCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::STORE_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new store";
        routineGenerator.store(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::LOAD_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new load";
        routineGenerator.load(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::STOREB_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new storeb";
        executeSTOREBCOMMAND(command, dynamicMemory, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::STOREW_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new storew";
        executeSTOREWCOMMAND(command, dynamicMemory, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::LOADB_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new loadb";
        executeLOADBCOMMAND(command, dynamicMemory, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::LOADW_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new loadw";
        executeLOADWCOMMAND(command, dynamicMemory, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::SET_TEXT_STYLE) == 0) {
        LOG_DEBUG << ":::::: new set_text_style ";
        routineGenerator.setTextStyle(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::ADD_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new add ";
        routineGenerator.add(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::SUB_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new sub ";
        routineGenerator.sub(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::MUL_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new mul ";
        routineGenerator.mul(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::DIV_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new div ";
        routineGenerator.div(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::MOD_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new mod ";
        routineGenerator.mod(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::AND_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new and ";
        routineGenerator.doAND(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::OR_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new or ";
        routineGenerator.doOR(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::NOT_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new not ";
        routineGenerator.doNOT(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::RET_TRUE_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new rtrue";
        routineGenerator.returnTrue();
    } else if (commandPart.compare(AssemblyParser::RET_FALSE_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new rfalse";
        routineGenerator.returnFalse();
    } else if (commandPart.compare(AssemblyParser::PRINT_RET_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new print_ret";
        routineGenerator.printRet(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::RESTART_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new restart";
        routineGenerator.restart();
    } else if (commandPart.compare(AssemblyParser::RET_POPPED_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new ret_popped";
        routineGenerator.retPopped();
    } else if (commandPart.compare(AssemblyParser::POP_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new pop";
        routineGenerator.pop();
    }else if (commandPart.compare(AssemblyParser::VERIFY_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new verify";
        routineGenerator.verify(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::PUSH_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new push";
        routineGenerator.push(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::PULL_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new pull";
        routineGenerator.pull(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::RANDOM_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new random";
        routineGenerator.random(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::READ_MOUSE) == 0) {
        LOG_DEBUG << ":::::: new read_mouse";
        vector<string> params = split(command, SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND);
        routineGenerator.readMouse(params.at(1), dynamicMemory);
    } else if (commandPart.compare(AssemblyParser::MOUSE_WINDOW) == 0) {
        LOG_DEBUG << ":::::: new mouse_window";
        routineGenerator.mouseWindow(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::GET_CURSOR) == 0) {
        LOG_DEBUG << ":::::: new get_cursor";
        vector<string> params = split(command, SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND);
        routineGenerator.getCursor(params.at(1), dynamicMemory);
    } else if (commandPart.compare(AssemblyParser::SET_CURSOR) == 0) {
        LOG_DEBUG << ":::::: new set_cursor";
        routineGenerator.setCursor(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::GET_WINDOW_PROPERTY) == 0) {
        LOG_DEBUG << ":::::: new get_window_property";
        routineGenerator.getWindowProperty(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::WINDOW_STYLE) == 0) {
        LOG_DEBUG << ":::::: new window_style";
        routineGenerator.windowStyle(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::SCROLL_WINDOW) == 0) {
        LOG_DEBUG << ":::::: new scroll_window";
        routineGenerator.scrollWindow(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::PUT_WIND_PROP) == 0) {
        LOG_DEBUG << ":::::: new put_wind_prop";
        routineGenerator.putWindProp(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::SET_WINDOW) == 0) {
        LOG_DEBUG << ":::::: new set_window";
        routineGenerator.setWindow(parseArguments(command));
    } else if (commandPart.at(commandPart.size() - 1) == ':') {
        string label = commandPart.substr(0, commandPart.size() - 1);
        LOG_DEBUG << ":::::: new label: " << label;
        routineGenerator.newLabel(label);
        registeredLabels.push_back(label);

        string afterLabel = command.substr(commandPart.size());
        executeCommand(trim(afterLabel), *currentGenerator, dynamicMemory);
    } else {
        LOG_DEBUG << "unknown command: " << command;
        throw AssemblyException(AssemblyException::ErrorType::INVALID_INSTRUCTION);
    }
}

bool AssemblyParser::checkIfCommandRoutineStart(const string &command) {
    vector<string> commandParts = this->split(command, ' ');
    for (unsigned int i = 0; i < commandParts.size(); i++) {
        if (commandParts.at(i).compare(ROUTINE_DIRECTIVE) == 0) {
            return true;
        }

    }
    return false;
}


unique_ptr<uint8_t> AssemblyParser::getAddressForId(const string &id) {
    if (id == "sp") {
        return unique_ptr<uint8_t>(new uint8_t(0));
    }

    // check global variables
    if (globalVariables.count(id)) {
        return unique_ptr<uint8_t>(new uint8_t(((uint8_t) (globalVariables[id] + 0x10))));
    } else if (currentGenerator->containsLocalVariable(id)) {
        return unique_ptr<uint8_t>(new uint8_t(currentGenerator->getAddressOfVariable(id)));
    } else {
        return unique_ptr<uint8_t>();
    }
}

vector<string> &AssemblyParser::split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<string> AssemblyParser::split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

vector<string> AssemblyParser::split(const string &s, const string &delim) {
    string s_copy = s;
    size_t pos = 0;
    string token;
    vector<string> tokens;
    while ((pos = s_copy.find(delim)) != string::npos) {
        token = s_copy.substr(0, pos);
        LOG_DEBUG << "found token " << token;
        tokens.push_back(token);
        s_copy.erase(0, pos + delim.length());
    }
    tokens.push_back(s_copy);

    return tokens;
}


void AssemblyParser::performByteArrayDirective(string command, shared_ptr<ZCodeContainer> dynamicMemory) {
    try {
        vector<string> param = this->split(command, ' ');
        string name = param.at(1);
        string sSize = param.at(2).substr(1, param.at(2).size() - 2);
        int size = std::stoi(sSize.c_str());
        auto initialSize = shared_ptr<ZCodeObject>(new ZCodeInstruction(size));
        auto table = shared_ptr<ZCodeObject>(new ZCodeMemorySpace(size, "ARRAY : " + name));
        auto label = dynamicMemory->getOrCreateLabel(name);

        dynamicMemory->add(label);
        dynamicMemory->add(initialSize);
        dynamicMemory->add(table);
    } catch (std::invalid_argument) {
        LOG_ERROR << "'.BYTEARRAY <name> [<size>]' expected. '" << command << "' found instead";
        throw AssemblyException(AssemblyException::ErrorType::INVALID_DIRECTIVE);
    } catch (std::out_of_range) {
        LOG_ERROR << "'.BYTEARRAY <name> [<size>]' expected. '" << command << "' found instead";
        throw AssemblyException(AssemblyException::ErrorType::INVALID_DIRECTIVE);
    }
}

void AssemblyParser::performWordArrayDirective(string command, shared_ptr<ZCodeContainer> dynamicMemory) {
    try {
        vector<string> param = this->split(command, ' ');
        string name = param.at(1);
        string sSize = param.at(2).substr(1, param.at(2).size() - 2);
        int size = std::stoi(sSize.c_str());
        auto label = dynamicMemory->getOrCreateLabel(name);
        dynamicMemory->add(label);
        auto table = shared_ptr<ZCodeObject>(new ZCodeMemorySpace(size * 2, "ARRAY : " + name));
        dynamicMemory->add(table);
    } catch (std::invalid_argument) {
        LOG_ERROR << "'.WORDARRAY <name> [<size>]' expected. '" << command << "' found instead";
        throw AssemblyException(AssemblyException::ErrorType::INVALID_DIRECTIVE);
    } catch (std::out_of_range) {
        LOG_ERROR << "'.WORDARRAY <name> [<size>]' expected. '" << command << "' found instead";
        throw AssemblyException(AssemblyException::ErrorType::INVALID_DIRECTIVE);
    }
}

void AssemblyParser::executeSTOREBCOMMAND(const string &command, shared_ptr<ZCodeContainer> dynamicMemory,
                                          RoutineGenerator &currentGenerator) {
    vector<string> sparam = split(command, SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND);
    vector<unique_ptr<ZParam>> params;
    params.push_back(unique_ptr<ZParam>(new ZNameParam(sparam.at(1))));
    params.push_back((createZParam(sparam.at(2))));
    params.push_back(createZParam(sparam.at(3)));
    currentGenerator.storeb(params, dynamicMemory);
}

void AssemblyParser::executeSTOREWCOMMAND(const string &command, shared_ptr<ZCodeContainer> dynamicMemory,
                                          RoutineGenerator &currentGenerator) {
    vector<string> sparam = split(command, SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND);
    vector<unique_ptr<ZParam>> params;
    params.push_back(unique_ptr<ZParam>(new ZNameParam(sparam.at(1))));
    params.push_back(createZParam(sparam.at(2)));
    params.push_back(createZParam(sparam.at(3)));
    currentGenerator.storew(params, dynamicMemory);
}

void AssemblyParser::executeLOADBCOMMAND(const string &command, shared_ptr<ZCodeContainer> dynamicMemory,
                                         RoutineGenerator &currentGenerator) {
    vector<string> parts = split(command, ASSIGNMENT_OPERATOR);
    vector<string> sparam = split(parts.at(0), SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND);
    vector<unique_ptr<ZParam>> params;

    try {
        int byteAddress = stoi(sparam.at(1));
        params.push_back(unique_ptr<ZParam>(new ZValueParam(byteAddress)));
        params.push_back(createZParam(trim(sparam.at(2), " ")));
        string s = parts.at(1);
        params.push_back(createZParam(trim(s, " ")));
        currentGenerator.loadb(params);
        return;
    } catch (const invalid_argument &invalidArgument) {

    }

    params.push_back(unique_ptr<ZParam>(new ZNameParam(sparam.at(1))));
    params.push_back(createZParam(trim(sparam.at(2), " ")));
    string s = parts.at(1);
    params.push_back(createZParam(trim(s, " ")));
    currentGenerator.loadb(params, dynamicMemory);
}

void AssemblyParser::executeLOADWCOMMAND(const string &command, shared_ptr<ZCodeContainer> dynamicMemory,
                                         RoutineGenerator &currentGenerator) {
    vector<string> parts = split(command, ASSIGNMENT_OPERATOR);
    vector<string> sparam = split(parts.at(0), SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND);
    vector<unique_ptr<ZParam>> params;
    params.push_back(unique_ptr<ZParam>(new ZNameParam(sparam.at(1))));
    params.push_back(createZParam(trim(sparam.at(2), " ")));
    string s = parts.at(1);
    params.push_back(createZParam(trim(s, " ")));
    currentGenerator.loadw(params, dynamicMemory);
}


