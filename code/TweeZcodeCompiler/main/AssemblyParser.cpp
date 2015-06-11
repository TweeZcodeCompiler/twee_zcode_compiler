//
// Created by manuel on 08.06.15.
//

#include "AssemblyParser.h"
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <bitset>


const std::string AssemblyParser::ROUTINE_COMMAND = ".FUNCT";
const std::string AssemblyParser::NEW_LINE_COMMAND = "new_line";
const std::string AssemblyParser::PRINT_COMMAND = "print";
const std::string AssemblyParser::JE_COMMAND = "je";
const std::string AssemblyParser::QUIT_COMMAND = "quit";
const std::string AssemblyParser::READ_CHAR_COMMAND = "read_char";
const std::string AssemblyParser::CALL_COMMAND = "call";
const std::string AssemblyParser::JUMP_COMMAND = "jump";

const char AssemblyParser::SPLITTER_BETWEEN_LEXEMS_IN_AN_COMMAND = ' '; // 9 is ascii for tab
const char AssemblyParser::STRING_IDENTIFIER = '\"'; // 9 is ascii for tab






void AssemblyParser::readAssembly(std::string assFilePath, std::vector <std::bitset<8>> &highMemoryZcode,
                                  size_t offset) {

    std::cout << "Compiler: Parse Assembly File\n";
    this->variableUsed = 0;

    std::vector <std::string> routineList = getRoutinesFromFile(assFilePath);

    for (int i = 0; i < routineList.size(); i++) {
        std::vector <std::bitset<8>> routineZCode;
        routineZCode = getZCodeForRoutine(routineList.at(i), highMemoryZcode, offset);
        Utils::append(highMemoryZcode, routineZCode);
    }
}

std::vector <std::bitset<8>> AssemblyParser::getZCodeForRoutine(std::string routine,
                                                                std::vector <std::bitset<8>> &highMemoryZcode,
                                                                size_t offset) {

    std::vector <std::bitset<8>> zCodeRoutine;

    std::vector <std::string> commands = this->split(routine, '\n');
    std::string routineName = split(commands.at(0), ' ').at(1);
    std::cout << ":::::: routine " << routineName << std::endl;
    RoutineGenerator routineGenerator = RoutineGenerator(routineName, 0, highMemoryZcode, offset);

    std::string command;
    for (int i = 0; i < commands.size(); i++) {
        command = commands.at(i);
        routineGenerator = executeCommand(command, routineGenerator);
    }

    zCodeRoutine = routineGenerator.getRoutine();
    return zCodeRoutine;
}


RoutineGenerator AssemblyParser::executePRINTCommand(std::string printCommand, RoutineGenerator &routineGenerator) {

    std::vector <std::string> commandParts = this->split(printCommand, AssemblyParser::STRING_IDENTIFIER);
    routineGenerator.printString(commandParts.at(1));
    std::cout << commandParts.at(1) << std::endl;
    return routineGenerator;
}

RoutineGenerator AssemblyParser::executeREADCommand(std::string readCommand, RoutineGenerator &routineGenerator) {

    this->globalVariableStack.insert(std::pair<std::string, int>("sp", variableUsed));
    variableUsed++;
    routineGenerator.readChar(0x10); //TODO: get avialabe address
    return routineGenerator;
}

RoutineGenerator AssemblyParser::executeJECommand(std::string jeCommand, RoutineGenerator &routineGenerator) {

    std::vector <std::string> commandParts = this->split(jeCommand, '?');
    std::string label = commandParts.at(1);
    std::cout << label << std::endl;
    routineGenerator.jumpEquals(label, true, 0x10, 49, true, false);

    return routineGenerator;
}
RoutineGenerator AssemblyParser::executeJUMPCommand(std::string jumpCommand, RoutineGenerator &routineGenerator) {

    std::vector <std::string> commandParts = this->split(jumpCommand, '?');
    std::string label = commandParts.at(1);
    std::cout << label << std::endl;
    routineGenerator.jump(label);

    return routineGenerator;
}

