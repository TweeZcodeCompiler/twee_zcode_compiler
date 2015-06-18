//
// Created by manuel on 08.06.15.
//

#include "AssemblyParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <bitset>
#include <cstdint>

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
const string AssemblyParser::CALL_VS_COMMAND = "call_vs";
const string AssemblyParser::CALL_1N_COMMAND = "call_1n";

const char AssemblyParser::SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND = ' '; // 9 is ascii for tab
const char AssemblyParser::STRING_IDENTIFIER = '\"'; // 9 is ascii for tab
const string AssemblyParser::ASSIGNMENT_OPERATOR = "->";



string trim(const string& str,
                 const string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

void AssemblyParser::readAssembly(istream& input, vector <bitset<8>> &highMemoryZcode,
                                  size_t offset) {

    cout << "Compiler: Parse Assembly File\n";

    for(string line; getline(input, line);) {
        line = trim(line);
        vector<string> lineComps;
        this->split(line, SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND, lineComps);
        if(lineComps.size()) {
            string firstComp = lineComps.at(0);

            if(line.at(0) == '.') { // directive
                if(firstComp.compare(ROUTINE_DIRECTIVE) == 0) {
                    cout << "found routine" << endl;

                    if(lineComps.size() < 2) {
                        cerr << "invalid routine declaration" << endl;
                        throw;
                    }

                    // currentGenerator exists, so we can get its code
                    if(currentGenerator) {
                        finishRoutine(highMemoryZcode);
                    }

                    string routineName = lineComps.at(1);

                    currentGenerator.reset(new RoutineGenerator(routineName, 0, highMemoryZcode, offset));
                } else if(firstComp.compare(GVAR_DIRECTIVE) == 0) {
                    cout << "found gvar" << endl;

                    this->split(line, SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND, lineComps);
                    if(lineComps.size() < 2) {
                        cerr << "empty gvar declaration" << endl;
                    }

                    string gvar = lineComps.at(1);

                    addGlobal(gvar);
                }
            } else { // normal instruction
                // check for label
                auto pos = line.find(":");
                if(pos != string::npos) {
                    cout << ":::::: new label: ";
                    string labelName = line.substr(0, pos);
                    currentGenerator->newLabel(labelName);
                    cout << labelName << endl;

                    string afterLabel = line.substr(pos + 1, line.length() - 1);
                    executeCommand(trim(afterLabel), *currentGenerator);
                } else {
                    executeCommand(line, *currentGenerator);
                }
            }
        }
    }

    if(currentGenerator) {
        finishRoutine(highMemoryZcode);
    }
}

void AssemblyParser::finishRoutine(vector <bitset<8>> &highMemoryZcode) {
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
    unsigned index = (unsigned) globalVariableStack.size();
    cout << "adding gvar " << globalName << " at index " << to_string(index) << endl;
    this->globalVariableStack[globalName] = index;
}

RoutineGenerator& AssemblyParser::executePRINTCommand(const string &printCommand, RoutineGenerator &routineGenerator) {

    vector <string> commandParts = this->split(printCommand, AssemblyParser::STRING_IDENTIFIER);
    routineGenerator.printString(commandParts.at(1));
    cout << commandParts.at(1) << endl;
    return routineGenerator;
}

RoutineGenerator& AssemblyParser::executeREADCommand(const string &readCommand, RoutineGenerator &routineGenerator) {
    vector <string> commandParts = this->split(readCommand, AssemblyParser::ASSIGNMENT_OPERATOR);
    if(commandParts.size() < 2) {
        // TODO: decide on whether to allow no args for read_char
        return routineGenerator;
    }
    auto last = trim(commandParts.back());
    routineGenerator.readChar(getAddressForId(last)); //TODO: get available address
    return routineGenerator;
}

RoutineGenerator& AssemblyParser::executeJECommand(const string &jeCommand, RoutineGenerator &routineGenerator) {

    vector <string> commandParts = this->split(jeCommand, '?');
    string label = commandParts.at(1);

    commandParts = split(jeCommand,' ');
    unique_ptr<ZParam> param1 = createZParam(commandParts.at(1));
    unique_ptr<ZParam> param2 = createZParam(commandParts.at(2));

    cout << " "  << label << endl;
    routineGenerator.jumpEquals(label, true, *param1, *param2);

    return routineGenerator;
}

RoutineGenerator& AssemblyParser::executeJUMPCommand(const string &jumpCommand, RoutineGenerator &routineGenerator) {

    vector <string> commandParts = this->split(jumpCommand, '?');
    string label = commandParts.at(1);
    cout << label << endl;
    routineGenerator.jump(label);

    return routineGenerator;
}

RoutineGenerator& AssemblyParser::executeCALLCommand(const string &callCommand, RoutineGenerator &routineGenerator) {
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

    return routineGenerator;
}

RoutineGenerator& AssemblyParser::executeCALL1nCommand(const string &callCommand, RoutineGenerator &routineGenerator) {

    vector <string> commandParts = this->split(callCommand, ' ');
    string callRoutineName = commandParts.at(1);
    cout << callRoutineName << endl;

    routineGenerator.callRoutine1n(callRoutineName);
    return routineGenerator;
}


RoutineGenerator& AssemblyParser::executeRETCommand(const string &callCommand, RoutineGenerator &routineGenerator) {
    vector <string> commandParts = this->split(callCommand, ' ');
    string value = commandParts.at(1);

    routineGenerator.returnValue(*createZParam(value));

    return routineGenerator;
}


RoutineGenerator& AssemblyParser::executeCommand(const string& command, RoutineGenerator &routineGenerator) {

    vector <string> commandParts = this->split(command,
                                                         AssemblyParser::SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND);

    if(!commandParts.size()) return routineGenerator;

    string commandPart = commandParts.at(0);

    if (commandPart.compare(AssemblyParser::NEW_LINE_COMMAND) == 0) {
        routineGenerator.newLine();
        cout << ":::::: new line" << endl;
    } else if (commandPart.compare(AssemblyParser::PRINT_COMMAND) == 0) {
        cout << ":::::: new print ";
        routineGenerator = executePRINTCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::JE_COMMAND) == 0) {
        cout << ":::::: new je ";
        routineGenerator = executeJECommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::QUIT_COMMAND) == 0) {
        routineGenerator.quitRoutine();
        cout << ":::::: new quit" << endl;
    } else if (commandPart.compare(AssemblyParser::READ_CHAR_COMMAND) == 0) {
        routineGenerator = executeREADCommand(command, routineGenerator);
        cout << ":::::: new read" << endl;
    } else if (commandPart.compare(AssemblyParser::CALL_COMMAND) == 0
               || commandPart.compare(AssemblyParser::CALL_COMMAND) == 0) {
        cout << ":::::: new call ";
        routineGenerator = executeCALLCommand(command, routineGenerator);
    } else if(commandPart.compare(AssemblyParser::JUMP_COMMAND) == 0) {
        cout << ":::::: new jump ";
        routineGenerator = executeJUMPCommand(command, routineGenerator);
    } else if(commandPart.compare(AssemblyParser::RET_COMMAND) == 0) {
        cout << ":::::: new return routine ";
        routineGenerator = executeRETCommand(command, routineGenerator);
    } else {
        cout << "unknown command: " << command << endl;
        throw;
    }

    return routineGenerator;
}


bool AssemblyParser::checkIfCommandRoutineStart(const string &command) {
    vector <string> commandParts = this->split(command, ' ');
    for (unsigned int i = 0; i < commandParts.size(); i++) {
        if (commandParts.at(i).compare(ROUTINE_DIRECTIVE) == 0) {
            return true;
        }

    }
    return false;
}


uint8_t AssemblyParser::getAddressForId(const string& id) {
    // TODO: check local variables
    if(id.compare("sp") == 0) {
        return 0x0;
    }
    // check global variables
    try {
        return 0x10 + globalVariableStack.at(id);
    } catch(out_of_range) {
        cout << "Could not find global " << id << endl;
        throw;
    }
}

vector <string> &AssemblyParser::split(const string &s, char delim, vector <string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector <string> AssemblyParser::split(const string &s, char delim) {
    vector <string> elems;
    split(s, delim, elems);
    return elems;
}

vector <string> AssemblyParser::split(const string &s, const string &delim) {
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


