//
// Created by manuel on 08.06.15.
//

#include "AssemblyParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <plog/Log.h>
#include <bitset>
#include <cstdint>

using namespace std;

const string AssemblyParser::ROUTINE_DIRECTIVE = ".FUNCT";
const string AssemblyParser::GVAR_DIRECTIVE = ".GVAR";

const string AssemblyParser::NEW_LINE_COMMAND = "new_line";
const string AssemblyParser::PRINT_COMMAND = "print";
const string AssemblyParser::JE_COMMAND = "je";
const string AssemblyParser::JUMP_GREATER_COMMAND = "jg";
const string AssemblyParser::JUMP_SMALLER_COMMAND = "jl";

const string AssemblyParser::QUIT_COMMAND = "quit";
const string AssemblyParser::READ_CHAR_COMMAND = "read_char";
const string AssemblyParser::CALL_COMMAND = "call";
const string AssemblyParser::JUMP_COMMAND = "jump";
const string AssemblyParser::RET_COMMAND = "ret";
const string AssemblyParser::SET_TEXT_STYLE = "set_text_style";
const string AssemblyParser::CALL_VS_COMMAND = "call_vs";
const string AssemblyParser::CALL_1N_COMMAND = "call_1n";

const char AssemblyParser::SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND = ' ';
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

void AssemblyParser::performRoutineDirectiveCommand( vector<string> lineComps, vector <bitset<8>> &highMemoryZcode,size_t offset)
{
    LOG_DEBUG << "found routine" ;

    if (lineComps.size() < 2) {
        cerr << "invalid routine declaration (no name specified)" ;
        throw;
    }
    string routineName = lineComps.at(1);

    // currentGenerator exists, so we can get its code
    if (currentGenerator) {
        finishRoutine(highMemoryZcode);
    }

    unsigned locVariablesCount = (unsigned) (lineComps.size() - 2);
    currentGenerator.reset(new RoutineGenerator(routineName, locVariablesCount, highMemoryZcode, offset));

    bool withoutComma = true;
    for (;locVariablesCount > 0; locVariablesCount--) {     // parse locale variables
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
            } catch (const invalid_argument& invaldArgument) {
                LOG_DEBUG << "Given value for local variable is not an integer: " << valueString ;
                throw;
            } catch (const out_of_range& outOfRange) {
                LOG_DEBUG << "Given value for local variable too large or too small: " << valueString ;
                throw;
            }

            if(val > INT16_MAX || val < INT16_MIN) {
                throw out_of_range(string("Given value for local variable too large or too small: ") + to_string(val) );
            }

            string name = var.substr(0, nameEnd);
            currentGenerator->setLocalVariable(name, val);
        } else {
            string name = var.substr(0, varEnd);
            currentGenerator->setLocalVariable(name);
        }
    }

}

void AssemblyParser::performRoutineGlobalVarCommand(string line) {
    LOG_DEBUG << "found gvar" ;
    vector<string> lineComps;

    this->split(line, SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND, lineComps);
    if (lineComps.size() < 2) {
        cerr << "empty gvar declaration" ;
    }

    string gvar = lineComps.at(1);

    addGlobal(gvar);
}



void AssemblyParser::readAssembly(istream& input, vector <bitset<8>> &highMemoryZcode,
                                  size_t offset) {

    LOG_DEBUG << "Compiler: Parse Assembly File\n";


    for(string line; getline(input, line);) {
        line = trim(line);
        vector<string> lineComps;
        this->split(line, SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND, lineComps);

        if (lineComps.size()) {
            string firstComp = lineComps.at(0);

            if (line.at(0) == '.') { // routine directive
                if (firstComp.compare(ROUTINE_DIRECTIVE) == 0) {
                    performRoutineDirectiveCommand(lineComps,highMemoryZcode,offset);
                } else if (firstComp.compare(GVAR_DIRECTIVE) == 0) { //global variable directive
                    performRoutineGlobalVarCommand(line);                }
            } else { // normal instruction
                executeCommand(line, *currentGenerator);
            }
        }
    }

    if (currentGenerator) {
        finishRoutine(highMemoryZcode);
    }
}

void AssemblyParser::finishRoutine(vector<bitset<8>> &highMemoryZcode) {
    LOG_DEBUG << "adding routine to zcode" ;
    auto routineCode = currentGenerator->getRoutine();
    Utils::append(highMemoryZcode, routineCode);
}


