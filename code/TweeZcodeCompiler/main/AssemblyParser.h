//
// Created by manuel on 08.06.15.
//

#ifndef PROJECT_ASSEMBLYPARSER_H
#define PROJECT_ASSEMBLYPARSER_H

#include <iostream>
#include <vector>
#include <bitset>
#include <memory>
#include "RoutineGenerator.h"
#include "Utils.h"


class AssemblyParser {

private:

    static const char SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND;
    static const std::string GVAR_DIRECTIVE;
    static const char STRING_DELIMITER;
    static const std::string ASSIGNMENT_OPERATOR;

    static const std::string ROUTINE_DIRECTIVE;
    static const std::string NEW_LINE_COMMAND;
    static const std::string PRINT_COMMAND;
    static const std::string JE_COMMAND;
    static const std::string JG_COMMAND;
    static const std::string JL_COMMAND;
    static const std::string JZ_COMMAND;
    static const std::string QUIT_COMMAND;
    static const std::string READ_CHAR_COMMAND;
    static const std::string PRINT_CHAR_COMMAND;
    static const std::string CALL_VS_COMMAND;
    static const std::string CALL_1N_COMMAND;
    static const std::string JUMP_COMMAND;
    static const std::string RET_COMMAND;
    static const std::string SET_TEXT_STYLE;
    static const std::string STORE_COMMAND;


    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

    std::vector<std::string> split(const std::string &s, char delim);
    std::vector<std::string> split(const std::string &s, const std::string &delim);

    bool checkIfCommandRoutineStart(const std::string &command);

    void executeCommand(const std::string &command, RoutineGenerator &routineGenerator);

    void executePRINTCommand(const std::string &printCommand, RoutineGenerator &routineGenerator);
    
    void executeSETTEXTSTYLECommand(const std::string &printCommand, RoutineGenerator &routineGenerator);

    void executeREADCommand(const std::string &readCommand, RoutineGenerator &routineGenerator);

    void executeJECommand(const std::string &jeCommand, RoutineGenerator &routineGenerator);
    void executeJGCommand(const std::string &jeCommand, RoutineGenerator &routineGenerator);
    void executeJLCommand(const std::string &jeCommand, RoutineGenerator &routineGenerator);
    void executeJZCommand(const std::string &jeCommand, RoutineGenerator &routineGenerator);

    void executeCALL1nCommand(const std::string &callCommand, RoutineGenerator &routineGenerator);
    void executeCALL_VSCommand(const std::string &callCommand, RoutineGenerator &routineGenerator);

    void executeJUMPCommand(const std::string &jumpCommand, RoutineGenerator &routineGenerator);

    void executeRETCommand(const std::string &callCommand, RoutineGenerator &routineGenerator);

    std::vector<std::unique_ptr<ZParam>> parseArguments(const std::string instruction);
    std::unique_ptr<ZParam> createZParam(const std::string& paramString);


    std::map<std::string, uint8_t> globalVariables;

    std::unique_ptr<RoutineGenerator> currentGenerator;

    void finishRoutine(std::vector <std::bitset<8>> &highMemoryZcode);
    void addGlobal(std::string globalName);
    std::unique_ptr<uint8_t> getAddressForId(const std::string& id);

public:
    void readAssembly(std::istream& input, std::vector<std::bitset<8>> &highMemoryZcode, size_t offset);

};


#endif //PROJECT_ASSEMBLYPARSER_H
