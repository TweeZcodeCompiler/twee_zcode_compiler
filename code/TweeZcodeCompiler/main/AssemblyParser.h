//
// Created by manuel on 08.06.15.
//

#ifndef PROJECT_ASSEMBLYPARSER_H
#define PROJECT_ASSEMBLYPARSER_H

#include <iostream>
#include <vector>
#include <bitset>
#include "RoutineGenerator.h"
#include  "Utils.h"


class AssemblyParser {

private:

    static const char SPLITTER_BETWEEN_LEXEMS_IN_AN_COMMAND;
    static const char STRING_IDENTIFIER;

    static const std::string ROUTINE_COMMAND;
    static const std::string NEW_LINE_COMMAND;
    static const std::string PRINT_COMMAND;
    static const std::string JE_COMMAND; //jump equals
    static const std::string QUIT_COMMAND;
    static const std::string READ_CHAR_COMMAND;
    static const std::string CALL_COMMAND;


    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

    std::vector<std::string> split(const std::string &s, char delim);

    bool checkIfCommandRoutineStart(std::string command);

    std::vector<std::string> getRoutinesFromFile(std::string fileNames);

    std::vector<std::bitset<8>> getZCodeForRoutine(std::string routine, std::vector<std::bitset<8>> &highMemoryZcode,
                                                   size_t offset);

    RoutineGenerator executeCommand(std::string command, RoutineGenerator &routineGenerator);

    RoutineGenerator executePRINTCommand(std::string printCommand, RoutineGenerator &routineGenerator);

    RoutineGenerator executeREADCommand(std::string readCommand, RoutineGenerator &routineGenerator);

    RoutineGenerator executeJECommand(std::string jeCommand, RoutineGenerator &routineGenerator);

    RoutineGenerator executeCALLCommand(std::string callCommand, RoutineGenerator &routineGenerator);


    std::map<std::string, int> globalVariableStack;
    int variableUsed;


public:
    void readAssembly(std::string assFilePath, std::vector<std::bitset<8>> &highMemoryZcode, size_t offset);

};


#endif //PROJECT_ASSEMBLYPARSER_H
