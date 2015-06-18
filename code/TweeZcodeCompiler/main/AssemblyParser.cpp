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

void AssemblyParser::readAssembly(istream &input, vector<bitset<8>> &highMemoryZcode,
                                  size_t offset) {

    cout << "Compiler: Parse Assembly File\n";

    for (string line; getline(input, line);) {
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


void AssemblyParser::addGlobal(string globalName) {
    // TODO: check if maximum gvar limit is reached
    unsigned index = (unsigned) globalVariables.size();
    cout << "adding gvar " << globalName << " at index " << to_string(index) << endl;
    this->globalVariables[globalName] = index;
}

RoutineGenerator &AssemblyParser::executePRINTCommand(const string &printCommand, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(printCommand, AssemblyParser::STRING_DELIMITER);
    routineGenerator.printString(commandParts.at(1));
    cout << commandParts.at(1) << endl;
    return routineGenerator;
}

RoutineGenerator &AssemblyParser::executeREADCommand(const string &readCommand, RoutineGenerator &routineGenerator) {
    vector<string> commandParts = this->split(readCommand, AssemblyParser::ASSIGNMENT_OPERATOR);
    if (commandParts.size() < 2) {
        // TODO: decide on whether to allow no args for read_char
        return routineGenerator;
    }
    auto last = trim(commandParts.back());
    routineGenerator.readChar(getAddressForId(last));
    return routineGenerator;
}

RoutineGenerator &AssemblyParser::executeJECommand(const string &jeCommand, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(jeCommand, '?');
    string label = commandParts.at(1);

    commandParts = split(jeCommand, ' ');
    string variableName = commandParts.at(2);
    string valueString = commandParts.at(1);
    uint16_t value = stoul(valueString); // TODO: check if this fits 8 or 16 bits, then set small or large constant
    cout << variableName << "," << "" << value;

    uint8_t globalZCodeAdress = getAddressForId(variableName);

    cout << " " << label << endl;
    routineGenerator.jumpEquals(label, true, globalZCodeAdress, value, true, false);

    return routineGenerator;
}

RoutineGenerator &AssemblyParser::executeJUMPCommand(const string &jumpCommand, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(jumpCommand, '?');
    string label = commandParts.at(1);
    cout << label << endl;
    routineGenerator.jump(label);

    return routineGenerator;
}

RoutineGenerator &AssemblyParser::executeCALLCommand(const string &callCommand, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(callCommand, ' ');
    string callRoutineName = commandParts.at(1);
    cout << callRoutineName << endl;

    routineGenerator.callRoutine(callRoutineName);
    return routineGenerator;
}


RoutineGenerator &AssemblyParser::executeCommand(const string &command, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(command,
                                              AssemblyParser::SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND);

    if (!commandParts.size()) return routineGenerator;

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
    } else if (commandPart.compare(AssemblyParser::CALL_COMMAND) == 0) {
        cout << ":::::: new call ";
        routineGenerator = executeCALLCommand(command, routineGenerator);
    } else if (command.compare(AssemblyParser::JUMP_COMMAND) == 0) {
        cout << ":::::: new jump ";
        routineGenerator = executeJUMPCommand(command, routineGenerator);
    } else if (command.compare(AssemblyParser::RET_COMMAND) == 0) {
        cout << ":::::: new return routine ";
        routineGenerator.returnValue(0, false);
    } else if (commandPart.at(commandPart.size() - 1) == ':') {
        string label = commandPart.substr(0, commandPart.size() - 1);
        cout << ":::::: new label: " << label << endl;
        routineGenerator.newLabel(label);

        string afterLabel = command.substr(commandPart.size());
        executeCommand(trim(afterLabel), *currentGenerator);
    } else {
        // TODO: handle this error more appropriately
        cout << "unknown command: " << command << endl;
        throw;
    }

    return routineGenerator;
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
        return globalVariables[id];
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