RoutineGenerator AssemblyParser::executeCALLCommand(std::string callCommand, RoutineGenerator &routineGenerator) {

    std::vector <std::string> commandParts = this->split(callCommand, ' ');
    std::string callRoutineName = commandParts.at(1);
    std::cout << callRoutineName << std::endl;

    routineGenerator.callRoutine(callRoutineName);
    return routineGenerator;
}


RoutineGenerator AssemblyParser::executeCommand(std::string command, RoutineGenerator &routineGenerator) {

    std::vector <std::string> commandParts = this->split(command,
                                                         AssemblyParser::SPLITTER_BETWEEN_LEXEMS_IN_AN_COMMAND);
    for (int i = 0; i < commandParts.size(); i++) {
        std::string commandPart = commandParts.at(i);
        char lastCharacter = commandPart.back();
        if (lastCharacter == ':') //new label
        {
            std::cout << ":::::: new label: ";
            std::string labelName = commandPart.substr(0, commandPart.size() - 1);
            routineGenerator.newLabel(labelName);
            std::cout << labelName << std::endl;
        }

        if (commandPart.compare(AssemblyParser::NEW_LINE_COMMAND) == 0) {
            routineGenerator.newLine();
            std::cout << ":::::: new line" << std::endl;
        }
        if (commandPart.compare(AssemblyParser::PRINT_COMMAND) == 0) {

            std::cout << ":::::: new print ";
            routineGenerator = executePRINTCommand(command, routineGenerator);

        }
        if (commandPart.compare(AssemblyParser::JE_COMMAND) == 0) {
            std::cout << ":::::: new je ";
            routineGenerator = executeJECommand(command, routineGenerator);

        }
        if (commandPart.compare(AssemblyParser::QUIT_COMMAND) == 0) {
            routineGenerator.quitRoutine();
            std::cout << ":::::: new quit" << std::endl;
        }
        if (commandPart.compare(AssemblyParser::READ_CHAR_COMMAND) == 0) {
            routineGenerator = executeREADCommand(command, routineGenerator);
            std::cout << ":::::: new read" << std::endl;
        }

        if (commandPart.compare(AssemblyParser::CALL_COMMAND) == 0) {
            std::cout << ":::::: new call ";
            routineGenerator = executeCALLCommand(command, routineGenerator);

        }
        if(command.compare(AssemblyParser::JUMP_COMMAND) == 0) {
            std::cout << ":::::: new jump ";
            routineGenerator = executeJUMPCommand(command, routineGenerator);
        }


    }


    return routineGenerator;


}

std::vector <std::string> AssemblyParser::getRoutinesFromFile(std::string assFilePath) {

    std::vector <std::string> routineList;
    std::string routine = "";
    std::ifstream input(assFilePath);


    //get all routines in own strings
    for (std::string command; getline(input, command);) {

        if (command.size() != 0) {
            if (command.at(0) != ';') //comment symbol
            {
                if (checkIfCommandRoutineStart(command) == true) {
                    if (routine.compare("") != 0) {
                        std::cout << "routine: " << routine << " routine end" << "\n\n";
                        routineList.push_back(routine);
                    }
                    routine = "";
                }
                routine += command + "\n";
            }
        }


    }
    if (routine.compare("") != 0) {
        std::cout << "routine: " << routine << " routine end" << "\n\n";
        routineList.push_back(routine);
    }

    std::cout << "Amount of Routines:" << routineList.size() << std::endl;
    return routineList;

}


bool AssemblyParser::checkIfCommandRoutineStart(std::string command) {
    std::vector <std::string> commandParts = this->split(command, ' ');
    for (int i = 0; i < commandParts.size(); i++) {
        if (commandParts.at(i).compare(ROUTINE_COMMAND) == 0) {
            return true;
        }

    }
    return false;
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