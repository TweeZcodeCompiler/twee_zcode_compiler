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
const string AssemblyParser::JG_COMMAND = "jg";
const string AssemblyParser::JL_COMMAND = "jl";
const string AssemblyParser::JZ_COMMAND = "jz";
const string AssemblyParser::QUIT_COMMAND = "quit";
const string AssemblyParser::READ_CHAR_COMMAND = "read_char";
const string AssemblyParser::PRINT_CHAR_COMMAND = "print_char";
const string AssemblyParser::JUMP_COMMAND = "jump";
const string AssemblyParser::RET_COMMAND = "ret";
const string AssemblyParser::SET_TEXT_STYLE = "set_text_style";
const string AssemblyParser::CALL_VS_COMMAND = "call_vs";
const string AssemblyParser::CALL_1N_COMMAND = "call_1n";
const string AssemblyParser::STORE_COMMAND = "store";

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


void AssemblyParser::readAssembly(istream& input, vector <bitset<8>> &highMemoryZcode,
                                  size_t offset) {

    cout << "Compiler: Parse Assembly File\n";


    for(string line; getline(input, line);) {
        line = trim(line);
        vector<string> lineComps;
        this->split(line, SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND, lineComps);
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

                    this->split(line, SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND, lineComps);
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

vector<unique_ptr<ZParam>> AssemblyParser::parseArguments(const string instruction) {
    vector <string> commandParts = this->split(instruction, AssemblyParser::SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND);
    vector<unique_ptr<ZParam>> params;

    if (commandParts.size() < 2) {  // first argument is opcode instruction
        return params;
    } else if ((count(instruction.begin(), instruction.end(), AssemblyParser::STRING_DELIMITER) % 2) != 0) {
        // checks if there are always 2 quotation marks for hard coded string
        cout << endl << endl << "String does not end!" << endl << endl;
        throw;
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
    if (instruction.find(AssemblyParser::ASSIGNMENT_OPERATOR) != string::npos) {   // contains instruction store address?
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
    for(auto part = commandParts.begin() + start; part != commandParts.begin() + paramsCount + 1; ++part) {
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
            while (++stringEndChar != instruction.size() - 1) {
                if (instruction.at(stringEndChar) == AssemblyParser::STRING_DELIMITER) {
                    break;
                }
            }

            string stringParam = instruction.substr(stringStartChar + 1, stringEndChar - stringStartChar - 1);
            params.push_back(unique_ptr<ZNameParam>(new ZNameParam(stringParam)));

            stringEndChar += 2;     // point to first char of next commandPart
            if (stringEndChar >= instruction.size()) {  //
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
            cout << endl << endl << "Unknown store address" << storeAddress << endl << endl;
            throw;
        }
    } else if (containsLabel) {
        auto label = split(instruction, '?').at(1);
        params.push_back(unique_ptr<ZNameParam>(new ZNameParam(label)));
    }

    return params;
}

unique_ptr<ZParam> AssemblyParser::createZParam(const string& paramString) {
    unique_ptr<ZParam> param;

    try {
        int paramNum = stoi(paramString);
        ZValueParam* valueParam = new ZValueParam((uint16_t) paramNum);
        param.reset(valueParam);

        return param;
    } catch(const invalid_argument& invalidArgument){

    }

    // not catching exception on purpose so we always return a proper value
    auto paramId = getAddressForId(paramString);

    if (paramId) {
        ZVariableParam *variableParam = new ZVariableParam((uint16_t) *paramId);
        param.reset(variableParam);
    } else {
        cout << endl << endl << "Could not parse parameter: " << paramString << endl << endl;
        throw;
    }

    return param;
}


void AssemblyParser::addGlobal(string globalName) {
    // TODO: check if maximum gvar limit is reached
    unsigned index = (unsigned) globalVariables.size();
    cout << "adding gvar " << globalName << " at index " << to_string(index) << endl;
    this->globalVariables[globalName] = index;
}

void AssemblyParser::executePRINTCommand(const string &printCommand, RoutineGenerator &routineGenerator) {
    routineGenerator.printString(parseArguments(printCommand));
}

void AssemblyParser::executeSETTEXTSTYLECommand(const string &printCommand, RoutineGenerator &routineGenerator) {

    vector <string> commandParts = this->split(printCommand, AssemblyParser::SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND);
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
    routineGenerator.readChar(parseArguments(readCommand));
}

void AssemblyParser::executeJECommand(const string &jeCommand, RoutineGenerator &routineGenerator) {
    routineGenerator.jumpEquals(parseArguments(jeCommand));
}

void AssemblyParser::executeJGCommand(const string &jeCommand, RoutineGenerator &routineGenerator) {
    routineGenerator.jumpGreaterThan(parseArguments(jeCommand));
}

void AssemblyParser::executeJLCommand(const string &jeCommand, RoutineGenerator &routineGenerator) {
    routineGenerator.jumpLessThan(parseArguments(jeCommand));
}

void AssemblyParser::executeJUMPCommand(const string &jumpCommand, RoutineGenerator &routineGenerator) {
    routineGenerator.jump(parseArguments(jumpCommand));
}

void AssemblyParser::executeJZCommand(const string &jumpCommand, RoutineGenerator &routineGenerator) {
    routineGenerator.jumpZero(parseArguments(jumpCommand));
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

void AssemblyParser::executeCommand(const string &command, RoutineGenerator &routineGenerator) {

    vector<string> commandParts = this->split(command,
                                              AssemblyParser::SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND);

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
    } else if (commandPart.compare(AssemblyParser::JG_COMMAND) == 0) {
        cout << ":::::: new jg ";
        executeJGCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::JL_COMMAND) == 0) {
        cout << ":::::: new jl ";
        executeJLCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::JZ_COMMAND) == 0) {
        cout << ":::::: new jz ";
        executeJZCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::QUIT_COMMAND) == 0) {
        routineGenerator.quitRoutine();
        cout << ":::::: new quit" << endl;
    } else if (commandPart.compare(AssemblyParser::READ_CHAR_COMMAND) == 0) {
        executeREADCommand(command, routineGenerator);
        cout << ":::::: new read_char" << endl;
    } else if (commandPart.compare(AssemblyParser::PRINT_CHAR_COMMAND) == 0) {
        cout << ":::::: new print_char" << endl;
        routineGenerator.printChar(parseArguments(command));
    } else if (commandPart.compare(AssemblyParser::CALL_VS_COMMAND) == 0) {
        cout << ":::::: new call_vs " << endl;
        executeCALL_VSCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::CALL_1N_COMMAND) == 0) {
        cout << ":::::: new call_1n" << endl;
        executeCALL1nCommand(command, routineGenerator);
    } else if(commandPart.compare(AssemblyParser::JUMP_COMMAND) == 0) {
        cout << ":::::: new jump " << endl;
        executeJUMPCommand(command, routineGenerator);
    } else if(commandPart.compare(AssemblyParser::RET_COMMAND) == 0) {
        cout << ":::::: new return routine " << endl;
        executeRETCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::STORE_COMMAND) == 0) {
        cout << ":::::: new store" << endl;
        routineGenerator.store(parseArguments(command));
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


unique_ptr<uint8_t> AssemblyParser::getAddressForId(const string &id) {
    if (id.compare("sp") == 0) {
        return 0;
    }

    // check global variables
    if (globalVariables.count(id)) {
        return unique_ptr<uint8_t> (new uint8_t(((uint8_t) (globalVariables[id] + 0x10))));
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
        cout << "found token " << token << endl;
        tokens.push_back(token);
        s_copy.erase(0, pos + delim.length());
    }
    tokens.push_back(s_copy);

    return tokens;
}


