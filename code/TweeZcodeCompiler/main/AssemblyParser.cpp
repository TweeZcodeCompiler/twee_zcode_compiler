//
// Created by manuel on 08.06.15.
//

#include "AssemblyParser.h"
#include <fstream>
#include <sstream>


const std::string AssemblyParser::ROUTINE_COMMAND = "Routine";
const std::string AssemblyParser::NEW_LINE_COMMAND = "new_line";
const std::string AssemblyParser::PRINT_COMMAND = "print";
const std::string AssemblyParser::JE_COMMAND = "je";




std::vector<std::bitset<8>> AssemblyParser::readAssembly(std::string assFilePath){

    std::cout << "Compiler: Parse Assembly File\n";



    std::vector<std::bitset<8>> zCode;
    std::vector<std::string> routineList = this->getRoutinesFromFile(assFilePath);

    getZCodeForRoutine(routineList.at(0));


    return  zCode;


}

std::vector<std::bitset<8>> AssemblyParser::getZCodeForRoutine(std::string routine)
{

    std::vector<std::string> commands = this->split(routine,'\n');
    std::cout << "\n";
    for(int i = 0;i<commands.size();i++)
    {
        std::cout << i <<": " << commands.at(i) << "\n";
        getCommandType(commands.at(i));
    }


}


std::string AssemblyParser::getCommandType(std::string command)
{
  std::vector<std::string> commandParts = this->split(command,' ');
  //  std::cout << command << commandParts.size();
    std::string s = "hello";
    return s;

}

std::vector<std::string> AssemblyParser::getRoutinesFromFile(std::string assFilePath)
{

    std::vector<std::string> routineList;
    std::string routine = "";
    std::ifstream input( assFilePath );


    //get all routines in own strings
    for( std::string command; getline( input, command ); )
    {

        if(command.size() != 0) {
            if (command.at(0) != ';') //comment symbol
            {
                if (checkIfCommandRoutineStart(command)== true) {
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
    if(routine.compare("") != 0)
    {
        std::cout << "routine: " << routine << " routine end" << "\n\n";
        routineList.push_back(routine);
    }

    std::cout << "size:"  << routineList.size();
    return routineList;

}



bool AssemblyParser::checkIfCommandRoutineStart(std::string command)
{
    std::vector<std::string> commandParts =  this->split(command,' ');
    for(int i = 0;i<commandParts.size();i++)
    {
        if(commandParts.at(i).compare(ROUTINE_COMMAND) == 0)
        {
            return true;
        }

    }
    return false;
}

std::vector<std::string> &AssemblyParser::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> AssemblyParser::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}