unique_ptr<ZParam> AssemblyParser::createZParam(const string& paramString) {
    unique_ptr<ZParam> param;
    int paramNum;

    try {
        paramNum = stoi(paramString);
        ZValueParam* valueParam = new ZValueParam((uint16_t) paramNum);
        param.reset(valueParam);

        return param;
    } catch(const invalid_argument& invalidArgument){

    }

    // not catching exception on purpose so we always return a proper value
    paramNum = getAddressForId(paramString);

    ZVariableParam* variableParam = new ZVariableParam((uint16_t) paramNum);
    param.reset(variableParam);

    return param;
}


void AssemblyParser::addGlobal(string globalName) {
    // TODO: check if maximum gvar limit is reached
    unsigned index = (unsigned) globalVariables.size();
    LOG_DEBUG << "adding gvar " << globalName << " at index " << to_string(index) ;
    this->globalVariables[globalName] = index;
}

void AssemblyParser::executePRINTCommand(const string &printCommand, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(printCommand, AssemblyParser::STRING_DELIMITER);
    routineGenerator.printString(commandParts.at(1));
    LOG_DEBUG << commandParts.at(1) ;
}

void AssemblyParser::executeSETTEXTSTYLECommand(const string &printCommand, RoutineGenerator &routineGenerator) {

    vector <string> commandParts = this->split(printCommand, AssemblyParser::SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND);
    bool bold = false, italic = false, underlined = false, roman = false;

    for(size_t i = 0; i < commandParts[1].size(); i++) {
        switch (commandParts[1][i]) {
            case 'b':
                bold = true;
                break;
            case 'i':
                italic = true;
                break;
            case 'u':
                underlined = true;
                break;
            case 'r':
                roman = true;
                break;
            default:
                cerr << "problem with set_text_style parameter parsing: " << printCommand ;
        }
        if(roman) {
            bold = false;
            italic = false;
            underlined = false;
            break;
        }
    }

    routineGenerator.setTextStyle( roman, false, bold, italic, underlined );
    LOG_DEBUG << commandParts.at(1) ;
}

void AssemblyParser::executeREADCommand(const string &readCommand, RoutineGenerator &routineGenerator) {
    vector<string> commandParts = this->split(readCommand, AssemblyParser::ASSIGNMENT_OPERATOR);
    if (commandParts.size() < 2) {
        // TODO: decide on whether to allow no args for read_char
        return;
    }
    auto last = trim(commandParts.back());
    routineGenerator.readChar(getAddressForId(last));
}

void AssemblyParser::executeJECommand(const string &jeCommand, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(jeCommand, '?');
    string label = commandParts.at(1);

    commandParts = split(jeCommand,' ');
    unique_ptr<ZParam> param1 = createZParam(commandParts.at(1));
    unique_ptr<ZParam> param2 = createZParam(commandParts.at(2));

    LOG_DEBUG << " "  << label ;
    routineGenerator.jumpEquals(label, true, *param1, *param2);
}

void AssemblyParser::executeJumpGreaterCommand(const string &command, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(command, '?');
    string label = commandParts.at(1);

    commandParts = split(command,' ');
    unique_ptr<ZParam> param1 = createZParam(commandParts.at(1));
    unique_ptr<ZParam> param2 = createZParam(commandParts.at(2));

    LOG_DEBUG << " "  << label ;
    routineGenerator.jumpGreaterThan(label, true, *param1, *param2);
}
void AssemblyParser::executeJumpSmallerCommand(const string &jumpSmallerCommand, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(jumpSmallerCommand, '?');
    string label = commandParts.at(1);

    commandParts = split(jumpSmallerCommand,' ');
    unique_ptr<ZParam> param1 = createZParam(commandParts.at(1));
    unique_ptr<ZParam> param2 = createZParam(commandParts.at(2));

    LOG_DEBUG << " "  << label ;
    routineGenerator.jumpLessThan(label, true, *param1, *param2);
}


void AssemblyParser::executeJUMPCommand(const string &jumpCommand, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(jumpCommand, '?');
    string label = commandParts.at(1);
    LOG_DEBUG << label ;
    routineGenerator.jump(label);
}

