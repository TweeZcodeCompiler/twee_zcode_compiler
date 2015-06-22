//
// Created by manuel on 08.06.15.
//

#include "AssemblyParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <bitset>
#include <cstdint>
#include <plog/Log.h>

using namespace std;

const string AssemblyParser::ROUTINE_DIRECTIVE = ".FUNCT";
const string AssemblyParser::GVAR_DIRECTIVE = ".GVAR";

const string AssemblyParser::NEW_LINE_COMMAND = "new_line";
const string AssemblyParser::PRINT_COMMAND = "print";
const string AssemblyParser::JE_COMMAND = "je";
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

bool AssemblyParser::checkIfRoutineNameExists(std::string routineName)
{
    return std::find(routineNameList.begin(), routineNameList.end(), routineName) != routineNameList.end());
}


void AssemblyParser::readAssembly(istream& input, vector <bitset<8>> &highMemoryZcode,
                                  size_t offset) {

    cout << "Compiler: Parse Assembly File\n";


    for(string line; getline(input, line);) {
        line = trim(line);
        vector<string> lineComps;
        this->split(line, SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND, lineComps);
        if (lineComps.size()) {
            string firstComp = lineComps.at(0);

            if (line.at(0) == '.') { // directive
                if (firstComp.compare(ROUTINE_DIRECTIVE) == 0) {
                    cout << "found routine" << endl;

                    if (lineComps.size() < 2) {
                        cerr << "invalid routine declaration (no name specified)" << endl;
                        throw;
                    }
                    string routineName = lineComps.at(1);

                    if(checkIfRoutineNameExists(routineName)) {
                        LOG_ERROR << "two routines have the same name";
                    }
                    routineNameList.push_back(routineName);

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
                                cout << "Given value for local variable is not an integer: " << valueString << endl;
                                throw;
                            } catch (const out_of_range& outOfRange) {
                                cout << "Given value for local variable too large or too small: " << valueString << endl;
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
                } else if (firstComp.compare(GVAR_DIRECTIVE) == 0) {
                    cout << "found gvar" << endl;

                    this->split(line, SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND, lineComps);
                    if (lineComps.size() < 2) {
                        cerr << "empty gvar declaration" << endl;
                    }

                    string gvar = lineComps.at(1);

                    addGlobal(gvar);
                }
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
    cout << "adding routine to zcode" << endl;
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
    cout << "adding gvar " << globalName << " at index " << to_string(index) << endl;
    this->globalVariables.find(globalName);
    if(globalVariables.find(globalName) != globalVariables.end()) {
        LOG_ERROR << "two global variable have the same name";
    }
    this->globalVariables[globalName] = index;
}

void AssemblyParser::executePRINTCommand(const string &printCommand, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(printCommand, AssemblyParser::STRING_DELIMITER);
    routineGenerator.printString(commandParts.at(1));
    cout << commandParts.at(1) << endl;
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
    cout << commandParts.at(1) << endl;
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

    cout << " "  << label << endl;
    routineGenerator.jumpEquals(label, true, *param1, *param2);
}

void AssemblyParser::executeJUMPCommand(const string &jumpCommand, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(jumpCommand, '?');
    string label = commandParts.at(1);
    cout << label << endl;
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
            cerr << "More than " << MAX_PARAMS << " params provided for instruction" << endl;
            throw;
        }
    }

    routineGenerator.callRoutine(callRoutineName, storeId, params.at(0).get(), params.at(1).get(), params.at(2).get());
}

void AssemblyParser::executeCALL1nCommand(const string &callCommand, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(callCommand, ' ');
    string callRoutineName = commandParts.at(1);
    cout << callRoutineName << endl;

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
        cout << ":::::: new line" << endl;
    } else if (commandPart.compare(AssemblyParser::PRINT_COMMAND) == 0) {
        cout << ":::::: new print ";
        executePRINTCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::JE_COMMAND) == 0) {
        cout << ":::::: new je ";
        executeJECommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::QUIT_COMMAND) == 0) {
        routineGenerator.quitRoutine();
        cout << ":::::: new quit" << endl;
    } else if (commandPart.compare(AssemblyParser::READ_CHAR_COMMAND) == 0) {
        executeREADCommand(command, routineGenerator);
        cout << ":::::: new read" << endl;
    } else if (commandPart.compare(AssemblyParser::CALL_VS_COMMAND) == 0
               || commandPart.compare(AssemblyParser::CALL_COMMAND) == 0) {
        cout << ":::::: new call ";
        executeCALLCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::CALL_1N_COMMAND) == 0) {
        cout << ":::::: new call_1n";
        executeCALL1nCommand(command, routineGenerator);
    } else if(commandPart.compare(AssemblyParser::JUMP_COMMAND) == 0) {
        cout << ":::::: new jump ";
        executeJUMPCommand(command, routineGenerator);
    } else if(commandPart.compare(AssemblyParser::RET_COMMAND) == 0) {
        cout << ":::::: new return routine ";
        executeRETCommand(command, routineGenerator);
    } else if (commandPart.at(commandPart.size() - 1) == ':') {
        string label = commandPart.substr(0, commandPart.size() - 1);
        cout << ":::::: new label: " << label << endl;
        routineGenerator.newLabel(label);

        string afterLabel = command.substr(commandPart.size());
        executeCommand(trim(afterLabel), *currentGenerator);
    } else if(commandPart.compare(AssemblyParser::SET_TEXT_STYLE) == 0) {
        cout << ":::::: new set_text_style ";
        executeSETTEXTSTYLECommand(command, routineGenerator);
    } else {
        cout << "unknown command: " << command << endl;
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
        cout << "Could not find variable " << id << endl;
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
        cout << "found token " << token << endl;
        tokens.push_back(token);
        s_copy.erase(0, pos + delim.length());
    }
    tokens.push_back(s_copy);

    return tokens;
}


