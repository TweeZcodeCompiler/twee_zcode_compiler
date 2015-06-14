//
// Created by manuel on 08.06.15.
//

#ifndef PROJECT_ASSEMBLYPARSER_H
#define PROJECT_ASSEMBLYPARSER_H

#include <iostream>
#include <vector>
#include <bitset>
#include "RoutineGenerator.h"
#include "Utils.h"


class AssemblyParser {

private:

    static const char SPLITTER_BETWEEN_LEXEMES_IN_AN_COMMAND;
    static const std::string GVAR_DIRECTIVE;
    static const char STRING_IDENTIFIER;
    static const std::string ASSIGNMENT_OPERATOR;

    static const std::string ROUTINE_DIRECTIVE;
    static const std::string NEW_LINE_COMMAND;
    static const std::string PRINT_COMMAND;
    static const std::string JE_COMMAND; //jump equals
    static const std::string QUIT_COMMAND;
    static const std::string READ_CHAR_COMMAND;
    static const std::string CALL_COMMAND;
    static const std::string JUMP_COMMAND;


    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

    std::vector<std::string> split(const std::string &s, char delim);
    std::vector<std::string> split(const std::string &s, const std::string &delim);

    bool checkIfCommandRoutineStart(const std::string &command);

    std::vector<std::string> getRoutines(std::istream& input);

    RoutineGenerator& executeCommand(const std::string &command, RoutineGenerator &routineGenerator);

    RoutineGenerator& executePRINTCommand(const std::string &printCommand, RoutineGenerator &routineGenerator);

    RoutineGenerator& executeREADCommand(const std::string &readCommand, RoutineGenerator &routineGenerator);

    RoutineGenerator& executeJECommand(const std::string &jeCommand, RoutineGenerator &routineGenerator);

    RoutineGenerator& executeCALLCommand(const std::string &callCommand, RoutineGenerator &routineGenerator);
    RoutineGenerator& executeJUMPCommand(const std::string &jumpCommand, RoutineGenerator &routineGenerator);


    std::map<std::string, unsigned int> globalVariableStack;

    std::unique_ptr<RoutineGenerator> currentGenerator;

    void finishRoutine(std::vector <std::bitset<8>> &highMemoryZcode);
    void addGlobal(std::string globalName);
    uint8_t getAddressForId(const std::string& id);

public:
    void readAssembly(std::istream& input, std::vector<std::bitset<8>> &highMemoryZcode, size_t offset);


};


#endif //PROJECT_ASSEMBLYPARSER_H