void AssemblyParser::executeCALLCommand(const string &callCommand, RoutineGenerator &routineGenerator) {
    static const unsigned MAX_PARAMS = 3;
    vector <string> commandParts = this->split(callCommand, ' ');
    string callRoutineName = commandParts.at(1);

    // TODO: what to do with a statement like "call_vs routine_name"?

    // TODO: extract so it can be used for other instructions
    // last part is storetarget
    string storeVarName = commandParts.back();
    auto storeId = getAddressForId(storeVarName);

    vector<unique_ptr<ZParam>> params(MAX_PARAMS);

    int paramsCount = 0;
    for(auto part = commandParts.begin() + 2; part != commandParts.end() - 2; part++) {
        params.push_back(createZParam(*part));
        paramsCount++;

        if(paramsCount > MAX_PARAMS) {
            cerr << "More than " << MAX_PARAMS << " params provided for instruction" ;
            throw;
        }
    }

    routineGenerator.callRoutine(callRoutineName, storeId, params.at(0).get(), params.at(1).get(), params.at(2).get());
}

void AssemblyParser::executeCALL1nCommand(const string &callCommand, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(callCommand, ' ');
    string callRoutineName = commandParts.at(1);
    LOG_DEBUG << callRoutineName ;

    routineGenerator.callRoutine1n(callRoutineName);
}


void AssemblyParser::executeRETCommand(const string &callCommand, RoutineGenerator &routineGenerator) {
    vector <string> commandParts = this->split(callCommand, ' ');
    string value = commandParts.at(1);

    routineGenerator.returnValue(*createZParam(value));

}


void AssemblyParser::executeCommand(const string &command, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(command,
                                              AssemblyParser::SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND);

    if (!commandParts.size()) return;

    string commandPart = commandParts.at(0);

    if (commandPart.compare(AssemblyParser::NEW_LINE_COMMAND) == 0) {
        routineGenerator.newLine();
        LOG_DEBUG << ":::::: new line" ;
    } else if (commandPart.compare(AssemblyParser::PRINT_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new print ";
        executePRINTCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::JE_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new je ";
        executeJECommand(command, routineGenerator);
    }else if (commandPart.compare(AssemblyParser::JUMP_GREATER_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new jump greater ";
        executeJumpGreaterCommand(command, routineGenerator);
    }else if (commandPart.compare(AssemblyParser::JUMP_SMALLER_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new jump smaller ";
        executeJumpSmallerCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::QUIT_COMMAND) == 0) {
        routineGenerator.quitRoutine();
        LOG_DEBUG << ":::::: new quit" ;
    } else if (commandPart.compare(AssemblyParser::READ_CHAR_COMMAND) == 0) {
        executeREADCommand(command, routineGenerator);
        LOG_DEBUG << ":::::: new read" ;
    } else if (commandPart.compare(AssemblyParser::CALL_VS_COMMAND) == 0
               || commandPart.compare(AssemblyParser::CALL_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new call ";
        executeCALLCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::CALL_1N_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new call_1n";
        executeCALL1nCommand(command, routineGenerator);
    } else if(commandPart.compare(AssemblyParser::JUMP_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new jump ";
        executeJUMPCommand(command, routineGenerator);
    } else if(commandPart.compare(AssemblyParser::RET_COMMAND) == 0) {
        LOG_DEBUG << ":::::: new return routine ";
        executeRETCommand(command, routineGenerator);
    } else if (commandPart.at(commandPart.size() - 1) == ':') {
        string label = commandPart.substr(0, commandPart.size() - 1);
        LOG_DEBUG << ":::::: new label: " << label ;
        routineGenerator.newLabel(label);

        string afterLabel = command.substr(commandPart.size());
        executeCommand(trim(afterLabel), *currentGenerator);
    } else if(commandPart.compare(AssemblyParser::SET_TEXT_STYLE) == 0) {
        LOG_DEBUG << ":::::: new set_text_style ";
        executeSETTEXTSTYLECommand(command, routineGenerator);
    } else {
        LOG_DEBUG << "unknown command: " << command ;
        throw;
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


uint8_t AssemblyParser::getAddressForId(const string &id) {
    if (id.compare("sp") == 0) {
        return 0;
    }

    // check global variables
    if (globalVariables.count(id)) {
        return (uint8_t) (globalVariables[id] + 0x10);
    } else if (currentGenerator->containsLocalVariable(id)) {
        return currentGenerator->getAddressOfVariable(id);
    } else {
        LOG_DEBUG << "Could not find variable " << id ;
        throw;
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
        LOG_DEBUG << "found token " << token ;
        tokens.push_back(token);
        s_copy.erase(0, pos + delim.length());
    }
    tokens.push_back(s_copy);

    return tokens;
}


