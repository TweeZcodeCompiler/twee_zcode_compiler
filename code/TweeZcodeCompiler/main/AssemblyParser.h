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
#include "ZCodeObjects/ZCodeContainer.h"
#include "ZCodeObjects/ZCodeCallAdress.h"


class AssemblyParser {

private:

    static const char SPLITTER_BETWEEN_LEXEMES_IN_A_COMMAND;
    static const std::string GVAR_DIRECTIVE;
    static const char STRING_DELIMITER;
    static const std::string ASSIGNMENT_OPERATOR;
    static const std::string BYTEARRAY;
    static const std::string WORDARRAY;

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
    static const std::string PRINT_NUM_COMMAND;
    static const std::string PRINT_ADDR_COMMAND;
    static const std::string CALL_VS_COMMAND;
    static const std::string CALL_1N_COMMAND;
    static const std::string CALL_VN_COMMAND;
    static const std::string JUMP_COMMAND;
    static const std::string RET_COMMAND;
    static const std::string SET_TEXT_STYLE;
    static const std::string STORE_COMMAND;
    static const std::string LOAD_COMMAND;
    static const std::string ADD_COMMAND;
    static const std::string SUB_COMMAND;
    static const std::string MUL_COMMAND;
    static const std::string DIV_COMMAND;
    static const std::string MOD_COMMAND;
    static const std::string AND_COMMAND;
    static const std::string OR_COMMAND;
    static const std::string NOT_COMMAND;
    static const std::string RET_TRUE_COMMAND;
    static const std::string RET_FALSE_COMMAND;
    static const std::string PRINT_RET_COMMAND;
    static const std::string RESTART_COMMAND;
    static const std::string POP_COMMAND;
    static const std::string RET_POPPED_COMMAND;
    static const std::string VERIFY_COMMAND;
    static const std::string STOREB_COMMAND;
    static const std::string STOREW_COMMAND;
    static const std::string LOADB_COMMAND;
    static const std::string LOADW_COMMAND;
    static const std::string PUSH_COMMAND;
    static const std::string PULL_COMMAND;
    static const std::string RANDOM_COMMAND;
    static const std::string READ_MOUSE;
    static const std::string MOUSE_WINDOW;
    static const std::string GET_CURSOR;
    static const std::string SET_CURSOR;
    static const std::string GET_WINDOW_PROPERTY;
    static const std::string WINDOW_STYLE;
    static const std::string SCROLL_WINDOW;
    static const std::string PUT_WIND_PROP;


    unsigned currentLineNumber;

    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

    std::vector<std::string> split(const std::string &s, char delim);

    std::vector<std::string> split(const std::string &s, const std::string &delim);

    bool checkIfCommandRoutineStart(const std::string &command);

    void executeCommand(const std::string &command, RoutineGenerator &routineGenerator,
                        std::shared_ptr<ZCodeContainer> dynamicMemory);

    void executePRINTCommand(const std::string &printCommand, RoutineGenerator &routineGenerator);

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

    std::unique_ptr<ZParam> createZParam(const std::string &paramString);

    void performRoutineDirectiveCommand(std::vector<std::string> lineComps, std::shared_ptr<ZCodeContainer> highMemory);

    void performRoutineGlobalVarCommand(std::string line);


    std::vector<std::pair<std::string, unsigned>> registeredJumpsAtLines;
    std::vector<std::string> registeredLabels;
    std::map<std::string, uint8_t> globalVariables;

    bool firstRoutine = true;

    std::unique_ptr<RoutineGenerator> currentGenerator;

    void registerJump(const std::vector<std::unique_ptr<ZParam>> &params);

    void finishRoutine(std::shared_ptr<ZCodeContainer> highMemoryZcode);

    void addGlobal(std::string globalName);

    std::unique_ptr<uint8_t> getAddressForId(const std::string &id);

public:
    void readAssembly(std::istream &input, std::shared_ptr<ZCodeContainer> dynamicMemory,
                      std::shared_ptr<ZCodeContainer> staticMemory, std::shared_ptr<ZCodeContainer> highMemory);

    void performByteArrayDirective(std::string command, std::shared_ptr<ZCodeContainer> shared_ptr);

    void performWordArrayDirective(std::string basic_string, std::shared_ptr<ZCodeContainer> shared_ptr);

    void executeSTOREBCOMMAND(const std::string &basic_string, std::shared_ptr<ZCodeContainer> shared_ptr,
                              RoutineGenerator &currentGenerator);

    void executeSTOREWCOMMAND(const std::string &basic_string, std::shared_ptr<ZCodeContainer> shared_ptr,
                              RoutineGenerator &currentGenerator);

    void executeLOADBCOMMAND(const std::string &basic_string, std::shared_ptr<ZCodeContainer> shared_ptr,
                             RoutineGenerator &currentGenerator);

    void executeLOADWCOMMAND(const std::string &basic_string, std::shared_ptr<ZCodeContainer> shared_ptr,
                             RoutineGenerator &currentGenerator);

};


#endif //PROJECT_ASSEMBLYPARSER_H
