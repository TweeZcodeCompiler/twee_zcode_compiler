//
// Created by manuel on 08.06.15.
//

#ifndef PROJECT_ASSEMBLYPARSER_H
#define PROJECT_ASSEMBLYPARSER_H

#include <iostream>
#include <vector>
#include <bitset>



class AssemblyParser {

private:

    static const std::string ROUTINE_COMMAND;
    static const std::string NEW_LINE_COMMAND;
    static const std::string PRINT_COMMAND;
    static const std::string JE_COMMAND; //jump equals

    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
    std::vector<std::string> split(const std::string &s, char delim);
    bool checkIfCommandRoutineStart(std::string command);

    std::vector<std::string> getRoutinesFromFile(std::string fileNames);
    std::vector<std::bitset<8>> getZCodeForRoutine(std::string routine);
    std::string getCommandType(std::string command);




public:
    std::vector<std::bitset<8>>  readAssembly(std::string assFilePath);

};


#endif //PROJECT_ASSEMBLYPARSER_H
