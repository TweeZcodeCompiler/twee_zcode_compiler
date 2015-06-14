//
// Created by manuel on 08.06.15.
//

#include "AssemblyParser.h"
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <bitset>
#include <cstdint>


const std::string AssemblyParser::ROUTINE_DIRECTIVE = ".FUNCT";
const std::string AssemblyParser::GVAR_DIRECTIVE = ".GVAR";

const std::string AssemblyParser::NEW_LINE_COMMAND = "new_line";
const std::string AssemblyParser::PRINT_COMMAND = "print";
const std::string AssemblyParser::JE_COMMAND = "je";
const std::string AssemblyParser::QUIT_COMMAND = "quit";
const std::string AssemblyParser::READ_CHAR_COMMAND = "read_char";
const std::string AssemblyParser::CALL_COMMAND = "call";
const std::string AssemblyParser::JUMP_COMMAND = "jump";

const char AssemblyParser::SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND = ' '; // 9 is ascii for tab
const char AssemblyParser::STRING_IDENTIFIER = '\"'; // 9 is ascii for tab
const std::string AssemblyParser::ASSIGNMENT_OPERATOR = "->";

std::string trim(const std::string& str,
                 const std::string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

void AssemblyParser::readAssembly(std::istream& input, std::vector <std::bitset<8>> &highMemoryZcode,
                                  size_t offset) {

    std::cout << "Compiler: Parse Assembly File\n";

    for(std::string line; getline(input, line);) {
        std::vector<std::string> lineComps;
        this->split(line, SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND, lineComps);
        if(lineComps.size()) {
            std::string firstComp = lineComps.at(0);

            if(line.at(0) == '.') { // directive
                if(firstComp.compare(ROUTINE_DIRECTIVE) == 0) {
                    std::cout << "found routine" << std::endl;

                    if(lineComps.size() < 2) {
                        std::cerr << "invalid routine declaration" << std::endl;
                        throw;
                    }

                    // currentGenerator exists, so we can get its code
                    if(currentGenerator) {
                        finishRoutine(highMemoryZcode);
                    }

                    std::string routineName = lineComps.at(1);

                    currentGenerator.reset(new RoutineGenerator(routineName, 0, highMemoryZcode, offset));
                } else if(firstComp.compare(GVAR_DIRECTIVE) == 0) {
                    std::cout << "found gvar" << std::endl;

                    this->split(line, SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND, lineComps);
                    if(lineComps.size() < 2) {
                        std::cerr << "empty gvar declaration" << std::endl;
                    }

                    std::string gvar = lineComps.at(1);

                    addGlobal(gvar);
                }
            } else { // normal instruction
                // check for label
                auto pos = line.find(":");
                if(pos != std::string::npos) {
                    std::cout << ":::::: new label: ";
                    std::string labelName = line.substr(0, pos);
                    currentGenerator->newLabel(labelName);
                    std::cout << labelName << std::endl;

                    std::string afterLabel = line.substr(pos + 1, line.length() - 1);
                    executeCommand(afterLabel, *currentGenerator);
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

void AssemblyParser::finishRoutine(std::vector <std::bitset<8>> &highMemoryZcode) {
    std::cout << "adding routine to zcode" << std::endl;
    auto routineCode = currentGenerator->getRoutine();
    Utils::append(highMemoryZcode, routineCode);
}


void AssemblyParser::addGlobal(std::string globalName) {
    // TODO: check if maximum gvar limit is reached
    unsigned index = (unsigned) globalVariableStack.size();
    std::cout << "adding gvar " << globalName << " at index " << std::to_string(index) << std::endl;
    this->globalVariableStack[globalName] = index;
}

RoutineGenerator& AssemblyParser::executePRINTCommand(const std::string &printCommand, RoutineGenerator &routineGenerator) {

    std::vector <std::string> commandParts = this->split(printCommand, AssemblyParser::STRING_IDENTIFIER);
    routineGenerator.printString(commandParts.at(1));
    std::cout << commandParts.at(1) << std::endl;
    return routineGenerator;
}

RoutineGenerator& AssemblyParser::executeREADCommand(const std::string &readCommand, RoutineGenerator &routineGenerator) {
    //this->globalVariableStack.insert(std::pair<std::string, int>("sp", variableUsed));
    std::vector <std::string> commandParts = this->split(readCommand, AssemblyParser::ASSIGNMENT_OPERATOR);
    if(commandParts.size() < 2) {
        // TODO: decide on whether to allow no args for read_char
        return routineGenerator;
    }
    auto last = trim(commandParts.back());
    routineGenerator.readChar(getAddressForId(last)); //TODO: get available address
    return routineGenerator;
}

RoutineGenerator& AssemblyParser::executeJECommand(const std::string &jeCommand, RoutineGenerator &routineGenerator) {

    std::vector <std::string> commandParts = this->split(jeCommand, '?');
    std::string label = commandParts.at(1);
    std::cout << label << std::endl;
    routineGenerator.jumpEquals(label, true, 0x10, 49, true, false);

    return routineGenerator;
}

RoutineGenerator& AssemblyParser::executeJUMPCommand(const std::string &jumpCommand, RoutineGenerator &routineGenerator) {

    std::vector <std::string> commandParts = this->split(jumpCommand, '?');
    std::string label = commandParts.at(1);
    std::cout << label << std::endl;
    routineGenerator.jump(label);

    return routineGenerator;
}

RoutineGenerator& AssemblyParser::executeCALLCommand(const std::string &callCommand, RoutineGenerator &routineGenerator) {

    std::vector <std::string> commandParts = this->split(callCommand, ' ');
    std::string callRoutineName = commandParts.at(1);
    std::cout << callRoutineName << std::endl;

    routineGenerator.callRoutine(callRoutineName);
    return routineGenerator;
}


RoutineGenerator& AssemblyParser::executeCommand(const std::string& command, RoutineGenerator &routineGenerator) {

    std::vector <std::string> commandParts = this->split(command,
                                                         AssemblyParser::SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND);

    if(!commandParts.size()) return routineGenerator;

    std::string commandPart = commandParts.at(0);

    if (commandPart.compare(AssemblyParser::NEW_LINE_COMMAND) == 0) {
        routineGenerator.newLine();
        std::cout << ":::::: new line" << std::endl;
    } else if (commandPart.compare(AssemblyParser::PRINT_COMMAND) == 0) {
        std::cout << ":::::: new print ";
        routineGenerator = executePRINTCommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::JE_COMMAND) == 0) {
        std::cout << ":::::: new je ";
        routineGenerator = executeJECommand(command, routineGenerator);
    } else if (commandPart.compare(AssemblyParser::QUIT_COMMAND) == 0) {
        routineGenerator.quitRoutine();
        std::cout << ":::::: new quit" << std::endl;
    } else if (commandPart.compare(AssemblyParser::READ_CHAR_COMMAND) == 0) {
        routineGenerator = executeREADCommand(command, routineGenerator);
        std::cout << ":::::: new read" << std::endl;
    } else if (commandPart.compare(AssemblyParser::CALL_COMMAND) == 0) {
        std::cout << ":::::: new call ";
        routineGenerator = executeCALLCommand(command, routineGenerator);
    } else if(command.compare(AssemblyParser::JUMP_COMMAND) == 0) {
        std::cout << ":::::: new jump ";
        routineGenerator = executeJUMPCommand(command, routineGenerator);
    } else {
        // TODO: handle this error more appropriately
        std::cout << "unknown command: " << command << std::endl;
        throw;
    }

    return routineGenerator;
}

std::vector <std::string> AssemblyParser::getRoutines(std::istream& input) {

    std::vector <std::string> routineList;
    std::string routine = "";

    //get all routines in own strings
    for (std::string command; getline(input, command);) {
        if (command.size() != 0 && command.at(0) != ';') {
            if (checkIfCommandRoutineStart(command)) {
                if (routine.compare("") != 0) {
                    std::cout << "routine: " << routine << " routine end" << "\n\n";
                    routineList.push_back(routine);
                }
                routine = "";
            }
            routine += command + "\n";
        }
    }


    if (routine.compare("") != 0) {
        std::cout << "routine: " << routine << " routine end" << "\n\n";
        routineList.push_back(routine);
    }

    std::cout << "Amount of Routines:" << routineList.size() << std::endl;
    return routineList;
}


bool AssemblyParser::checkIfCommandRoutineStart(const std::string &command) {
    std::vector <std::string> commandParts = this->split(command, ' ');
    for (unsigned int i = 0; i < commandParts.size(); i++) {
        if (commandParts.at(i).compare(ROUTINE_DIRECTIVE) == 0) {
            return true;
        }

    }
    return false;
}


uint8_t AssemblyParser::getAddressForId(const std::string& id) {
    // TODO: check local variables
    if(id.compare("sp") == 0) {
        return 0x0;
    }
    // check global variables
    try {
        return globalVariableStack.at(id);
    } catch(std::out_of_range) {
        std::cout << "Could not find global " << id << std::endl;
        throw;
    }
}

std::vector <std::string> &AssemblyParser::split(const std::string &s, char delim, std::vector <std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector <std::string> AssemblyParser::split(const std::string &s, char delim) {
    std::vector <std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::vector <std::string> AssemblyParser::split(const std::string &s, const std::string &delim) {
    std::string s_copy = s;
    size_t pos = 0;
    std::string token;
    std::vector<std::string> tokens;
    while ((pos = s_copy.find(delim)) != std::string::npos) {
        token = s_copy.substr(0, pos);
        std::cout << "found token " << token << std::endl;
        tokens.push_back(token);
        s_copy.erase(0, pos + delim.length());
    }
    tokens.push_back(s_copy);

    return tokens;
}


